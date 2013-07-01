#include "oc3_cityservice_workershire.hpp"
#include "oc3_city.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_buildingengineer.hpp"
#include "oc3_prefecture.hpp"
#include "oc3_workerhunter.hpp"

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
  _d->priorities[ 5 ] = B_GRAPE;
  _d->priorities[ 6 ] = B_GRANARY;
  _d->priorities[ 7 ] = B_IRON_MINE;
  _d->priorities[ 8 ] = B_TEMPLE_CERES;
  _d->priorities[ 9 ] = B_POTTERY;
  _d->priorities[ 10 ] = B_WAREHOUSE;  
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

  unsigned int vacantPop=0;

  _d->hrInCity = _d->city->getWalkerList( WT_WORKERS_HUNTER );

  for( Priorities::iterator it=_d->priorities.begin(); it != _d->priorities.end(); it++ )
    _hireByType( (*it).second );
}