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
#include "game/resourcegroup.hpp"
#include "pathway/pathway_helper.hpp"
#include "helper.hpp"
#include "animals.hpp"
#include "throwing_weapon.hpp"
#include "core/foreach.hpp"

using namespace constants;
using namespace gfx;

class EnemySoldier::Impl
{
public:  
  EnemySoldier::EsAction action;
  unsigned int waitInterval;
};

EnemySoldier::EnemySoldier( PlayerCityPtr city, walker::Type type )
: Soldier( city, type ), _d( new Impl )
{
  _d->action = check4attack;
  _d->waitInterval = 0;
}

bool EnemySoldier::_tryAttack()
{
  BuildingList buildings = _findBuildingsInRange( 1 );
  if( !buildings.empty() )
  {
    _d->action = destroyBuilding;
    setSpeed( 0.f );
    _setAction( acFight );
    return true;
  }
  else
  {
    WalkerList enemies = _findEnemiesInRange( 1 );
    if( !enemies.empty() )
    {
      _d->action = fightEnemy;
      setSpeed( 0.f );
      _setAction( acFight );
      return true;
    }
  }

  return false;
}

void EnemySoldier::_setSubAction(EnemySoldier::EsAction action) {  _d->action = action; }
EnemySoldier::EsAction EnemySoldier::_getSubAction() const{  return _d->action; }

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
  switch( _d->action )
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

Pathway EnemySoldier::_findFreeSlot( const int range )
{
  for( int currentRange=0; currentRange <= range; currentRange++ )
  {
    TilePos offset( currentRange, currentRange );
    TilesArray tiles = _city()->tilemap().getRectangle( pos() - offset, pos() + offset );
    tiles = tiles.walkableTiles( true );

    foreach( itile, tiles )
    {
      Pathway pathway = PathwayHelper::create( pos(), (*itile)->pos(), PathwayHelper::allTerrain );
      if( pathway.isValid() )
      {
        return pathway;
      }
    }
  }

  return Pathway();
}

bool EnemySoldier::_move2freePos()
{
  const int defaultRange = 10;
  Pathway way2freeslot = _findFreeSlot( defaultRange );
  if( way2freeslot.isValid() )
  {
    _updatePathway( way2freeslot );
    go();
    _d->action = go2position;
    return true;
  }

  return false;
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
  Pathway pathway = _findPathway2NearestEnemy( 20 );

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
    pathway = PathwayHelper::randomWay( _city(), pos(), 20 );
  }

  if( pathway.isValid() )
  {
    _d->action = go2position;
    setSpeed( 1.0 );
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
      if( b.isValid() && b->getClass() != building::disasterGroup )
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

bool EnemySoldier::_isCurrentPosBusy()
{
  bool needMeMove = false;
  EnemySoldierList walkers;
  walkers << _city()->getWalkers( walker::all, pos() );
  foreach( it, walkers )
  {
    if( *it == this )
    {
      needMeMove = (it == walkers.begin());
      walkers.erase( it );
      break;
    }
  }

  return ( !walkers.empty() && needMeMove );
}

void EnemySoldier::_centerTile()
{
  bool isPosBusy = _isCurrentPosBusy();

  if( isPosBusy )
  {
    _move2freePos();
  }
  else
  {
    switch( _d->action )
    {
    case doNothing:
    break;

    case check4attack:
    {
      _check4attack();
    }
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
  }
  Walker::_centerTile();
}

void EnemySoldier::timeStep(const unsigned long time)
{
  if( _d->waitInterval > 0 )
  {
    _d->waitInterval--;
    return;
  }

  Soldier::timeStep( time );

  switch( _d->action )
  {
  case fightEnemy:
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
      _check4attack();
    }
  }
  break;

  case destroyBuilding:
  {
    BuildingList buildings = _findBuildingsInRange( 1 );

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

void EnemySoldier::die()
{
  Soldier::die();
  WalkerPtr wlk = Corpse::create( _city(), this );
  if( wlk->isDeleted() )
  {
     Corpse::create( _city(), pos(), ResourceGroup::celts, 393, 400 );
  }
}

void EnemySoldier::load( const VariantMap& stream )
{
  Soldier::load( stream );
 
  _d->action = (EsAction)stream.get( "EsAction" ).toInt();
  _d->waitInterval = (int)stream.get( "wait" );
}

void EnemySoldier::save( VariantMap& stream ) const
{
  Soldier::save( stream );

  stream[ "type" ] = (int)type();
  stream[ "animation" ] =
  stream[ "EsAction" ] = (int)_d->action;
  stream[ "wait" ] = _d->waitInterval;
  stream[ "__debug_typeName" ] = Variant( WalkerHelper::getTypename( type() ) );
}

void EnemySoldier::wait(unsigned int time)
{
  _d->waitInterval = time;
}
