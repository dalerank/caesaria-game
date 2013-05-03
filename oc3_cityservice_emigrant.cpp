#include "oc3_cityservice_emigrant.hpp"
#include "oc3_city.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_emigrant.hpp"
#include "oc3_positioni.hpp"

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

  City::LandOverlays houses = _city.getBuildingList(B_HOUSE);
  for( City::LandOverlays::iterator itHouse = houses.begin(); itHouse != houses.end(); ++itHouse )
  {
    House* house = safety_cast<House*>(*itHouse);
    if( house && house->getAccessRoads().size() > 0 )
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
  Road* roadEntry = safety_cast< Road* >( roadTile.get_terrain().getOverlay() );

  if( roadEntry )
  {
    Emigrant::create( _city, *roadEntry );
  }    
}