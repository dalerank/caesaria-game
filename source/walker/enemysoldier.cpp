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

#include "enemysoldier.hpp"
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
#include "city/helper.hpp"
#include "game/resourcegroup.hpp"
#include "pathway/pathway_helper.hpp"
#include "helper.hpp"
#include "animals.hpp"
#include "throwing_weapon.hpp"
#include "core/foreach.hpp"

using namespace constants;
using namespace gfx;

EnemySoldier::EnemySoldier( PlayerCityPtr city, walker::Type type )
: Soldier( city, type )
{
  _setSubAction( check4attack );
  setAttackDistance( 1 );
}

bool EnemySoldier::_tryAttack()
{
  WalkerList enemies = _findEnemiesInRange( attackDistance() );

  TilePos targetPos;
  if( !enemies.empty() )
  {
    _setSubAction( Soldier::fightEnemy );
    targetPos = enemies.front()->pos();
    fight();
  }
  else
  {
    BuildingList buildings = _findBuildingsInRange( attackDistance() );
    if( !buildings.empty() )
    {
      _setSubAction( Soldier::destroyBuilding );
      targetPos = buildings.front()->pos();
      fight();
    }
  }

  if( action() == acFight )
  {
    city::Helper helper( _city() );
    bool needMeMove = false;
    helper.isTileBusy<EnemySoldier>( pos(), this, needMeMove );
    if( needMeMove )
    {
      _move2freePos( targetPos );
    }
  }

  return action() == acFight;
}

void EnemySoldier::_waitFinished()
{
  _setSubAction( check4attack );
}

void EnemySoldier::_brokePathway(TilePos pos)
{
  if( !_tryAttack() )
  {
    _check4attack();
  }
}

void EnemySoldier::_reachedPathway()
{
  Soldier::_reachedPathway();
  switch( _subAction() )
  {
  case check4attack:
  case go2position:
  {
    bool findAny4attack = _tryAttack();
    if( !findAny4attack )
    {
      _check4attack();
    }
  }
  break;

  default: break;
  }
}

WalkerList EnemySoldier::_findEnemiesInRange( unsigned int range )
{
  Tilemap& tmap = _city()->tilemap();
  WalkerList walkers;

  for( unsigned int k=0; k <= range; k ++ )
  {
    TilePos offset( k, k );
    TilesArray tiles = tmap.getRectangle( pos() - offset, pos() + offset );

    walker::Type rtype;
    foreach( tile, tiles )
    {
      WalkerList tileWalkers = _city()->getWalkers( walker::any, (*tile)->pos() );

      foreach( i, tileWalkers )
      {
        rtype = (*i)->type();
        if( rtype == type() || is_kind_of<Animal>(*i) || rtype  == walker::corpse
            || is_kind_of<EnemySoldier>(*i) || is_kind_of<ThrowingWeapon>(*i))
          continue;

        walkers.push_back( *i );
      }
    }
  }

  return walkers;
}

Pathway EnemySoldier::_findPathway2NearestEnemy( unsigned int range )
{
  Pathway ret;

  for( unsigned int tmpRange=1; tmpRange <= range; tmpRange++ )
  {
    WalkerList walkers = _findEnemiesInRange( tmpRange );

    foreach( it, walkers)
    {
      ret = PathwayHelper::create( pos(), (*it)->pos(), PathwayHelper::allTerrain );
      if( ret.isValid() )
      {
        return ret;
      }
    }
  }

  return Pathway();
}

void EnemySoldier::_check4attack()
{
  //try find any walkers in range
  Pathway pathway = _findPathway2NearestEnemy( 10 );

  if( !pathway.isValid() )
  {
    int size = _city()->tilemap().size();
    pathway = _findPathway2NearestConstruction( size/2 );
  }   

  if( !pathway.isValid() )
  {
    pathway = PathwayHelper::create( pos(), _city()->borderInfo().roadExit,
                                     PathwayHelper::allTerrain );
  }

  if( !pathway.isValid() )
  {
    pathway = PathwayHelper::randomWay( _city(), pos(), 10 );
  }

  if( pathway.isValid() )
  {
    _setSubAction( go2position );
    setPathway( pathway );
    go();
  }
  else
  {
    //impossible state, but...
    Logger::warning( "EnemySoldier: can't find any path" );
    die();
  }
}

