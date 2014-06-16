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

#include "workerhunter.hpp"
#include "objects/house.hpp"
#include "core/predefinitions.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "servicewalker_helper.hpp"
#include "city/city.hpp"
#include "game/enums.hpp"
#include "game/resourcegroup.hpp"
#include "pathway/path_finding.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "core/foreach.hpp"
#include "helper.hpp"

using namespace constants;

namespace {
CAESARIA_LITERALCONST(needworkers)
CAESARIA_LITERALCONST(priority)
static const int noPriority = 999;
}

class Recruter::Impl
{
public:
  typedef std::map< building::Group, int > PriorityMap;

  unsigned int needWorkers;
  city::HirePriorities priority;
  PriorityMap priorityMap;

public:
  bool isMyPriorityOver(BuildingPtr base, WorkingBuildingPtr wbuilding );
};

Recruter::Recruter(PlayerCityPtr city )
 : ServiceWalker( city, Service::recruter ), _d( new Impl )
{    
  _d->needWorkers = 0;
  _setType( walker::recruter );
}

void Recruter::hireWorkers( const int workers )
{
  WorkingBuildingPtr wbase = ptr_cast<WorkingBuilding>( base() );
  if( wbase.isValid() ) 
  {
    _d->needWorkers = math::clamp( _d->needWorkers - workers, 0u, 0xffu );
    wbase->addWorkers( workers );
  }
}

void Recruter::setPriority(const city::HirePriorities& priority)
{
  _d->priority = priority;

  int priorityLevel = 1;
  foreach( i, _d->priority )
  {
    city::Industry::BuildingGroups groups = city::Industry::toGroups( *i );
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
          int numWorkers = std::min( wbld->numberWorkers(), _d->needWorkers );
          wbld->removeWorkers( numWorkers );
          hireWorkers( numWorkers );
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

void Recruter::save(VariantMap& stream) const
{
  ServiceWalker::save( stream );
  stream[ lc_priority ] = _d->priority.toVariantList();
  stream[ lc_needworkers ] = _d->needWorkers;
}

void Recruter::load(const VariantMap& stream)
{
  ServiceWalker::load( stream );
  _d->needWorkers = stream.get( lc_needworkers );
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
  PriorityMap::iterator myPrIt = priorityMap.find( (building::Group)base->group() );
  PriorityMap::iterator bldPrIt = priorityMap.find( (building::Group)wbuilding->group() );
  int mypriority = (myPrIt != priorityMap.end() ? myPrIt->second : noPriority);
  int wpriority = (bldPrIt != priorityMap.end() ? bldPrIt->second : noPriority);

  return mypriority < wpriority;
}
