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

#include "workerhunter.hpp"
#include "building/house.hpp"
#include "core/predefinitions.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "servicewalker_helper.hpp"
#include "game/city.hpp"
#include "game/enums.hpp"
#include "game/resourcegroup.hpp"
#include "game/path_finding.hpp"
#include "constants.hpp"
#include "corpse.hpp"

using namespace constants;

WorkersHunter::WorkersHunter( CityPtr city )
 : ServiceWalker( city, Service::workersRecruter )
{    
    _workersNeeded = 0;
    _setGraphic( WG_POOR );
    _setType( walker::recruter );
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
    foreach( HousePtr house, houses )
    {
      house->applyService( ServiceWalkerPtr( this ) );
    }
  }
  else
  {
    if( !_getPathway().isReverse() ) 
    {
      _getPathway().toggleDirection();
    }
  }
}

WorkersHunterPtr WorkersHunter::create( CityPtr city )
{ 
  WorkersHunterPtr ret( new WorkersHunter( city ) );
  ret->drop();
  return ret;
}

void WorkersHunter::send2City( WorkingBuildingPtr building, const int workersNeeded )
{
  _workersNeeded = workersNeeded;
  ServiceWalker::send2City( building.as< Building >() );
}

void WorkersHunter::die()
{
  ServiceWalker::die();

  Corpse::create( _getCity(), getIJ(), ResourceGroup::citizen1, 97, 104 );
}
