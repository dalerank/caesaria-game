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

#include "wallguard.hpp"
#include "game/city.hpp"
#include "game/name_generator.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "building/military.hpp"
#include "game/pathway_helper.hpp"
#include "gfx/tilemap.hpp"
#include "animals.hpp"
#include "enemysoldier.hpp"
#include "building/tower.hpp"
#include "building/fortification.hpp"

using namespace constants;

class WallGuard::Impl
{
public:
  typedef enum { doNothing=0, back2tower, go2position, fightEnemy,
                 patrol } State;
  TowerPtr base;
  State action;
  gfx::Type walk;
  gfx::Type fight;
  TilePos patrolPosition;
  double strikeForce, resistance;
};

WallGuard::WallGuard( PlayerCityPtr city, walker::Type type ) : Soldier( city ), _d( new Impl )
{
  _setType( type );
  _setAnimation( gfx::soldier );

  _init( type );
  _d->patrolPosition = TilePos( -1, -1 );
}

void WallGuard::_init( walker::Type type )
{
  _setType( type );
  switch( type )
  {
  case walker::romeGuard:
    _setAnimation( gfx::guardGo );
    _d->walk = gfx::guardGo;
    _d->fight = gfx::guardFigth;
    _d->strikeForce = 3;
    _d->resistance = 1;
  break;
  default:
    _CAESARIA_DEBUG_BREAK_IF( __FILE__": function not work yet");
  }

  setName( NameGenerator::rand( NameGenerator::male ) );
}

WallGuardPtr WallGuard::create(PlayerCityPtr city, walker::Type type)
{
  WallGuardPtr ret( new WallGuard( city, type ) );
  ret->drop();

  return ret;
}

void WallGuard::die()
{
  Soldier::die();

  switch( getType() )
  {
  case walker::romeGuard:
    Corpse::create( _getCity(), getIJ(), ResourceGroup::citizen3, 233, 240 );
  break;

  default:
    _CAESARIA_DEBUG_BREAK_IF("not work yet");
  }
}

void WallGuard::timeStep(const unsigned long time)
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
      turn( p->getIJ() );
      p->updateHealth( -3 );
      p->acceptAction( Walker::acFight, getIJ() );
    }
    else
    {
      _tryAttack();
    }
  }
  break;

  case Impl::patrol:
    if( time % 15 == 1 )
    {
      _tryAttack();
    }
  break;

  default: break;
  } // end switch( _d->action )
}

void WallGuard::save(VariantMap& stream) const
{
  Soldier::save( stream );

  stream[ "crtAction" ] = (int)_d->action;
  stream[ "base" ] = _d->base->getTilePos();
  stream[ "strikeForce" ] = _d->strikeForce;
  stream[ "resistance" ] = _d->resistance;
  stream[ "patrolPosition" ] = _d->patrolPosition;
  stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(WallGuard) ) );
}

void WallGuard::load(const VariantMap& stream)
{
  Soldier::load( stream );

  _d->action = (Impl::State)stream.get( "crtAction" ).toInt();
  _d->strikeForce = stream.get( "strikeForce" );
  _d->resistance = stream.get( "resistance" );
  _d->patrolPosition = stream.get( "patrolPosition" );

  TilePos basePosition = stream.get( "base" );
  TowerPtr tower = _getCity()->getOverlay( basePosition ).as<Tower>();

  if( tower.isValid() )
  {
    _d->base = tower;
    tower->addWalker( this );
  }
  else
  {
    die();
  }
}

WalkerList WallGuard::_findEnemiesInRange( unsigned int range )
{
  Tilemap& tmap = _getCity()->getTilemap();
  WalkerList walkers;

  TilePos offset( range, range );
  TilesArray tiles = tmap.getRectangle( getIJ() - offset, getIJ() + offset );

  foreach( Tile* tile, tiles )
  {
    WalkerList tileWalkers = _getCity()->getWalkers( walker::any, tile->getIJ() );

    foreach( WalkerPtr w, tileWalkers )
    {
      if( w.is<EnemySoldier>() )
      {
        walkers.push_back( w );
      }
    }
  }

  return walkers;
}

bool WallGuard::_tryAttack()
{
  WalkerList enemies = _findEnemiesInRange( 1 );
  if( !enemies.empty() )
  {
    _d->action = Impl::fightEnemy;
    setSpeed( 0.f );
    _setAction( acFight );
    _setAnimation( _d->fight );
    _changeDirection();
    return true;
  }

  return false;
}

Pathway WallGuard::_findPathway2NearestEnemy()
{
 /* Pathway ret;

  for( unsigned int tmpRange=1; tmpRange <= range; tmpRange++ )
  {
    WalkerList walkers = _findEnemiesInRange( tmpRange );

    foreach( WalkerPtr w, walkers)
    {
      ret = PathwayHelper::create( getIJ(), w->getIJ(), PathwayHelper::allTerrain );
      if( ret.isValid() )
      {
        return ret;
      }
    }
  }
*/
  return Pathway();
}

void WallGuard::_back2tower()
{
  if( _d->base.isValid() )
  {
    Pathway way = PathwayHelper::create( getIJ(), _d->base.as<Construction>(), PathwayHelper::allTerrain );

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

void WallGuard::_back2patrol()
{

}

void WallGuard::_reachedPathway()
{
  Soldier::_reachedPathway();

  switch( _d->action )
  {

  case Impl::go2position:
  {
    if( _getCity()->getWalkers( getType(), getIJ() ).size() != 1 ) //only me in this tile
    {
      _back2tower();
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

void WallGuard::_brokePathway(TilePos pos)
{
  Soldier::_brokePathway( pos );

  if( _d->patrolPosition.getI() >= 0 )
  {
    Pathway way = PathwayHelper::create( getIJ(), _d->patrolPosition,
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

void WallGuard::_centerTile()
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

void WallGuard::send2city( TowerPtr base, Pathway pathway )
{
  setIJ( pathway.getOrigin().getIJ() );
  _d->base = base;

  setPathway( pathway );
  go();

  if( !isDeleted() )
  {
    _getCity()->addWalker( this );
  }
}
