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

#include "oc3_cityservice_water.hpp"

class CityServiceWater::Impl
{
public:
};

CityServicePtr CityServiceWater::create( City& city )
{
  CityServiceWater* ret = new CityServiceWater( city );

  return CityServicePtr( ret );
}

CityServiceWater::CityServiceWater( City& city )
: CityService( city, "water" ), _d( new Impl )
{
}

void CityServiceWater::update( const unsigned int time )
{
  if( time % 22 != 1 )
    return;

  unsigned int vacantPop=0;
}