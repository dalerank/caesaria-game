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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "prefect.hpp"
#include "core/position.hpp"
#include "objects/prefecture.hpp"
#include "objects/house.hpp"
#include "pathway/astarpathfinding.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "city/statistic.hpp"
#include "core/variant_map.hpp"
#include "name_generator.hpp"
#include "core/utils.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "corpse.hpp"
#include "events/showinfobox.hpp"
#include "game/resourcegroup.hpp"
#include "events/disaster.hpp"
#include "pathway/pathway_helper.hpp"
#include "walker/helper.hpp"
#include "walkers_factory.hpp"
#include "gfx/helper.hpp"
#include "events/fireworkers.hpp"
#include "core/common.hpp"

using namespace gfx;
using namespace events;

REGISTER_CLASS_IN_WALKERFACTORY(walker::prefect, Prefect)

namespace {
  const Walker::Action acDragWater = Walker::Action( Walker::acMax + 1 );
  const Walker::Action acFightFire = Walker::Action( Walker::acMax + 2 );
}

class Prefect::Impl
{
public:
  int water;
  TilePos endPatrolPoint;
  Prefect::SbAction prefectAction;
  int fumigateHouseNumber;
};

Prefect::Prefect(PlayerCityPtr city )
  : ServiceWalker( city, Service::prefect ), _d( new Impl )
{
  _setType( walker::prefect );
  _d->water = 0;
  _d->fumigateHouseNumber = 0;
  _setSubAction( doNothing );  

  setName( NameGenerator::rand( NameGenerator::male ) );
}

bool Prefect::_looks4Fire( ReachedBuildings& buildings, TilePos& p )
{
  buildings = getReachedBuildings( pos() );

  BuildingPtr b = buildings.firstOf( object::burning_ruins );
  if( b.isValid() )
    p = b->pos();

  return b.isValid();
}

WalkerPtr Prefect::_looks4Enemy( const int range )
{
  TilePos offset( range, range );
  WalkerList walkers = _city()->statistic().walkers.find<Walker>( walker::any,
                                                                  pos() - offset, pos() + offset );

  walkers = utils::selectAgressive( walkers );
  return utils::findNearest( pos(), walkers );
}

bool Prefect::_checkPath2NearestFire( const ReachedBuildings& buildings )
{
  for( auto building : buildings )
  {
    if( building->type() != object::burning_ruins )
      continue;

    if( building->pos().distanceFrom( pos() ) < 1.5f )
    {
      turn( building->pos() );
      _setSubAction( fightFire );
      _setAction( acFightFire  );
      setSpeed( 0.f );
      return true;
    }
  }

  for( auto building : buildings )
  {
    if( building->type() != object::burning_ruins )
      continue;

    Pathway tmp = PathwayHelper::create( pos(), building, PathwayHelper::allTerrain );
    if( tmp.isValid() )
    {
      _setSubAction( go2fire );
      _updatePathway( tmp );      
      go();
      _setAction( acDragWater );
      return true;
    }
  }

  return false;
}

void Prefect::_back2Prefecture()
{
  auto prefecture = _map().overlay( baseLocation() ).as<Prefecture>();

  if( prefecture.isNull() )
  {
    Logger::warning( "!!! WARNING: Prefect lost base " );
    deleteLater();
    return;
  }

  TilesArray area = prefecture->enterArea();

  if( area.contain( pos() ) )
  {
    _setAction( Walker::acNone );
    deleteLater();
    return;
  }

  Pathway pathway = PathwayHelper::create( pos(), prefecture, PathwayHelper::roadFirst );

  if( pathway.isValid() )
  {
    _setSubAction( patrol );
    _updatePathway( pathway );
    _d->endPatrolPoint = pathway.stopPos();
    go();
  }
  else
  {
    deleteLater();
  }
}

void Prefect::_serveHouse( HousePtr house )
{
  if( !house.isValid() )
    return;

  int healthLevel = house->state( pr::health );
  if( healthLevel < 1 )
  {
    house->deleteLater();

    _d->fumigateHouseNumber++;
    house->removeHabitants( 1000 ); //all habitants will killed

    GameEventPtr e = Disaster::create( house->tile(), Disaster::plague );
    e->dispatch();

    if( _d->fumigateHouseNumber > 5 )
    {
      e = ShowInfobox::create( "##pestilence_event_title##", "##pestilent_event_text##",
                               ShowInfobox::send2scribe, "sick" );
      e->dispatch();
      _d->fumigateHouseNumber = -999;
    }

    if( _city()->getOption( PlayerCity::destroyEpidemicHouses ) )
    {
      HouseList hlist = _city()->statistic().objects.neighbors<House>( house );
      for( auto h : hlist )
      {
        GameEventPtr e = Disaster::create( h->tile(), Disaster::plague );
        e->dispatch();
      }
    }
  }
}

void Prefect::_serveBuildings( ReachedBuildings& reachedBuildings )
{        
  for( auto bld : reachedBuildings )
  {
    if( bld.isNull() ) continue;
    if( bld->isDeleted() ) continue;

    bld->applyService( this );

    _serveHouse( bld.as<House>() );
  }
}

