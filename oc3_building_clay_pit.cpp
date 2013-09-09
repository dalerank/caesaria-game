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

#include "oc3_building_clay_pit.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tile.hpp"
#include "oc3_city.hpp"
#include "oc3_foreach.hpp"
#include "oc3_tilemap.hpp"

ClayPit::ClayPit() : Factory( Good::none, Good::clay, B_CLAY_PIT, Size(2) )
{
  _setProductRate( 9.6f );
  setPicture( Picture::load( ResourceGroup::commerce, 61 ) );

  _getAnimation().load( ResourceGroup::commerce, 62, 10);
  _getAnimation().setFrameDelay( 3 );
  _fgPictures.resize(2);

  setMaxWorkers( 10 );
  setWorkers( 0 );
}

void ClayPit::timeStep( const unsigned long time )
{
  Factory::timeStep( time );
}

bool ClayPit::canBuild(const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( pos );
  bool near_water = false;

  Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
  PtrTilesList perimetr = tilemap.getRectangle( pos + TilePos( -1, -1), getSize() + Size( 2 ), Tilemap::checkCorners );
  foreach( Tile* tile, perimetr )
  {
    near_water |= tile->getTerrain().isWater();
  }

  return (is_constructible && near_water);
} 
