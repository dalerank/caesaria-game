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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "enemyarcher.hpp"
#include "core/position.hpp"
#include "pathway/astarpathfinding.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "city/city.hpp"
#include "core/variant.hpp"
#include "name_generator.hpp"
#include "core/stringhelper.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "pathway/pathway_helper.hpp"
#include "animals.hpp"
#include "spear.hpp"
#include "core/foreach.hpp"

using namespace constants;

class EnemyArcher::Impl
{
public:  
  unsigned int attackDistance;
  unsigned int wait;
};

EnemyArcher::EnemyArcher(PlayerCityPtr city)
  : EnemySoldier( city ), _d( new Impl )
{
  _setSubAction( EnemySoldier::check4attack );
  _d->attackDistance = 6;  
  _d->wait = 0;
}

bool EnemyArcher::_tryAttack()
{
  BuildingList buildings = _findBuildingsInRange( _d->attackDistance );
  if( !buildings.empty() )
  {
    _setSubAction( EnemySoldier::destroyBuilding );
    setSpeed( 0.f );
    _setAction( acFight );
    _setAnimation( _getAnimation( Walker::acFight ) );
    _changeDirection();
    return true;
  }
  else
  {
    WalkerList enemies = _findEnemiesInRange( _d->attackDistance );
    if( !enemies.empty() )
    {
      _setSubAction( EnemySoldier::fightEnemy );
      setSpeed( 0.f );
      _setAction( acFight );
      _setAnimation( _getAnimation( Walker::acFight ) );
      _changeDirection();
      return true;
    }
  }

  return false;
}

void EnemyArcher::_fire( TilePos p )
{
  SpearPtr spear = Spear::create( _getCity() );
  spear->toThrow( pos(), p );
  _d->wait = 30;
}

void EnemyArcher::timeStep(const unsigned long time)
{
  if( _d->wait > 0 )
  {
    _d->wait--;
    return;
  }

  Soldier::timeStep( time );

  switch( _getSubAction() )
  {
  case EnemySoldier::fightEnemy:
  {
    WalkerList enemies = _findEnemiesInRange( _d->attackDistance );

    if( !enemies.empty() )
    {
      WalkerPtr p = enemies.front();
      turn( p->pos() );

      if( _animationRef().index() == (int)(_animationRef().frameCount()-1) )
      {
        _fire( p->pos() );
        _updateAnimation( time+1 );
      }
    }
    else
    {
      _check4attack();
    }
  }
  break;

  case EnemySoldier::destroyBuilding:
  {
    BuildingList buildings = _findBuildingsInRange( _d->attackDistance );

    if( !buildings.empty() )
    {
      BuildingPtr b = buildings.front();
      turn( b->pos() );

      if( _animationRef().index() == (int)(_animationRef().frameCount()-1) )
      {
        _fire( b->pos() );
        _updateAnimation( time+1 );
      }
    }
    else
    {
      _check4attack();
    }
  }

  default: break;
  } // end switch( _d->action )
}

EnemyArcherPtr EnemyArcher::create(PlayerCityPtr city, constants::walker::Type type )
{
  EnemyArcherPtr ret( new EnemyArcher( city ) );
  ret->_init( type );
  ret->drop();

  return ret;
}

void EnemyArcher::load( const VariantMap& stream )
{
  EnemySoldier::load( stream );
}

void EnemyArcher::save( VariantMap& stream ) const
{
  Soldier::save( stream );
}
