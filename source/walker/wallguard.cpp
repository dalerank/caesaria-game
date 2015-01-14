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
#include "city/helper.hpp"
#include "name_generator.hpp"
#include "corpse.hpp"
#include "core/variant_map.hpp"
#include "game/resourcegroup.hpp"
#include "objects/military.hpp"
#include "pathway/pathway_helper.hpp"
#include "gfx/tilemap.hpp"
#include "animals.hpp"
#include "enemysoldier.hpp"
#include "objects/tower.hpp"
#include "objects/fortification.hpp"
#include "walker/spear.hpp"
#include "core/foreach.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"
#include "walker/helper.hpp"

using namespace constants;
using namespace gfx;

class WallGuard::Impl
{
public:
  TowerPtr base;
  TilePos patrolPosition;
  double strikeForce, resistance;
};

WallGuard::WallGuard( PlayerCityPtr city, walker::Type type )
  : RomeSoldier( city, type ), _d( new Impl )
{
  setName( NameGenerator::rand( NameGenerator::male ) );

  setAttackDistance( 5 );
  _d->patrolPosition = TilePos( -1, -1 );
}

WallGuard::~WallGuard(){}

WallGuardPtr WallGuard::create(PlayerCityPtr city, walker::Type type)
{
  WallGuardPtr ret( new WallGuard( city, type ) );
  ret->drop();

  return ret;
}

bool WallGuard::die()
{
  bool created = Soldier::die();

  if( !created )
  {
    switch( type() )
    {
    case walker::romeGuard:
      Corpse::create( _city(), pos(), ResourceGroup::citizen3, 233, 240 );
      created = true;
    break;

    default:
      Logger::warning( "Wallguard::die() not work yet for this type " + WalkerHelper::getTypename( type() ) );
    }
  }

  return created;
}

void WallGuard::timeStep(const unsigned long time)
{
  Soldier::timeStep( time );

  switch( _subAction() )
  {
  case fightEnemy:
  {
    EnemySoldierList enemies;
    enemies << _findEnemiesInRange( attackDistance() );

    if( !enemies.empty() )
    {
      if( _animationRef().atEnd() )
      {
        EnemySoldierPtr p = _findNearbyEnemy( enemies );
        turn( p->pos() );
        _fire( p->pos() );
        _updateAnimation( time+1 );
      }
    }
    else
    {
      _back2base();
    }
  }
  break;

  case check4attack:
  {
    EnemySoldierList enemies;
    enemies << _findEnemiesInRange( attackDistance() );

    if( !enemies.empty() )
    {    
      fight();      
    }
    else
    {
      bool haveEnemies = _tryAttack();
      if( !haveEnemies )
      {
        _back2base();
      }
    }
  }
  break;

  case patrol:
    if( game::Date::isDayChanged() )
    {
      bool haveEnemies = _tryAttack();
      if( !haveEnemies )
      {
        _back2base();
      }
    }
  break;

  default: break;
  } // end switch( _d->action )
}

void WallGuard::fight()
{
  Soldier::fight();
  _setSubAction( Soldier::fightEnemy );
}

void WallGuard::save(VariantMap& stream) const
{
  Soldier::save( stream );

  stream[ "base" ] = _d->base.isValid() ? _d->base->pos() : TilePos( -1, -1 );
  stream[ "strikeForce" ] = _d->strikeForce;
  stream[ "resistance" ] = _d->resistance;
  stream[ "patrolPosition" ] = _d->patrolPosition;
  stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(WallGuard) ) );
}

