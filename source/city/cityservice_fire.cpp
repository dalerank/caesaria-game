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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "cityservice_fire.hpp"
#include "city.hpp"
#include "gfx/tilemap.hpp"
#include "game/gamedate.hpp"

using namespace gfx;

namespace city
{

class Fire::Impl
{
public:
  Fire::Locations locations;
};

city::SrvcPtr Fire::create( PlayerCityPtr city )
{
  city::SrvcPtr ret( new Fire( city ) );
  ret->drop();

  return ret;
}

std::string Fire::defaultName() { return CAESARIA_STR_EXT(Fire); }

Fire::Fire( PlayerCityPtr city )
  : city::Srvc( city, defaultName() ), _d( new Impl )
{
}

void Fire::timeStep( const unsigned int time )
{  
}

void Fire::addLocation(const TilePos& location) { _d->locations.insert( location ); }
void Fire::rmLocation(const TilePos& location) { _d->locations.erase( location ); }
const Fire::Locations&Fire::locations() const { return _d->locations;  }

}//end namespace city
