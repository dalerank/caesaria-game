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
};

CityServicePtr CityServiceWorkersHire::create( City& city )
{
  CityServiceWorkersHire* ret = new CityServiceWorkersHire( city );

  return CityServicePtr( ret );
}

CityServiceWorkersHire::CityServiceWorkersHire( City& city )
: CityService( city, "workershire" ), _d( new Impl )
{
  _d->priorities[ 1 ] = B_PREFECT;
  _d->priorities[ 2 ] = B_ENGINEER;
  _d->priorities[ 3 ] = B_CLAY_PIT;
  _d->priorities[ 4 ] = B_WHEAT;
  _d->priorities[ 5 ] = B_GRAPE;
  _d->priorities[ 6 ] = B_GRANARY;
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
  CityHelper hlp( _city );
  WorkingBuildings buildings = hlp.getBuildings< WorkingBuilding >( type );
  for( WorkingBuildings::iterator it = buildings.begin(); it != buildings.end(); ++it )
  {
    WorkingBuildingPtr wb = *it;
    if( _haveHr( wb ) )
      continue;

    if( wb.isValid() && wb->getAccessRoads().size() > 0 
        && wb->getWorkers() < wb->getMaxWorkers() )
    {
      WorkersHunterPtr hr = WorkersHunter::create( _city );
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

  _d->hrInCity = _city.getWalkerList( WT_WORKERS_HUNTER );

  for( Priorities::iterator it=_d->priorities.begin(); it != _d->priorities.end(); it++ )
    _hireByType( (*it).second );
}