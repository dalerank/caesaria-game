#include "oc3_cityservice_workershire.hpp"
#include "oc3_city.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_buildingengineer.hpp"
#include "oc3_buildingprefect.hpp"
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
}

bool CityServiceWorkersHire::_haveHr( WorkingBuilding* building )
{
  for( Walkers::iterator it=_d->hrInCity.begin(); it != _d->hrInCity.end(); it++ )
  {
    SmartPtr<WorkersHunter> hr = (*it).as<WorkersHunter>();
    if( hr.isValid() )
    {
      if( &hr->getBase() == building )
        return true;
    }
  }

  return false;
}

void CityServiceWorkersHire::_hireByType( const BuildingType type )
{
  CityHelper hlp( _city );
  std::list< WorkingBuilding* > buildings = hlp.getBuildings< WorkingBuilding* >( type );
  for( std::list< WorkingBuilding* >::iterator it = buildings.begin(); it != buildings.end(); ++it )
  {
    WorkingBuilding* wb = *it;
     if( _haveHr( wb ) )
      continue;

    if( wb && wb->getAccessRoads().size() > 0 
        && wb->getWorkers() < wb->getMaxWorkers() )
    {
      WorkersHunter* hr = new WorkersHunter( *wb, wb->getMaxWorkers() - wb->getWorkers() );
      hr->setMaxDistance( 20 );
      hr->start();
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