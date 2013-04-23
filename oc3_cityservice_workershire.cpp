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
  City::Walkers hrInCity;
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
}

bool CityServiceWorkersHire::_haveHr( ServiceBuilding* building )
{
  for( City::Walkers::iterator it=_d->hrInCity.begin(); it != _d->hrInCity.end(); it++ )
  {
    if( WorkersHunter* hr = safety_cast< WorkersHunter* >(*it) )
    {
      if( &hr->getServiceBuilding() == building )
        return true;
    }
  }

  return false;
}

void CityServiceWorkersHire::_hireByType( const BuildingType type )
{
  City::LandOverlays buildings = _city.getBuildingList(type);
  for( City::LandOverlays::iterator it = buildings.begin(); it != buildings.end(); ++it )
  {
    ServiceBuilding* sb = safety_cast<ServiceBuilding*>(*it);

    if( _haveHr( sb ) )
      continue;

    if( sb && sb->getAccessRoads().size() > 0 
        && sb->getWorkers() < sb->getMaxWorkers() )
    {
      WorkersHunter* hr = new WorkersHunter( *sb, sb->getMaxWorkers() - sb->getWorkers() );
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