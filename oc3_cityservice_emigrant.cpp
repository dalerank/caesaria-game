#include "oc3_cityservice_emigrant.hpp"
#include "oc3_city.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_emigrant.hpp"
#include "oc3_positioni.hpp"
#include "oc3_road.hpp"

CityServicePtr CityServiceEmigrant::create( City& city )
{
  CityServiceEmigrant* ret = new CityServiceEmigrant( city );
  
  return CityServicePtr( ret );
}

CityServiceEmigrant::CityServiceEmigrant( City& city )
: CityService( city, "emigration" )
{
  
}

void CityServiceEmigrant::update( const unsigned int time )
{
  if( time % 44 != 1 )
    return;
  
  unsigned int vacantPop=0;

  LandOverlays houses = _city.getBuildingList(B_HOUSE);
  for( LandOverlays::iterator itHouse = houses.begin(); itHouse != houses.end(); ++itHouse )
  {
    HousePtr house = (*itHouse).as<House>();
    if( house.isValid() && house->getAccessRoads().size() > 0 )
    {
      vacantPop += house->getMaxHabitants() - house->getNbHabitants();
    }
  }

  if( vacantPop == 0 )
  {
    return;
  }

  Walkers walkers = _city.getWalkerList( WT_EMIGRANT );

  if( vacantPop <= walkers.size() * 5 )
  {
    return;
  }

  Tile& roadTile = _city.getTilemap().at( _city.getRoadEntry() );
  RoadPtr roadEntry = roadTile.get_terrain().getOverlay().as<Road>();

  if( roadEntry.isValid() )
  {
    Emigrant::create( _city, roadEntry );
  }    
}