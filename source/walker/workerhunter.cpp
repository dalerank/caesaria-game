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
 : ServiceWalker( city, Service::recruter ), __INIT_IMPL(Recruter)
{    
  _setType( walker::recruter );

  __D_REF(d,Recruter)
  d.needWorkers = 0;
  d.reachDistance = (int)ReachDistance::max;
  d.once_shot = false;
  d.failedCounter = 0;
  d.patrolFinished = false;
}

void Recruter::hireWorkers( const int workers )
{
  auto parentBuilding = base().as<WorkingBuilding>();
  if( parentBuilding.isValid() )
  {
    unsigned int reallyHire = parentBuilding->addWorkers( workers );
    _dfunc()->needWorkers -= reallyHire;
  }
  else
  {
    Logger::warning( "!!! Recruter base[{0},{1}] is null. Stop working.", baseLocation().i(), baseLocation().j() );
    return2Base();
  }
}

void Recruter::setPriority(const city::HirePriorities& priority)
{
  __D_REF(d,Recruter)
  d.priority = priority;

  int priorityLevel = 1;
  for( const auto& priority : d.priority )
  {
    object::Groups groups = city::industry::toGroups( priority );
    for( auto group : groups )
    {
      d.priorityMap[ group ] = priorityLevel;
    }

    priorityLevel++;
  }
}

int Recruter::needWorkers() const { return _dfunc()->needWorkers; }

void Recruter::_centerTile()
{  
  Walker::_centerTile();
  BuildingPtr refBase = base();

  if( refBase.isNull() )
  {
    Logger::warning( "!!! Recruter haveno base" );
    return;
  }

  __D_REF(d,Recruter)
  ReachedBuildings reached = getReachedBuildings( pos() );
  if( d.needWorkers > 0 )
  {
    UqBuildings<House> houses = reached.select<House>();

    for( auto house : houses )
      house->applyService( this );

    if( !d.priority.empty() )
    {
      auto workingBuildings = reached.select<WorkingBuilding>();

      for( auto bld : workingBuildings )
      {
        if( bld.equals( refBase ) ) //avoid recruting from out base
          continue;

        bool priorityOver = d.isMyPriorityOver( refBase, bld );
        if( priorityOver )
        {
          int removedFromWb = bld->removeWorkers( d.needWorkers );
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
  __D_REF(d,Recruter)
  d.failedCounter++;

  Pathway newway;
  if( d.failedCounter > 5 )
  {
    Logger::warning( "!!! Failed find way for recruter " + name() );
    die();
  }

  newway = PathwayHelper::create( pos(), base(), PathwayHelper::roadFirst );

  if( newway.isValid() )
  {
    setPathway( newway );
    d.failedCounter = 0;
    d.patrolFinished = true;
    go();
  }
}

void Recruter::send2City( WorkingBuildingPtr building, const int workersNeeded )
{
  _dfunc()->needWorkers = workersNeeded;
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
    Logger::warning( "!!! Recruter try hire workers for non working buildng. Delete rectuter.");
    deleteLater();
  }
}

void Recruter::once(WorkingBuildingPtr building, const unsigned int workersNeed, unsigned int distance )
{
  __D_REF(d,Recruter)
  d.needWorkers = workersNeed;
  d.reachDistance = distance;
  d.once_shot = true;

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

unsigned int Recruter::reachDistance() const { return _dfunc()->reachDistance;}

void Recruter::save(VariantMap& stream) const
{
  ServiceWalker::save( stream );
  __D_IMPL_CONST(d,Recruter)
  VARIANT_SAVE_CLASS_D( stream, d, priority )
  VARIANT_SAVE_ANY_D( stream, d, needWorkers )
  VARIANT_SAVE_ANY_D( stream, d, failedCounter )
  VARIANT_SAVE_ANY_D( stream, d, patrolFinished )
}

void Recruter::load(const VariantMap& stream)
{
  ServiceWalker::load( stream );
  __D_IMPL(d,Recruter)
  VARIANT_LOAD_ANY_D( d, needWorkers, stream )
  VARIANT_LOAD_CLASS_D_LIST( d, priority, stream )
  VARIANT_LOAD_ANY_D( d, failedCounter, stream )
  VARIANT_LOAD_ANY_D( d, patrolFinished, stream )
}

bool Recruter::die()
{
  if( _dfunc()->once_shot )
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
  if( _dfunc()->patrolFinished )
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
