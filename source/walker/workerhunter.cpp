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

#include "workerhunter.hpp"
#include "objects/house.hpp"
#include "core/predefinitions.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "city/city.hpp"
#include "core/variant_map.hpp"
#include "game/resourcegroup.hpp"
#include "pathway/pathway_helper.hpp"
#include "pathway/path_finding.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "objects/working.hpp"
#include "helper.hpp"
#include "gfx/helper.hpp"
#include "walkers_factory.hpp"

REGISTER_CLASS_IN_WALKERFACTORY(walker::recruter, Recruter)

enum class ReachDistance : unsigned short
{
  max=2
};

enum class HirePriority : unsigned short
{
  no = 999
};

class Recruter::Impl
{
public:
  typedef std::map< object::Group, int > PriorityMap;

  unsigned int needWorkers;
  city::HirePriorities priority;
  PriorityMap priorityMap;
  unsigned int reachDistance;
  bool patrolFinished;
  int failedCounter;
  bool once_shot;

public:
  bool isMyPriorityOver(BuildingPtr base, WorkingBuildingPtr wbuilding );
};

Recruter::Recruter(PlayerCityPtr city )
 : ServiceWalker( city, Service::recruter ), _d( new Impl )
{    
  _d->needWorkers = 0;
  _d->reachDistance = (int)ReachDistance::max;
  _d->once_shot = false;
  _d->failedCounter = 0;
  _d->patrolFinished = false;
  _setType( walker::recruter );
}

void Recruter::hireWorkers( const int workers )
{
  auto parentBuilding = base().as<WorkingBuilding>();
  if( parentBuilding.isValid() )
  {
    unsigned int reallyHire = parentBuilding->addWorkers( workers );
    _d->needWorkers -= reallyHire;
  }
  else
  {
    Logger::warning( "!!! WARNING: Recruter base[%d,%d] is null. Stop working.", baseLocation().i(), baseLocation().j() );
    return2Base();
  }
}

void Recruter::setPriority(const city::HirePriorities& priority)
{
  _d->priority = priority;

  int priorityLevel = 1;
  for( auto& priority : _d->priority )
  {
    object::Groups groups = city::industry::toGroups( priority );
    for( auto group : groups )
    {
      _d->priorityMap[ group ] = priorityLevel;
    }

    priorityLevel++;
  }
}

int Recruter::needWorkers() const { return _d->needWorkers; }

void Recruter::_centerTile()
{
  Walker::_centerTile();
  BuildingPtr refBase = base();

  if( refBase.isNull() )
  {
    Logger::warning( "!!! WARNING: Recruter haveno base" );
    return;
  }

  ReachedBuildings reached = getReachedBuildings( pos() );
  if( _d->needWorkers )
  {
    UqBuildings<House> houses = reached.select<House>();

    for( auto house : houses )
      house->applyService( this );

    if( !_d->priority.empty() )
    {
      UqBuildings<WorkingBuilding> buildings = reached.select<WorkingBuilding>();

      for( auto bld : buildings )
      {
        if( bld.equals( refBase ) ) //avoid recruting from out base
          continue;

        bool priorityOver = _d->isMyPriorityOver( refBase, bld );
        if( priorityOver )
        {
          int removedFromWb = bld->removeWorkers( _d->needWorkers );
          hireWorkers( removedFromWb );
        }
      }
    }
  }
  else
  {    
    if( !_pathway().isReverse() ) //return2Base();
    {
      _pathway().toggleDirection();
    }
  }
}

void Recruter::_noWay()
{
  _d->failedCounter++;

  Pathway newway;
  if( _d->failedCounter > 5 )
  {
    Logger::warning( "!!! WARNING: Failed find way for recruter " + name() );
    die();
  }

  newway = PathwayHelper::create( pos(), base(), PathwayHelper::roadFirst );

  if( newway.isValid() )
  {
    setPathway( newway );
    _d->failedCounter = 0;
    _d->patrolFinished = true;
    go();
  }
}

RecruterPtr Recruter::create(PlayerCityPtr city )
{ 
  RecruterPtr ret( new Recruter( city ) );
  ret->initialize( WalkerHelper::getOptions( ret->type() ) );

  ret->drop();
  return ret;
}

void Recruter::send2City( WorkingBuildingPtr building, const int workersNeeded )
{
  _d->needWorkers = workersNeeded;
  ServiceWalker::send2City( building.object(), ServiceWalker::goServiceMaximum | ServiceWalker::anywayWhenFailed );
}

void Recruter::send2City(BuildingPtr base, int orders)
{
  auto parentBuilding = base.as<WorkingBuilding>();
  if( parentBuilding.isValid() )
  {
    send2City( parentBuilding, parentBuilding->needWorkers() );
  }
  else
  {
    Logger::warning( "!!!WARNING: Recruter try hire workers for non working buildng. Delete rectuter.");
    deleteLater();
  }
}

void Recruter::once(WorkingBuildingPtr building, const unsigned int workersNeed, unsigned int distance )
{
  _d->needWorkers = workersNeed;
  _d->reachDistance = distance;
  _d->once_shot = true;

  setBase( building );
  setPos( building->pos() );
  _centerTile();
}

void Recruter::timeStep(const unsigned long time)
{
  ServiceWalker::timeStep( time );
}

TilePos Recruter::places(Walker::Place type) const
{
  switch( type )
  {
  case plOrigin: return baseLocation();
  default: break;
  }

  return ServiceWalker::places( type );
}

unsigned int Recruter::reachDistance() const { return _d->reachDistance;}

void Recruter::save(VariantMap& stream) const
{
  ServiceWalker::save( stream );
  VARIANT_SAVE_CLASS_D( stream, _d, priority )
  VARIANT_SAVE_ANY_D( stream, _d, needWorkers )
  VARIANT_SAVE_ANY_D( stream, _d, failedCounter )
  VARIANT_SAVE_ANY_D( stream, _d, patrolFinished )
}

void Recruter::load(const VariantMap& stream)
{
  ServiceWalker::load( stream );
  VARIANT_LOAD_ANY_D( _d, needWorkers, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, priority, stream )
  VARIANT_LOAD_ANY_D( _d, failedCounter, stream )
  VARIANT_LOAD_ANY_D( _d, patrolFinished, stream )
}

bool Recruter::die()
{
  if( _d->once_shot )
  {
    deleteLater();
    return true;
  }

  bool created = ServiceWalker::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::citizen1, 97, 104 );
    return true;
  }

  return created;
}

void Recruter::_reachedPathway()
{
  if( _d->patrolFinished )
  {
    deleteLater();
  }

  ServiceWalker::_reachedPathway();
}

bool Recruter::Impl::isMyPriorityOver(BuildingPtr base, WorkingBuildingPtr wbuilding)
{
  PriorityMap::iterator myPrIt = priorityMap.find( base->group() );
  PriorityMap::iterator bldPrIt = priorityMap.find( wbuilding->group() );
  int mypriority = (myPrIt != priorityMap.end() ? myPrIt->second : (int)HirePriority::no);
  int wpriority = (bldPrIt != priorityMap.end() ? bldPrIt->second : (int)HirePriority::no);

  return mypriority < wpriority;
}