void Prefect::_back2Patrol()
{
  Pathway pathway = PathwayHelper::create( pos(), _d->endPatrolPoint,
                                           PathwayHelper::allTerrain );

  if( pathway.isValid() )
  {
    _setSubAction( _d->water > 0 ? go2fire : patrol );
    _updatePathway( pathway );
    go();
    _setAction( _d->water > 0 ? acDragWater : acMove );
  }
  else
  {
    _back2Prefecture();
  }
}

void Prefect::_setSubAction( const Prefect::SbAction action)
{
  _d->prefectAction = action;

  switch( action )
  {
  case fightFire: _animation().setDelay( Animation::middle ); break;
  default: _animation().setDelay( Animation::fast ); break;
  }
}

bool Prefect::_figthFire()
{
  BuildingList buildings = _map().getNeighbors(pos(), Tilemap::AllNeighbors)
                                 .overlays()
                                 .select<Building>();

  for( auto building : buildings )
  {
    if( building->type() == object::burning_ruins )
    {
      turn( building->pos() );
      _setSubAction( fightFire );
      _d->endPatrolPoint = building->pos();
      _setAction( acFightFire );
      setSpeed( 0.f );
      return true;
    }
  }

  return false;
}

bool Prefect::_findFire()
{
  TilePos firePos;
  ReachedBuildings reachedBuildings;
  bool haveBurningRuinsNear = _looks4Fire( reachedBuildings, firePos );
  if( haveBurningRuinsNear && _d->water > 0 )
  {
    return _checkPath2NearestFire( reachedBuildings );
  }

  return false;
}

void Prefect::_brokePathway(TilePos p)
{
  OverlayPtr overlay = _map().overlay( p );

  if( overlay.isValid() && overlay->type() == object::burning_ruins )
  {
    setSpeed( 0.f );
    _setAction( acFightFire );
    _setSubAction( fightFire );
    return;
  }
  else if( _d->water > 0 )
  {
    TilePos destination = _pathway().stopPos();

    Pathway pathway = PathwayHelper::create( pos(), destination, PathwayHelper::allTerrain );
    if( pathway.isValid() )
    {
      _setSubAction( findFire );
      setPathway( pathway );
      go();
      _setAction( acDragWater );
      return;
    }
  }

  _back2Patrol();
}

void Prefect::_reachedPathway()
{
  TilesArray area;
  BuildingPtr base = _map().overlay( baseLocation() ).as<Building>();
  if( base.isValid() )
    area = base->enterArea();

  switch( _d->prefectAction )
  {
  case patrol:
    if( area.contain( pos() )  )
    {
      deleteLater();
      _setSubAction( doNothing );
    }
    else
    {
      _back2Prefecture();
    }
  break;

  case go2fire:
    if( !_figthFire() )
    {
      _d->water = 0;
      _back2Patrol();
    }
  break;

  case findFire:
    if( !_findFire() )
    {
      _d->water = 0;
      _back2Patrol();
    }
  break;

  default: break;
  }
}

void Prefect::_centerTile()
{
  switch( _d->prefectAction )
  {
  case doNothing:
  break; 

  case patrol:
  {
    TilePos firePos;
    ReachedBuildings reachedBuildings;
    WalkerPtr enemy = _looks4Enemy( 5 );
    bool haveBurningRuinsNear = _looks4Fire( reachedBuildings, firePos );

    if( enemy.isValid() )
    {      
      Pathway pathway = PathwayHelper::create( pos(), enemy->pos(), PathwayHelper::allTerrain );

      if( pathway.isValid() )
      {
        _updatePathway( pathway );
        go( 1.5f );

        _setSubAction( go2enemy );
      }
    }
    else if( haveBurningRuinsNear )
    {
      //tell our prefecture that need send prefect with water to fight with fire
      //on next deliverService

      //found fire, no water, go prefecture
      auto prefecture = _map().overlay<Prefecture>( baseLocation() );
      if( prefecture.isValid() )
        prefecture->fireDetect( firePos );

      _back2Prefecture();
    }
    else
    {
      _serveBuildings( reachedBuildings );
    }
  }
  break;

  case findFire:
  {
    _findFire();
  }
  break;

  case go2enemy:
  {
    WalkerPtr enemy = _looks4Enemy( 5 );
    if( enemy.isValid() )
    {
      if( enemy->pos().distanceFrom( pos() ) < 1.5f  )
      {
        turn( enemy->pos() );
        _setSubAction( fightEnemy );
        setSpeed( 0.f );
        _setAction( acFight );
        return;
      }
    }
    else
    {
      _back2Patrol();
    }
  }
  break;

  case go2fire:
  {
    if( _figthFire() )
      return;
  }
  break;

  case fightFire:
  case fightEnemy:
  break;
  }
  Walker::_centerTile();
}

void Prefect::_noWay()
{
  if( _d->water > 0 )
  {
    if( _findFire() )
      return;
  }

  _back2Prefecture();
}

static BuildingPtr isBurningRuins( const Tile& tile, bool& inFire )
{
  BuildingPtr building = tile.overlay<Building>();
  inFire = (building.isValid() && building->type() == object::burning_ruins );

  return inFire ? building : BuildingPtr();
}

