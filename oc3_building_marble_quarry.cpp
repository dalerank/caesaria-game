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

#include "oc3_building_marble_quarry.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_tile.hpp"
#include "oc3_city.hpp"

MarbleQuarry::MarbleQuarry() : Factory(Good::none, Good::marble, B_MARBLE_QUARRY, Size(2) )
{
  _setProductRate( 9.6f );
  setPicture( Picture::load( ResourceGroup::commerce, 43 ) );

  _getAnimation().load( ResourceGroup::commerce, 44, 10);
  _getAnimation().setFrameDelay( 4 );
  _fgPictures.resize(2);
}

void MarbleQuarry::timeStep( const unsigned long time )
{
  bool mayAnimate = getWorkers() > 0;

  if( mayAnimate && _getAnimation().isStopped() )
  {
    _getAnimation().start();
  }

  if( !mayAnimate && _getAnimation().isRunning() )
  {
    _getAnimation().stop();
  }

  Factory::timeStep( time );
}

bool MarbleQuarry::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( city, pos );
  bool near_mountain = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = city->getTilemap();
  TilemapTiles perimetr = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size( 2 ), Tilemap::checkCorners);
  foreach( Tile* tile, perimetr )
  {
    near_mountain |= tile->getTerrain().isRock();
  }

  return (is_constructible && near_mountain);
}
