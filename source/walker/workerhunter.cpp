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
#include "servicewalker_helper.hpp"
#include "city/helper.hpp"
#include "core/variant_map.hpp"
#include "game/enums.hpp"
#include "game/resourcegroup.hpp"
#include "pathway/path_finding.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "core/foreach.hpp"
#include "helper.hpp"
#include "walkers_factory.hpp"

using namespace constants;

REGISTER_CLASS_IN_WALKERFACTORY(walker::recruter, Recruter)

namespace {
CAESARIA_LITERALCONST(priority)
static const int noPriority = 999;
}

class Recruter::Impl
{
public:
  typedef std::map< objects::Group, int > PriorityMap;

  unsigned int needWorkers;
  city::HirePriorities priority;
  PriorityMap priorityMap;
  unsigned int reachDistance;

public:
  bool isMyPriorityOver(BuildingPtr base, WorkingBuildingPtr wbuilding );
};

Recruter::Recruter(PlayerCityPtr city )
 : ServiceWalker( city, Service::recruter ), _d( new Impl )
{    
  _d->needWorkers = 0;
  _d->reachDistance = 2;
  _setType( walker::recruter );
}

void Recruter::hireWorkers( const int workers )
{
  WorkingBuildingPtr wbase = ptr_cast<WorkingBuilding>( base() );
  if( wbase.isValid() ) 
  {
    unsigned int reallyHire = wbase->addWorkers( workers );
    _d->needWorkers -= reallyHire;
  }
}

void Recruter::setPriority(const city::HirePriorities& priority)
{
  _d->priority = priority;

  int priorityLevel = 1;
  foreach( i, _d->priority )
  {
    city::industry::BuildingGroups groups = city::industry::toGroups( *i );
    foreach( grIt, groups )
    {
      _d->priorityMap[ *grIt ] = priorityLevel;
    }

    priorityLevel++;
  }
}

int Recruter::needWorkers() const { return _d->needWorkers; }

void Recruter::_centerTile()
{
  Walker::_centerTile();

  if( _d->needWorkers )
  {
    ServiceWalkerHelper hlp( *this );
    std::set<HousePtr> houses = hlp.getReachedBuildings<House>( pos() );

    foreach( it, houses ) { (*it)->applyService( this ); }

    if( !_d->priority.empty() )
    {
      std::set<WorkingBuildingPtr> blds = hlp.getReachedBuildings<WorkingBuilding>( pos() );

      foreach( it, blds )
      {
        bool priorityOver = _d->isMyPriorityOver( base(), *it );
        if( priorityOver )
        {
          WorkingBuildingPtr wbld = *it;
          int removedFromWb = wbld->removeWorkers( _d->needWorkers );
          hireWorkers( removedFromWb );
        }
      }
    }
  }
  else
  {    
    if( !_pathwayRef().isReverse() ) //return2Base();
    {
      _pathwayRef().toggleDirection();
    }
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
  ServiceWalker::send2City( building.object(), ServiceWalker::goLowerService | ServiceWalker::anywayWhenFailed );
}

void Recruter::send2City(BuildingPtr base, int orders)
{
  WorkingBuildingPtr wb = ptr_cast<WorkingBuilding>( base );
  if( wb.isValid() )
  {
    send2City( wb, wb->needWorkers() );
  }
  else
  {
    Logger::warning( "WARNING !!!: Recruter try hire workers for non working buildng. Delete rectuter.");
    deleteLater();
  }
}

void Recruter::once(WorkingBuildingPtr building, const unsigned int workersNeed, unsigned int distance )
{
  _d->needWorkers = workersNeed;
  _d->reachDistance = distance;
  setBase( ptr_cast<Building>( building ) );
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
  case plOrigin: return base().isValid() ? base()->pos() : TilePos( -1, -1 );
  default: break;
  }

  return ServiceWalker::places( type );
}

unsigned int Recruter::reachDistance() const { return _d->reachDistance;}

void Recruter::save(VariantMap& stream) const
{
  ServiceWalker::save( stream );
  stream[ lc_priority ] = _d->priority.toVariantList();
  VARIANT_SAVE_ANY_D( stream, _d, needWorkers );
}

void Recruter::load(const VariantMap& stream)
{
  ServiceWalker::load( stream );
  VARIANT_LOAD_ANY_D( _d, needWorkers, stream );
  _d->priority << stream.get( lc_priority ).toList();
}

bool Recruter::die()
{
  bool created = ServiceWalker::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::citizen1, 97, 104 );
    return true;
  }

  return created;
}

bool Recruter::Impl::isMyPriorityOver(BuildingPtr base, WorkingBuildingPtr wbuilding)
{
  PriorityMap::iterator myPrIt = priorityMap.find( (objects::Group)base->group() );
  PriorityMap::iterator bldPrIt = priorityMap.find( (objects::Group)wbuilding->group() );
  int mypriority = (myPrIt != priorityMap.end() ? myPrIt->second : noPriority);
  int wpriority = (bldPrIt != priorityMap.end() ? bldPrIt->second : noPriority);

  return mypriority < wpriority;
}
