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

#include "cityservice_disorder.hpp"
#include "city.hpp"
#include "building/constants.hpp"
#include "core/foreach.hpp"
#include "building/house.hpp"
#include "walker/rioter.hpp"

using namespace constants;

namespace constants
{
const int defaultCrimeLevel = 75;
}

class CityServiceDisorder::Impl
{
public:
  CityPtr city;
  int minCrimeLevel;
};

CityServicePtr CityServiceDisorder::create( CityPtr city )
{
  CityServiceDisorder* ret = new CityServiceDisorder( city );

  return CityServicePtr( ret );
}

CityServiceDisorder::CityServiceDisorder( CityPtr city )
: CityService( "disorder" ), _d( new Impl )
{
  _d->city = city;
  _d->minCrimeLevel = defaultCrimeLevel;
}

void CityServiceDisorder::update( const unsigned int time )
{
  if( time % 22 != 1 )
    return;

  CityHelper helper( _d->city );
  HouseList houses = helper.find<House>( building::house );

  foreach( HousePtr house, houses )
  {
    int houseCrime = house->getServiceValue( Service::crime );

    int crime = rand() % houseCrime;
    if( crime >= _d->minCrimeLevel )
    {
      RioterPtr rioter = Rioter::create( _d->city );
      rioter->send2City( house );

      house->appendServiceValue( Service::crime, -defaultCrimeLevel / 2 );
    }
  }
}
