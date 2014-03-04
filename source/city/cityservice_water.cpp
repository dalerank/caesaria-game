// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#include "cityservice_water.hpp"
#include "city.hpp"

class CityServiceWater::Impl
{
public:
  PlayerCityPtr city;
};

city::SrvcPtr CityServiceWater::create(PlayerCityPtr city )
{
  CityServiceWater* ret = new CityServiceWater( city );

  return city::SrvcPtr( ret );
}

CityServiceWater::CityServiceWater(PlayerCityPtr city )
  : city::Srvc( "water" ), _d( new Impl )
{
  _d->city = city;
}

void CityServiceWater::update( const unsigned int time )
{
  if( time % 22 != 1 )
    return;

  //unsigned int vacantPop=0;
}