BuildingList EnemySoldier::_findBuildingsInRange( unsigned int range )
{
  BuildingList ret;
  Tilemap& tmap = _city()->tilemap();

  for( unsigned int k=0; k <= range; k++ )
  {
    TilePos offset( k, k );
    TilesArray tiles = tmap.getRectangle( pos() - offset, pos() + offset );

    foreach( it, tiles )
    {
      BuildingPtr b = ptr_cast<Building>( (*it)->overlay() );
      if( b.isValid() && b->group() != building::disasterGroup )
      {
        ret.push_back( b );
      }
    }
  }

  return ret;
}

Pathway EnemySoldier::_findPathway2NearestConstruction( unsigned int range )
{
  Pathway ret;

  for( unsigned int tmpRange=1; tmpRange <= range; tmpRange++ )
  {
    BuildingList buildings = _findBuildingsInRange( tmpRange );

    foreach( it, buildings )
    {
      ConstructionPtr c = ptr_cast<Construction>( *it );
      ret = PathwayHelper::create( pos(), c, PathwayHelper::allTerrain );
      if( ret.isValid() )
      {
        return ret;
      }
    }
  }

  return Pathway();
}

void EnemySoldier::_centerTile()
{
  switch( _subAction() )
  {
  case doNothing:
  break;

  case check4attack: _check4attack(); break;

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

void EnemySoldier::timeStep(const unsigned long time)
{
  Soldier::timeStep( time );

  switch( _subAction() )
  {
  case fightEnemy:
  {
    WalkerList enemies = _findEnemiesInRange( attackDistance() );

    if( !enemies.empty() )
    {
      WalkerPtr p = enemies.front();
      turn( p->pos() );
      p->updateHealth( -3 );
      p->acceptAction( Walker::acFight, pos() );
    }
    else
    {
      _check4attack();
    }
  }
  break;

  case destroyBuilding:
  {
    BuildingList buildings = _findBuildingsInRange( attackDistance() );

    if( !buildings.empty() )
    {
      BuildingPtr b = buildings.front();

      turn( b->pos() );
      b->updateState( Construction::damage, 1 );
    }
    else
    {
      _check4attack();
    }
  }

  default: break;
  } // end switch( _d->action )
}

EnemySoldier::~EnemySoldier() {}

int EnemySoldier::agressive() const { return 2; }

EnemySoldierPtr EnemySoldier::create(PlayerCityPtr city, constants::walker::Type type )
{
  EnemySoldierPtr ret( new EnemySoldier( city, type ) );
  ret->setName( NameGenerator::rand( NameGenerator::male ) );
  ret->initialize( WalkerHelper::getOptions( type ) );
  ret->drop();

  return ret;
}

void EnemySoldier::send2City( TilePos pos )
{
  setPos( pos );
  _check4attack();
  _city()->addWalker( this );
}

bool EnemySoldier::die()
{
  bool created = Soldier::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::celts, 393, 400 );
    return true;
  }

  return created;
}

void EnemySoldier::acceptAction(Walker::Action action, TilePos pos)
{
  Soldier::acceptAction( action, pos );
  if( action == Walker::acFight )
  {
    _tryAttack();
  }
}

void EnemySoldier::load( const VariantMap& stream )
{
  Soldier::load( stream );
}

void EnemySoldier::save( VariantMap& stream ) const
{
  Soldier::save( stream );

  stream[ "type" ] = (int)type();
  stream[ "__debug_typeName" ] = Variant( WalkerHelper::getTypename( type() ) );
}
