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

#include "enemysoldier.hpp"
#include "core/position.hpp"
#include "game/astarpathfinding.hpp"
#include "game/path_finding.hpp"
#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "game/city.hpp"
#include "core/variant.hpp"
#include "game/name_generator.hpp"
#include "core/stringhelper.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "building/constants.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "game/pathway_helper.hpp"
#include "animals.hpp"

using namespace constants;

class EnemySoldier::Impl
{
public:
  typedef enum { check4attack=0,
                 go2position,
                 go2enemy,
                 fightEnemy,
                 destroyBuilding,
                 doNothing } EsAction;
    
  EsAction action;
  unsigned int strikeForce;
  unsigned int resistance;
  gfx::Type fightAnimation;
  gfx::Type walkAnimation;
};

EnemySoldier::EnemySoldier(PlayerCityPtr city )
: Soldier( city ), _d( new Impl )
{
  _d->action = Impl::check4attack;
}

void EnemySoldier::_changeTile()
{
  Walker::_changeTile();
}

bool EnemySoldier::_tryAttack()
{
  BuildingList buildings = _findBuildingsInRange( 1 );
  if( !buildings.empty() )
  {
    _d->action = Impl::destroyBuilding;
    setSpeed( 0.f );
    _setAction( acFight );
    _setAnimation( _d->fightAnimation );
    _changeDirection();
    return true;
  }
  else
  {
    WalkerList enemies = _findEnemiesInRange( 1 );
    if( !enemies.empty() )
    {
      _d->action = Impl::fightEnemy;
      setSpeed( 0.f );
      _setAction( acFight );
      _setAnimation( _d->fightAnimation );
      _changeDirection();
      return true;
    }
  }

  return false;
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
  switch( _d->action )
  {
  case Impl::check4attack:
  case Impl::go2position:
    _check4attack();
  break;

  default: break;
  }
}

WalkerList EnemySoldier::_findEnemiesInRange( unsigned int range )
{
  Tilemap& tmap = _getCity()->getTilemap();
  WalkerList walkers;

  TilePos offset( range, range );
  TilesArray tiles = tmap.getRectangle( getIJ() - offset, getIJ() + offset );

  walker::Type type;
  foreach( Tile* tile, tiles )
  {
    WalkerList tileWalkers = _getCity()->getWalkers( walker::any, tile->getIJ() );

    for( WalkerList::iterator i=tileWalkers.begin();i!=tileWalkers.end(); i++ )
    {
      type = (*i)->getType();
      if( type == getType() || (*i).is<Animal>() || type  == walker::corpse )
        continue;

      walkers.push_back( *i );
    }
  }

  return walkers;
}

void EnemySoldier::_init(walker::Type type)
{
  _setType( type );
  switch( type )
  {
  case walker::britonSoldier:
    _setAnimation( gfx::britonSoldier );
    _d->walkAnimation = gfx::britonSoldier;
    _d->fightAnimation = gfx::britonSoldierFight;
    _d->strikeForce = 3;
    _d->resistance = 1;
  break;
  default:
    _CAESARIA_DEBUG_BREAK_IF("not work yet");
  }

  setName( NameGenerator::rand( NameGenerator::male ) );
}

Pathway EnemySoldier::_findPathway2NearestEnemy( unsigned int range )
{
  Pathway ret;

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

  return Pathway();
}

void EnemySoldier::_check4attack()
{
  Pathway pathway = _findPathway2NearestEnemy( 20 );

  if( !pathway.isValid() )
  {
    pathway = _findPathway2NearestConstruction( 20 );
  }

  if( !pathway.isValid() )
  {
    pathway = PathwayHelper::create( getIJ(), _getCity()->getBorderInfo().boatExit,
                                     PathwayHelper::allTerrain );
  }

  if( !pathway.isValid() )
  {
    pathway = PathwayHelper::randomWay( _getCity(), getIJ(), 20 );
  }

  if( pathway.isValid() )
  {
    _d->action = Impl::go2position;
    setSpeed( 1.0 );
    _setAnimation( _d->walkAnimation );
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
  Tilemap& tmap = _getCity()->getTilemap();

  TilePos offset( range, range );
  TilesArray tiles = tmap.getRectangle( getIJ() - offset, getIJ() + offset );

  foreach( Tile* tile, tiles )
  {
    BuildingPtr b = tile->getOverlay().as<Building>();
    if( b.isValid() && b->getClass() != building::disasterGroup )
    {
      ret.push_back( b );
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

    foreach( BuildingPtr building, buildings )
    {
      ret = PathwayHelper::create( getIJ(), building.as<Construction>(), PathwayHelper::allTerrain );
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
  switch( _d->action )
  {
  case Impl::doNothing:
  break; 

  case Impl::check4attack:
  {
    _check4attack();
  }
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

void EnemySoldier::timeStep(const unsigned long time)
{
  setSpeed( 0.f );
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
      _check4attack();
    }
  }
  break;

  case Impl::destroyBuilding:
  {
    BuildingList buildings = _findBuildingsInRange( 1 );

    if( !buildings.empty() )
    {
      BuildingPtr b = buildings.front();

      turn( b->getTilePos() );
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

EnemySoldier::~EnemySoldier()
{
}

EnemySoldierPtr EnemySoldier::create(PlayerCityPtr city, constants::walker::Type type )
{
  EnemySoldierPtr ret( new EnemySoldier( city ) );
  ret->_init( type );
  ret->drop();

  return ret;
}

void EnemySoldier::send2City( TilePos pos )
{
  setIJ( pos );
  _check4attack();
  _getCity()->addWalker( WalkerPtr( this ));
}

void EnemySoldier::die()
{
  Soldier::die();

  switch( getType() )
  {
  case walker::britonSoldier:
    Corpse::create( _getCity(), getIJ(), ResourceGroup::celts, 393, 400 );
  break;

  default:
    _CAESARIA_DEBUG_BREAK_IF("not work yet");
  }
}

void EnemySoldier::load( const VariantMap& stream )
{
  Soldier::load( stream );
 
  _d->action = (Impl::EsAction)stream.get( "EsAction" ).toInt();

  _init( (walker::Type)stream.get( "type" ).toInt() );
  _getCity()->addWalker( this );
}

void EnemySoldier::save( VariantMap& stream ) const
{
  Soldier::save( stream );

  stream[ "type" ] = (int)getType();
  stream[ "animation" ] =
  stream[ "EsAction" ] = (int)_d->action;
  stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(EnemySoldier) ) );
}