void Prefect::timeStep(const unsigned long time)
{
  ServiceWalker::timeStep( time );

  switch( (int)action() )
  {
  case acDragWater:
    _walk();
    _updateAnimation( time );
  break;

  case acFightFire:
    _updateAnimation( time );
  break;

  default: break;
  }

  switch( _d->prefectAction )
  {

  case fightFire:
  {    
    bool inFire;
    BuildingPtr building = isBurningRuins( _nextTile(), inFire );

    if( !inFire )
      building = isBurningRuins( tile(), inFire );

    if( inFire )
    {
      const float beforeFight = building->evaluateService( this );
      building->applyService( this );
      const float afterFight = building->evaluateService( this );
      _d->water -= math::clamp( (int)(beforeFight - afterFight), 0, 100 );

      if( afterFight == 0)
      {
        inFire = false;
      }
    }

    if( !inFire && _d->water > 0 )
    {
      if( !_findFire() )
      {
        _back2Patrol();
      }
    }
    else if( _d->water <= 0 )
    {
      _back2Prefecture();
    }
  }
  break;

  case findFire:
  case go2fire:
      if( speed() == 0.f )
          _back2Patrol();
  break;

  case fightEnemy:
  {
    WalkerPtr enemy = _looks4Enemy(  1 );

    if( enemy.isValid() )
    {
      turn( enemy->pos() );

      enemy->updateHealth( -3 );
      enemy->acceptAction( Walker::acFight, pos() );
    }
    else
    {
      _back2Patrol();
    }
  }
  break;

  default: break;
  } // end switch( _d->action )
}

Prefect::~Prefect() {}

float Prefect::serviceValue() const {  return 5; }

PrefectPtr Prefect::create(PlayerCityPtr city )
{
  PrefectPtr ret( new Prefect( city ) );
  ret->initialize( WalkerHelper::getOptions( ret->type() ) );
  ret->drop();

  return ret;
}

void Prefect::send2City(PrefecturePtr prefecture, Prefect::SbAction action, int water/*=0 */ )
{
  _setSubAction( water > 0 ? findFire : patrol );
  _d->water = water;
  _setAction( water > 0 ? acDragWater : acMove );

  if( water > 0 )
  {
    setBase( prefecture.object() );
    attach();
  }
  else
  {
    ServiceWalker::send2City( prefecture.object() );
  }

  if( _pathway().isValid() )
  {
    _d->endPatrolPoint = _pathway().stopPos();
  }
}

void Prefect::send2City(BuildingPtr base, int orders)
{
  auto prefecture = base.as<Prefecture>();

  if( prefecture.isValid() )
  {
    send2City( prefecture, Prefect::patrol );
  }
  else
  {
    Logger::warning( "!!!Warning: Prefect try send from non prefecture building. Delete prefect.");
    deleteLater();
  }
}

void Prefect::acceptAction(Walker::Action action, TilePos pos)
{
  ServiceWalker::acceptAction( action, pos );

  _setAction( acFight );
  _setSubAction( fightEnemy );
}

bool Prefect::die()
{
  bool created = ServiceWalker::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::citizen2, 711, 718 );
    return true;
  }

  return created;
}

void Prefect::initialize(const VariantMap& options)
{
  ServiceWalker::initialize( options );
}

std::string Prefect::thoughts(Thought th) const
{
  switch( th )
  {
  case thCurrent:
    switch( _d->prefectAction )
    {
    case go2fire: return "##prefect_goto_fire##";
    case fightFire: return "##prefect_fight_fire##";
    default: break;
    }
  break;

  default:
  break;
  }

  return ServiceWalker::thoughts(th);
}

TilePos Prefect::places(Walker::Place type) const
{
  switch( type )
  {
  case plDestination: return _d->endPatrolPoint;
  default: break;
  }

  return ServiceWalker::places( type );
}

void Prefect::load( const VariantMap& stream )
{
  ServiceWalker::load( stream );
 
  VARIANT_LOAD_ENUM_D( _d, prefectAction, stream )
  VARIANT_LOAD_ANY_D( _d, water, stream )
  VARIANT_LOAD_ANY_D( _d, endPatrolPoint, stream )

  _setSubAction( _d->prefectAction );
  _setAction( _d->water > 0 ? acDragWater : acMove );

  auto prefecture = _map().overlay( baseLocation() ).as<Prefecture>();
  if( prefecture.isValid() )
  {
    prefecture->addWalker( this );
    attach();
  }
  else
  {
    Logger::warning( "Not found prefecture on loading" );
    deleteLater();
  }
}

void Prefect::save( VariantMap& stream ) const
{
  ServiceWalker::save( stream );

  stream[ "type" ] = (int)walker::prefect;
  VARIANT_SAVE_ENUM_D( stream, _d, prefectAction )
  VARIANT_SAVE_ANY_D( stream, _d, water )
  VARIANT_SAVE_ANY_D( stream, _d, endPatrolPoint )
  stream[ "__debug_typeName" ] = Variant( WalkerHelper::getTypename( type() ) );
}
