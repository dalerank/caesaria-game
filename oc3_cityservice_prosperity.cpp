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

#include "oc3_cityservice_prosperity.hpp"
#include "oc3_city.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_positioni.hpp"
#include "oc3_building_house.hpp"
#include "oc3_house_level.hpp"
#include "oc3_tile.hpp"
#include "oc3_building_entertainment.hpp"
#include "oc3_gamedate.hpp"

class CityServiceProsperity::Impl
{
public:
  CityPtr city;
  DateTime lastDate;
  int prosperity;
  int prosperityExtend;
};

CityServicePtr CityServiceProsperity::create( CityPtr city )
{
  CityServicePtr ret( new CityServiceProsperity( city ) );
  ret->drop();

  return ret;
}

CityServiceProsperity::CityServiceProsperity( CityPtr city )
  : CityService( "prosperity" ), _d( new Impl )
{
  _d->city = city;
  _d->lastDate = GameDate::current();
  _d->prosperity = 0;
  _d->prosperityExtend = 0;
}

void CityServiceProsperity::update( const unsigned int time )
{
  if( time % 44 != 1 )
    return;

  if( abs( GameDate::current().getYear() - _d->lastDate.getYear() ) == 1 )
  {
    _d->lastDate = GameDate::current();

    if( _d->city->getPopulation() == 0 )
    {
      _d->prosperity = 0;
      _d->prosperityExtend = 0;
      return;
    }

    CityHelper helper( _d->city );
    HouseList houses = helper.getBuildings<House>( B_HOUSE );

    int prosperityCap = 0;
    int patricianCount = 0;
    int plebsCount = 0;
    foreach( HousePtr house, houses)
    {
      prosperityCap += house->getLevelSpec().getProsperity();
      patricianCount += house->getLevelSpec().isPatrician() ? house->getNbHabitants() : 0;
      plebsCount += house->getLevelSpec().getHouseLevel() < 5 ? house->getNbHabitants() : 0;
    }

    prosperityCap /= houses.size();

    _d->prosperity = math::clamp( prosperityCap, 0, _d->prosperity + 2 );

    bool cityMakeProfit = false;
    _d->prosperityExtend = (cityMakeProfit ? 2 : -1);

    bool more10PercentIsPatrician = (patricianCount / (float)_d->city->getPopulation()) > 0.1;
    _d->prosperityExtend += (more10PercentIsPatrician ? 1 : 0);

    bool less30percentIsPlebs = (plebsCount / (float)_d->city->getPopulation()) < 0.3;
    _d->prosperityExtend += (less30percentIsPlebs ? 1 : 0);

    bool haveHippodrome = helper.getBuildings<Hippodrome>( B_HIPPODROME ).size() > 0;
    _d->prosperityExtend += (haveHippodrome > 0 ? 1 : 0);

    bool unemploymentLess5percent = false;
    bool unemploymentMore15percent = false;
    _d->prosperityExtend += (unemploymentLess5percent ? 1 : 0);
    _d->prosperityExtend += (unemploymentMore15percent ? -1 : 0);

    bool havePatrician = patricianCount > 0;
    _d->prosperityExtend += (havePatrician ? 1 : 0);

    bool payMoreThanRome = false;
    bool payLessThanRome = false;
    _d->prosperityExtend += (payMoreThanRome ? 1 : 0);
    _d->prosperityExtend += (payLessThanRome ? -1 : 0);
   
    bool brokeAndCaesarBailCity = false;
    _d->prosperityExtend += (brokeAndCaesarBailCity ? -3 : 0);

    bool failurePayTribute = false;
    _d->prosperityExtend += (failurePayTribute ? -3 : 0);
  }
}

int CityServiceProsperity::getValue() const
{
  return _d->prosperity + _d->prosperityExtend;
}
