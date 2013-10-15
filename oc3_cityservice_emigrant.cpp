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

#include "oc3_cityservice_emigrant.hpp"
#include "oc3_city.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_walker_emigrant.hpp"
#include "oc3_positioni.hpp"
#include "oc3_road.hpp"
#include "oc3_building_house.hpp"
#include "oc3_tile.hpp"
#include "oc3_cityfunds.hpp"
#include "oc3_empire.hpp"
#include "oc3_game_settings.hpp"

class CityServiceEmigrant::Impl
{
public:
  CityPtr city;
};

CityServicePtr CityServiceEmigrant::create( CityPtr city )
{
  CityServicePtr ret( new CityServiceEmigrant( city ) );
  ret->drop();

  return ret;
}

CityServiceEmigrant::CityServiceEmigrant( CityPtr city )
: CityService( "emigration" ), _d( new Impl )
{
  _d->city = city;
}

void CityServiceEmigrant::update( const unsigned int time )
{
  if( time % 44 != 1 )
    return;
  
  unsigned int vacantPop=0;
  int emigrantsDesirability = 50; //base desirability value
  float emDesKoeff = math::clamp<float>( (float)GameSettings::get( GameSettings::emigrantSalaryKoeff ), 1.f, 99.f );
  //if salary in city more then empire people more effectivelly go to ouu city
  emigrantsDesirability += (_d->city->getEmpire()->getWorkersSalary() - _d->city->getFunds().getWorkerSalary()) * emDesKoeff;

  int worklessPercent = CityStatistic::getWorklessNumber( _d->city ) * 100 / CityStatistic::getAvailableWorkersNumber( _d->city );
  emigrantsDesirability += worklessPercent;

  int goddesRandom = rand() % 100;
  if( goddesRandom > emigrantsDesirability )
    return;


  CityHelper helper( _d->city );
  HouseList houses = helper.getBuildings<House>(B_HOUSE);
  foreach( HousePtr house, houses )
  {
    if( house->getAccessRoads().size() > 0 )
    {
      vacantPop += math::clamp( house->getMaxHabitants() - house->getHabitants().count(), 0, 0xff );
    }
  }

  if( vacantPop == 0 )
  {
    return;
  }

  WalkerList walkers = _d->city->getWalkerList( WT_EMIGRANT );

  if( vacantPop <= walkers.size() * 5 )
  {
    return;
  }

  Tile& roadTile = _d->city->getTilemap().at( _d->city->getBorderInfo().roadEntry );

  EmigrantPtr emigrant = Emigrant::create( _d->city );

  if( emigrant.isValid() )
  {
    emigrant->send2City( roadTile );
  }
}
