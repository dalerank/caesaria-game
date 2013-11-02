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

#include "cityservice_workershire.hpp"
#include "city.hpp"
#include "core/safetycast.hpp"
#include "building/engineer_post.hpp"
#include "building/prefecture.hpp"
#include "walker/workerhunter.hpp"
#include "core/foreach.hpp"
#include "building/constants.hpp"
#include <map>

using namespace constants;
using namespace std;

typedef map< int, TileOverlay::Type > Priorities;

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
  _d->priorities[ 1 ] = building::B_PREFECTURE;
  _d->priorities[ 2 ] = building::B_ENGINEER_POST;
  _d->priorities[ 3 ] = building::B_CLAY_PIT;
  _d->priorities[ 4 ] = building::B_WHEAT_FARM;
  _d->priorities[ 5 ] = building::B_GRAPE_FARM;
  _d->priorities[ 6 ] = building::B_GRANARY;
  _d->priorities[ 7 ] = building::B_IRON_MINE;
  _d->priorities[ 8 ] = building::B_TEMPLE_CERES;
  _d->priorities[ 9 ] = building::B_POTTERY;
  _d->priorities[ 10 ] = building::B_WAREHOUSE;
  _d->priorities[ 11 ] = building::B_FORUM;
  _d->priorities[ 12 ] = building::B_DOCTOR;
  _d->priorities[ 13 ] = building::B_HOSPITAL;
  _d->priorities[ 14 ] = building::B_BARBER;
  _d->priorities[ 15 ] = building::B_BATHS;
  _d->priorities[ 16 ] = building::B_FRUIT_FARM;
  _d->priorities[ 17 ] = building::B_OLIVE_FARM;
  _d->priorities[ 18 ] = building::B_VEGETABLE_FARM;
  _d->priorities[ 19 ] = building::B_PIG_FARM;
  _d->priorities[ 20 ] = building::B_SENATE;
  _d->priorities[ 21 ] = building::B_MARKET;
  _d->priorities[ 22 ] = building::B_TIMBER_YARD;
  _d->priorities[ 23 ] = building::B_MARBLE_QUARRY;
  _d->priorities[ 24 ] = building::B_FURNITURE;
  _d->priorities[ 25 ] = building::B_WEAPONS_WORKSHOP;
  _d->priorities[ 26 ] = building::theater;
  _d->priorities[ 27 ] = building::actorColony;
  _d->priorities[ 28 ] = building::B_SCHOOL;
  _d->priorities[ 29 ] = building::amphitheater;
  _d->priorities[ 30 ] = building::gladiatorSchool;
  _d->priorities[ 31 ] = building::B_WHARF;
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

void CityServiceWorkersHire::_hireByType(const TileOverlay::Type type )
{
  CityHelper hlp( _d->city );
  WorkingBuildingList buildings = hlp.find< WorkingBuilding >( type );
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
