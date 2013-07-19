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

#include "oc3_cityservice_workershire.hpp"
#include "oc3_city.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_building_engineer_post.hpp"
#include "oc3_prefecture.hpp"
#include "oc3_walker_workerhunter.hpp"

#include <map>

using namespace std;

typedef map< int, BuildingType > Priorities;

class CityServiceWorkersHire::Impl
{
public:
  Priorities priorities;
  Walkers hrInCity;
  CityPtr city;
};

CityServicePtr CityServiceWorkersHire::create( CityPtr city )
{
  CityServicePtr ret( new CityServiceWorkersHire( city ));
  ret->drop();

  return ret;
}

CityServiceWorkersHire::CityServiceWorkersHire( CityPtr city )
: CityService( "workershire" ), _d( new Impl )
{
  _d->city = city;
  _d->priorities[ 1 ] = B_PREFECTURE;
  _d->priorities[ 2 ] = B_ENGINEER_POST;
  _d->priorities[ 3 ] = B_CLAY_PIT;
  _d->priorities[ 4 ] = B_WHEAT_FARM;
  _d->priorities[ 5 ] = B_GRAPE_FARM;
  _d->priorities[ 6 ] = B_GRANARY;
  _d->priorities[ 7 ] = B_IRON_MINE;
  _d->priorities[ 8 ] = B_TEMPLE_CERES;
  _d->priorities[ 9 ] = B_POTTERY;
  _d->priorities[ 10 ] = B_WAREHOUSE;  
  _d->priorities[ 11 ] = B_FORUM;
  _d->priorities[ 12 ] = B_DOCTOR;
  _d->priorities[ 13 ] = B_HOSPITAL;
  _d->priorities[ 14 ] = B_BARBER;
  _d->priorities[ 15 ] = B_BATHS;
  _d->priorities[ 16 ] = B_FRUIT_FARM;
  _d->priorities[ 17 ] = B_OLIVE_FARM;
  _d->priorities[ 18 ] = B_VEGETABLE_FARM;
  _d->priorities[ 19 ] = B_PIG_FARM;
  _d->priorities[ 20 ] = B_SENATE;
  _d->priorities[ 21 ] = B_MARKET;
  _d->priorities[ 22 ] = B_TIMBER_YARD;
  _d->priorities[ 23 ] = B_MARBLE_QUARRY;
  _d->priorities[ 24 ] = B_FURNITURE;
  _d->priorities[ 25 ] = B_WEAPONS_WORKSHOP;
}

bool CityServiceWorkersHire::_haveHr( WorkingBuildingPtr building )
{
  for( Walkers::iterator it=_d->hrInCity.begin(); it != _d->hrInCity.end(); it++ )
  {
    SmartPtr<WorkersHunter> hr = (*it).as<WorkersHunter>();
    if( hr.isValid() )
    {
      if( hr->getBase() == building.as<Building>() )
        return true;
    }
  }

  return false;
}

void CityServiceWorkersHire::_hireByType( const BuildingType type )
{
  CityHelper hlp( _d->city );
  WorkingBuildings buildings = hlp.getBuildings< WorkingBuilding >( type );
  for( WorkingBuildings::iterator it = buildings.begin(); it != buildings.end(); ++it )
  {
    WorkingBuildingPtr wb = *it;
    if( _haveHr( wb ) )
      continue;

    if( wb.isValid() && wb->getAccessRoads().size() > 0 && wb->getWorkers() < wb->getMaxWorkers() )
    {
      WorkersHunterPtr hr = WorkersHunter::create( _d->city );
      hr->setMaxDistance( 20 );
      hr->send2City( wb, wb->getMaxWorkers() - wb->getWorkers());
    }
  }
}

void CityServiceWorkersHire::update( const unsigned int time )
{
  if( time % 22 != 1 )
    return;

  //unsigned int vacantPop=0;

  _d->hrInCity = _d->city->getWalkerList( WT_WORKERS_HUNTER );

  for( Priorities::iterator it=_d->priorities.begin(); it != _d->priorities.end(); it++ )
    _hireByType( (*it).second );
}
