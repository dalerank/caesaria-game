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
#include "helper.hpp"
#include "core/foreach.hpp"

using namespace constants;

EnemyArcher::EnemyArcher(PlayerCityPtr city, walker::Type type )
  : EnemySoldier( city, type )
{
  _setSubAction( Soldier::check4attack );
  setAttackDistance( 6 );
}

void EnemyArcher::_fire( TilePos p )
{
  SpearPtr spear = Spear::create( _city() );
  spear->toThrow( pos(), p );
  wait( 30 );
}

void EnemyArcher::_waitFinished()
{
  _setSubAction( check4attack );
}

void EnemyArcher::timeStep(const unsigned long time)
{
  Soldier::timeStep( time );

  switch( _subAction() )
  {
  case Soldier::fightEnemy:
  {
    WalkerList enemies = _findEnemiesInRange( attackDistance() );

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

  case Soldier::destroyBuilding:
  {
    BuildingList buildings = _findBuildingsInRange( attackDistance() );

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
  EnemyArcherPtr ret( new EnemyArcher( city, type ) );
  ret->initialize( WalkerHelper::getOptions( type ) );
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
