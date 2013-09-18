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

#include "oc3_cityservice_festival.hpp"
#include "oc3_gamedate.hpp"
#include "oc3_city.hpp"
#include "oc3_divinity.hpp"

class CityServiceFestival::Impl
{
public:
  CityPtr city;

  DateTime festivalDate;
  RomeDivinityType godName;
  int festivalType;
};

CityServicePtr CityServiceFestival::create( CityPtr city )
{
  CityServicePtr ret( new CityServiceFestival( city ) );
  ret->drop();

  return ret;
}

CityServiceFestival::CityServiceFestival( CityPtr city )
: CityService( "festival" ), _d( new Impl )
{
  _d->city = city;
}

void CityServiceFestival::update( const unsigned int time )
{
  if( time % 44 != 1 )
    return;

  if( _d->festivalDate == GameDate::current() )
  {
    DivinePantheon::doFestival4( _d->godName, _d->festivalType );
  }
}
