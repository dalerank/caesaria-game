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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "prefect.hpp"
#include "core/position.hpp"
#include "objects/prefecture.hpp"
#include "objects/house.hpp"
#include "pathway/astarpathfinding.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "city/helper.hpp"
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
#include "events/fireworkers.hpp"

using namespace constants;
using namespace gfx;

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
  Prefect::SbAction action;
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

bool Prefect::_looks4Fire( ServiceWalker::ReachedBuildings& buildings, TilePos& p )
{
  buildings = getReachedBuildings( pos() );

  foreach( it, buildings )
  {
    if( (*it)->type() == objects::burning_ruins )
    {
      p = (*it)->pos();
      return true;
    }
  }

  return false;
}

WalkerPtr Prefect::_looks4Enemy( const int range )
{
  TilePos offset( range, range );
  city::Helper helper( _city() );
  WalkerList walkers = helper.find<Walker>( walker::any, pos() - offset, pos() + offset );

  for( WalkerList::iterator it = walkers.begin(); it != walkers.end(); )
  {
    if( (*it)->agressive() <= 0 ) { it = walkers.erase( it ); }
    else { ++it; }
  }

  WalkerPtr enemy = findNearestWalker( pos(), walkers );

  return enemy;
}

bool Prefect::_checkPath2NearestFire( const ReachedBuildings& buildings )
{
  foreach( it, buildings )
  {
    BuildingPtr building = *it;
    if( building->type() != objects::burning_ruins )
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

  foreach( it, buildings )
  {
    BuildingPtr building = *it;
    if( building->type() != objects::burning_ruins )
      continue;

    Pathway tmp = PathwayHelper::create( pos(), ptr_cast<Construction>( building ), PathwayHelper::allTerrain );
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
  TilesArray area = base()->enterArea();
  if( area.contain( pos() ) )
  {
    _setAction( Walker::acNone );
    deleteLater();
    return;
  }

  Pathway pathway = PathwayHelper::create( pos(), ptr_cast<Construction>( base() ),
                                           PathwayHelper::roadFirst );  

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

void Prefect::_serveBuildings( ReachedBuildings& reachedBuildings )
{
  foreach( it, reachedBuildings )
  {
    BuildingPtr building = *it;
    building->applyService( ServiceWalkerPtr( this ) );

    HousePtr house = ptr_cast<House>( building );
    if( house.isValid() )
    {
      int healthLevel = house->state( (Construction::Param)House::health );
      if( healthLevel < 1 )
      {
        house->deleteLater();

        _d->fumigateHouseNumber++;
        CitizenGroup citizens = house->remHabitants( 1000 ); //all habitants will killed
        events::GameEventPtr e = events::FireWorkers::create( house->pos(), citizens.count() );
        e->dispatch();

        e = events::Disaster::create( house->tile(), events::Disaster::plague );
        e->dispatch();

        if( _d->fumigateHouseNumber > 5 )
        {
          e = events::ShowInfobox::create( "##pestilence_event_title##", "##pestilent_event_text##",
                                           events::ShowInfobox::send2scribe, "/smk/SICK.SMK" );
          e->dispatch();
          _d->fumigateHouseNumber = -999;
        }
      }
    }
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
  _d->action = action;

  switch( action )
  {
  case fightFire: _animationRef().setDelay( 2 ); break;
  default: _animationRef().setDelay( 1 ); break;
  }
}

bool Prefect::_figthFire()
{
  TilesArray tiles = _city()->tilemap().getNeighbors(pos(), Tilemap::AllNeighbors);

  foreach( it, tiles )
  {
    BuildingPtr building = ptr_cast<Building>( (*it)->overlay() );
    if( building.isValid() && building->type() == objects::burning_ruins )
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
  TileOverlayPtr overlay = _city()->getOverlay( p );
  if( overlay.isValid() && overlay->type() == objects::burning_ruins )
  {
    setSpeed( 0.f );
    _setAction( acFightFire );
    _setSubAction( fightFire );
    return;
  }
  else if( _d->water > 0 )
  {
    TilePos destination = _pathwayRef().stopPos();

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
  switch( _d->action )
  {
  case patrol:
    if( base()->enterArea().contain( pos() )  )
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
  switch( _d->action )
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
      if( base().isValid() )
      {
        PrefecturePtr ptr = ptr_cast<Prefecture>( base() );
        if( ptr.isValid() )
          ptr->fireDetect( firePos );

        _back2Prefecture();
      }
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

  switch( _d->action )
  {

  case fightFire:
  {    
    BuildingPtr building = ptr_cast<Building>( _nextTile().overlay() );
    bool inFire = (building.isValid() && building->type() == objects::burning_ruins );

    if( inFire )
    {
      ServiceWalkerPtr ptr( this );
      const float beforeFight = building->evaluateService( ptr );
      building->applyService( ptr );
      const float afterFight = building->evaluateService( ptr );
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

    _city()->addWalker( this );
  }
  else
  {
    ServiceWalker::send2City( prefecture.object() );
  }

  if( _pathwayRef().isValid() )
  {
    _d->endPatrolPoint = _pathwayRef().stopPos();
  }
}

void Prefect::send2City(BuildingPtr base, int orders)
{
  PrefecturePtr prefecture = ptr_cast<Prefecture>( base );

  if( prefecture.isValid() )
  {
    send2City( prefecture, Prefect::patrol );
  }
  else
  {
    Logger::warning( "WARNING !!!: Prefect try send from non prefecture building. Delete prefect.");
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
    switch( _d->action )
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
 
  _setSubAction( (SbAction)stream.get( "prefectAction" ).toInt() );
  VARIANT_LOAD_ANY_D( _d, water, stream );
  VARIANT_LOAD_ANY_D( _d, endPatrolPoint, stream );

  _setAction( _d->water > 0 ? acDragWater : acMove );

  PrefecturePtr prefecture = ptr_cast<Prefecture>( base() );
  if( prefecture.isValid() )
  {
    prefecture->addWalker( WalkerPtr( this ) );
    _city()->addWalker( WalkerPtr( this ) );
  }
  
  if( prefecture.isNull() )
  {
    Logger::warning( "Not found prefecture on loading" );
    deleteLater();
  }
}

void Prefect::save( VariantMap& stream ) const
{
  ServiceWalker::save( stream );

  stream[ "type" ] = (int)walker::prefect;
  stream[ "prefectAction" ] = (int)_d->action;
  VARIANT_SAVE_ANY_D( stream, _d, water );
  VARIANT_SAVE_ANY_D( stream, _d, endPatrolPoint );
  stream[ "__debug_typeName" ] = Variant( WalkerHelper::getTypename( type() ) );
}