void WallGuard::load(const VariantMap& stream)
{
  Soldier::load( stream );

  _d->strikeForce = stream.get( "strikeForce" );
  _d->resistance = stream.get( "resistance" );
  _d->patrolPosition = stream.get( "patrolPosition" );

  TilePos basePosition = stream.get( "base" );
  TowerPtr tower = ptr_cast<Tower>( _city()->getOverlay( basePosition ) );

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

std::string WallGuard::thoughts(Thought th) const
{
  switch( th )
  {
  case thCurrent:
  {
    city::Helper helper( _city() );

    TilePos offset( 10, 10 );
    EnemySoldierList enemies = helper.find<EnemySoldier>( walker::any, pos() - offset, pos() + offset );
    if( enemies.empty() )
    {
      return Soldier::thoughts(th);
    }
    else
    {
      return "##city_have_defence##";
    }
  }
  break;

  default: break;
  }

  return "";
}

TilePos WallGuard::places(Walker::Place type) const
{
  switch( type )
  {
  case plOrigin: return _d->base.isValid() ? _d->base->pos() : TilePos( -1, -1 );
  case plDestination: return _d->patrolPosition;
  default: break;
  }

  return RomeSoldier::places( type );
}


void WallGuard::setBase(TowerPtr tower)
{
  _d->base = tower;
}

FortificationList WallGuard::_findNearestWalls( EnemySoldierPtr enemy )
{
  FortificationList ret;

  Tilemap& tmap = _city()->tilemap();
  for( int range=1; range < 8; range++ )
  {
    TilePos offset( range, range );
    TilePos ePos = enemy->pos();
    TilesArray tiles = tmap.getRectangle( ePos - offset, ePos + offset );

    foreach( tile, tiles )
    {
      FortificationPtr f = ptr_cast<Fortification>( (*tile)->overlay() );
      if( f.isValid() && f->mayPatrol() )
      {
        ret.push_back( f );
      }
    }
  }

  return ret;
}

bool WallGuard::_tryAttack()
{
  EnemySoldierList enemies;
  enemies << _findEnemiesInRange( attackDistance() * 2 );

  if( !enemies.empty() )
  {
    //find nearest walkable wall
    EnemySoldierPtr soldierInAttackRange = _findNearbyEnemy( enemies );

    if( soldierInAttackRange.isValid() )
    {
      _setSubAction( fightEnemy );
      fight();
      _changeDirection();
    }
    else
    {
      PathwayPtr shortestWay;
      int minDistance = 999;
      foreach( it, enemies )
      {
        EnemySoldierPtr enemy = *it;
        FortificationList nearestWall = _findNearestWalls( enemy );

        PathwayList wayList = _d->base->getWays( pos(), nearestWall );
        foreach( way, wayList )
        {
          double tmpDistance = (*way)->stopPos().distanceFrom( enemy->pos() );
          if( tmpDistance < minDistance )
          {
            shortestWay = *way;
            minDistance = tmpDistance;
          }
        }
      }

      if( shortestWay.isValid() )
      {
        _updatePathway( *shortestWay.object() );
        _setSubAction( go2position );
        go();
        return true;
      }
    }
  }

  return false;
}

void WallGuard::_back2base()
{
  if( _d->base.isValid() )
  {
    _setSubAction( back2base );
    TilesArray enter = _d->base->enterArea();

    if( !enter.empty() )
    {
      Pathway way = _d->base->getWay( pos(), enter.front()->pos() );
      setPathway( way );
      go();
    }

    if( !_pathwayRef().isValid() )
    {
      deleteLater();
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

  switch( _subAction() )
  {

  case go2position:
  {
    bool haveEnemies = _tryAttack();
    if( !haveEnemies )
    {
      _back2base();
      _setSubAction( back2base );
    }
  }
  break;

  case back2base:
    deleteLater();
    _setSubAction( doNothing );
  break;

  default:
  break;
  }
}

void WallGuard::_brokePathway(TilePos p)
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
      _setSubAction( patrol );
      setPathway( Pathway() );
      wait( game::Date::days2ticks( 7 ) );
    }
  }
}

void WallGuard::_waitFinished()
{
  _setSubAction( check4attack );
}

void WallGuard::_fire( TilePos target )
{
  SpearPtr spear = Spear::create( _city() );
  spear->toThrow( pos(), target );
  wait( game::Date::days2ticks( 1 ) / 2 );
}

void WallGuard::_centerTile()
{
  switch( _subAction() )
  {
  case doNothing:
  break;

  case go2position:
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

void WallGuard::send2city( TowerPtr tower, Pathway pathway )
{
  setPos( pathway.startPos() );
  setBase( tower );

  setPathway( pathway );
  go();

  _setSubAction( go2position );

  if( !isDeleted() )
  {
    attach();
  }
}

EnemySoldierPtr WallGuard::_findNearbyEnemy(EnemySoldierList enemies )
{
  EnemySoldierPtr ret;
  double minDistance = 999;
  foreach( it, enemies )
  {
    double tmpDistance = pos().distanceFrom( (*it)->pos() );
    if( tmpDistance > 2 && tmpDistance < minDistance )
    {
      minDistance = tmpDistance;
      ret = *it;
    }
  }

  return ret;
}
