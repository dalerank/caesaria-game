// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "romesoldier.hpp"
#include "city/city.hpp"
#include "name_generator.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "objects/military.hpp"
#include "pathway/pathway_helper.hpp"
#include "gfx/tilemap.hpp"
#include "animals.hpp"
#include "enemysoldier.hpp"
#include "core/foreach.hpp"
#include "game/gamedate.hpp"

using namespace constants;
using namespace gfx;

class RomeSoldier::Impl
{
public:
  typedef enum { doNothing=0, back2fort, go2position, fightEnemy,
                 patrol } State;
  FortPtr base;
  State action;
  TilePos patrolPosition;
  double strikeForce, resistance;
};

RomeSoldier::RomeSoldier( PlayerCityPtr city, walker::Type type )
    : Soldier( city, type ), _d( new Impl )
{
  setName( NameGenerator::rand( NameGenerator::male ) );

  _d->patrolPosition = TilePos( -1, -1 );
}

RomeSoldierPtr RomeSoldier::create(PlayerCityPtr city, walker::Type type)
{
  RomeSoldierPtr ret( new RomeSoldier( city, type ) );
  ret->drop();

  return ret;
}

void RomeSoldier::die()
{
  Soldier::die();

  switch( type() )
  {
  case walker::legionary:
    Corpse::create(_city(), this );
  break;

  default:
    _CAESARIA_DEBUG_BREAK_IF("not work yet");
  }
}

void RomeSoldier::timeStep(const unsigned long time)
{
  Soldier::timeStep( time );

  switch( _d->action )
  {
  case Impl::fightEnemy:
  {
    WalkerList enemies = _findEnemiesInRange( 1 );

    if( !enemies.empty() )
    {
      WalkerPtr p = enemies.front();
      turn( p->pos() );
      p->updateHealth( -3 );
      p->acceptAction( Walker::acFight, pos() );
    }
    else
    {
      _tryAttack();
    }
  }
  break;

  case Impl::patrol:
    if( GameDate::current().day() % 2 == 0 )
    {
      _tryAttack();
    }
  break;

  default: break;
  } // end switch( _d->action )
}

void RomeSoldier::send2patrol()
{
  _back2fort();
}

void RomeSoldier::save(VariantMap& stream) const
{
  Soldier::save( stream );

  stream[ "crtAction" ] = (int)_d->action;
  stream[ "base" ] = _d->base->pos();
  stream[ "strikeForce" ] = _d->strikeForce;
  stream[ "resistance" ] = _d->resistance;
  stream[ "patrolPosition" ] = _d->patrolPosition;
  stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(RomeSoldier) ) );
}

void RomeSoldier::load(const VariantMap& stream)
{
  Soldier::load( stream );

  _d->action = (Impl::State)stream.get( "crtAction" ).toInt();
  _d->strikeForce = stream.get( "strikeForce" );
  _d->resistance = stream.get( "resistance" );
  _d->patrolPosition = stream.get( "patrolPosition" );

  TilePos basePosition = stream.get( "base" );
  FortPtr fort = ptr_cast<Fort>( _city()->getOverlay( basePosition ) );

  if( fort.isValid() )
  {
    _d->base = fort;
    fort->addWalker( this );
  }
  else
  {
    die();
  }
}

WalkerList RomeSoldier::_findEnemiesInRange( unsigned int range )
{
  Tilemap& tmap = _city()->tilemap();
  WalkerList walkers;

  TilePos offset( range, range );
  TilesArray tiles = tmap.getRectangle( pos() - offset, pos() + offset );

  foreach( tile, tiles )
  {
    WalkerList tileWalkers = _city()->getWalkers( walker::any, (*tile)->pos() );

    foreach( w, tileWalkers )
    {
      if( is_kind_of<EnemySoldier>( *w ) )
      {
        walkers.push_back( *w );
      }
    }
  }

  return walkers;
}

bool RomeSoldier::_tryAttack()
{
  WalkerList enemies = _findEnemiesInRange( 1 );
  if( !enemies.empty() )
  {
    _d->action = Impl::fightEnemy;
    setSpeed( 0.f );
    _setAction( acFight );
    _changeDirection();
    return true;
  }

  return false;
}

Pathway RomeSoldier::_findPathway2NearestEnemy( unsigned int range )
{
  Pathway ret;

  for( unsigned int tmpRange=1; tmpRange <= range; tmpRange++ )
  {
    WalkerList walkers = _findEnemiesInRange( tmpRange );

    foreach( w, walkers)
    {
      ret = PathwayHelper::create( pos(), (*w)->pos(), PathwayHelper::allTerrain );
      if( ret.isValid() )
      {
        return ret;
      }
    }
  }

  return Pathway();
}

void RomeSoldier::_back2fort()
{
  if( _d->base.isValid() )
  {
    Pathway way = PathwayHelper::create( pos(), _d->base->getFreeSlot(), PathwayHelper::allTerrain );

    if( way.isValid() )
    {
      setPathway( way );
      _d->action = Impl::go2position;
      go();
      return;
    }
  }
  else
  {
    die();
  }
}

void RomeSoldier::_reachedPathway()
{
  Soldier::_reachedPathway();

  switch( _d->action )
  {

  case Impl::go2position:
  {
    if( _city()->getWalkers( type(), pos() ).size() != 1 ) //only me in this tile
    {
      _back2fort();
    }
    else
    {
      _d->action = Impl::patrol;
    }
  }
  break;

  default:
  break;
  }
}

void RomeSoldier::_brokePathway(TilePos p)
{
  Soldier::_brokePathway( p );

  if( _d->patrolPosition.i() >= 0 )
  {
    Pathway way = PathwayHelper::create( pos(), _d->patrolPosition,
                                         PathwayHelper::allTerrain );

    if( way.isValid() )
    {
      setPathway( way );
      go();
    }
    else
    {
      _d->action = Impl::patrol;
      _setAction( acNone );
      setPathway( Pathway() );
    }
  }
}

void RomeSoldier::_centerTile()
{
  switch( _d->action )
  {
  case Impl::doNothing:
  break;

  case Impl::go2position:
  {
    if( _tryAttack() )
      return;
  }
  break;

  default:
  break;
  }

  Walker::_centerTile();
}

void RomeSoldier::send2city(FortPtr base, TilePos pos )
{
  setPos( pos );
  _d->base = base;
  _back2fort();

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}
