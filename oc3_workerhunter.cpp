// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_workerhunter.hpp"
#include "oc3_house.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_positioni.hpp"
#include "oc3_servicewalker_helper.h"

WorkersHunter::WorkersHunter( WorkingBuildingPtr building, const int workersNeeded  )
 : ServiceWalker( building.as<Building>(), S_WORKERS_HUNTER)
{    
    _workersNeeded = workersNeeded;
    _walkerGraphic = WG_POOR;
    _walkerType = WT_WORKERS_HUNTER;
}

void WorkersHunter::hireWorkers( const int workers )
{
  WorkingBuildingPtr wbase = getBase().as<WorkingBuilding>();
  if( wbase.isValid() ) 
  {
    _workersNeeded = math::clamp( _workersNeeded - workers, 0, 0xff );
    wbase->addWorkers( workers );
  }
}

int WorkersHunter::getWorkersNeeded() const
{
    return _workersNeeded;
}

void WorkersHunter::onNewTile()
{
  Walker::onNewTile();

  if( _workersNeeded )
  {
    ServiceWalkerHelper hlp( *this );
    std::set<HousePtr> houses = hlp.getReachedBuildings<House>( getIJ() );
    for( std::set<HousePtr>::iterator it = houses.begin(); 
         it != houses.end(); ++it)
    {
        (*it)->applyService( ServiceWalkerPtr( this ) );
    }
  }
  else
  {
    if( !_pathWay.isReverse() ) 
      _pathWay.toggleDirection();
  }
}

ServiceWalkerPtr WorkersHunter::create( WorkingBuildingPtr building, const int workersNeeded )
{
  ServiceWalkerPtr ret( new WorkersHunter( building, workersNeeded ) );
  ret->drop();
  return ret;
}