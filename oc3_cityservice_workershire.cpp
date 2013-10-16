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
#include "oc3_building_prefecture.hpp"
#include "oc3_walker_workerhunter.hpp"
#include "oc3_foreach.hpp"

#include <map>

using namespace std;

typedef map< int, LandOverlayType > Priorities;

class CityServiceWorkersHire::Impl
{
public:
  Priorities priorities;
  WalkerList hrInCity;
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
  _d->priorities[ 26 ] = B_THEATER;
  _d->priorities[ 27 ] = B_ACTOR_COLONY;
  _d->priorities[ 28 ] = B_SCHOOL;
  _d->priorities[ 29 ] = buildingAmphitheater;
  _d->priorities[ 30 ] = B_GLADIATOR_SCHOOL;
}

bool CityServiceWorkersHire::_haveHr( WorkingBuildingPtr building )
{
  foreach( WalkerPtr walker, _d->hrInCity )
  {
    SmartPtr<WorkersHunter> hr = walker.as<WorkersHunter>();
    if( hr.isValid() )
    {
      if( hr->getBase() == building.as<Building>() )
        return true;
    }
  }

  return false;
}

void CityServiceWorkersHire::_hireByType( const LandOverlayType type )
{
  CityHelper hlp( _d->city );
  WorkingBuildingList buildings = hlp.getBuildings< WorkingBuilding >( type );
  foreach( WorkingBuildingPtr wrkbld, buildings )
  {
    if( _haveHr( wrkbld ) )
      continue;

    if( wrkbld->getAccessRoads().size() > 0 && wrkbld->getWorkers() < wrkbld->getMaxWorkers() )
    {
      WorkersHunterPtr hr = WorkersHunter::create( _d->city );
      hr->setMaxDistance( 20 );
      hr->send2City( wrkbld, wrkbld->getMaxWorkers() - wrkbld->getWorkers());
    }
  }
}

void CityServiceWorkersHire::update( const unsigned int time )
{
  if( time % 22 != 1 )
    return;

  //unsigned int vacantPop=0;

  _d->hrInCity = _d->city->getWalkerList( WT_WORKERS_HUNTER );

  foreach( Priorities::value_type& pr, _d->priorities )
  {
    _hireByType( pr.second );
  }
}
