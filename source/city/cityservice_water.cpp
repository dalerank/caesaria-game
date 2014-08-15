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

#include "cityservice_water.hpp"
#include "city.hpp"
#include "gfx/tilemap.hpp"
#include "game/gamedate.hpp"
#include "gfx/tilesarray.hpp"

using namespace gfx;

namespace city
{

const int waterDecreaseInterval = GameDate::days2ticks( 5 );

class Water::Impl
{
public:
  PlayerCityPtr city;
  TilesArray tiles;
};

city::SrvcPtr Water::create(PlayerCityPtr city )
{
  city::SrvcPtr ret( new Water( city ) );
  ret->drop();

  return ret;
}

Water::Water(PlayerCityPtr city )
  : city::Srvc( *city.object(), "water" ), _d( new Impl )
{
  _d->city = city;
  _d->tiles = city->tilemap().allTiles();
}

void Water::update( const unsigned int time )
{
  if( time % waterDecreaseInterval == 0 )
  {
    foreach( it, _d->tiles )
    {
      Tile* tile = *it;
      int value = tile->param( Tile::pFountainWater );
      if( value > 0 )
        tile->setParam( Tile::pFountainWater, math::max( 0, value-1) );

      value = tile->param( Tile::pWellWater );
      if( value > 0 )
       tile->setParam( Tile::pWellWater, math::max( 0, value-1) );
    }
  }
}

}//end namespace city
