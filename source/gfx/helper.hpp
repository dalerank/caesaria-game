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

#ifndef __CAESARIA_HELPER_H_INCLUDED__
#define __CAESARIA_HELPER_H_INCLUDED__

#include "tile.hpp"

namespace gfx
{

namespace imgid
{
  std::string toResource( const unsigned int imgId );
  int fromResource( const std::string &pic_name);
  Picture& toPicture( const unsigned int imgId );
}

namespace tile
{
  int encode( const Tile& tt );
  int turnCoastTile(int imgid , constants::Direction newDirection);
  unsigned int hash( const TilePos& pos );
  Point tilepos2screen( const TilePos& pos );
  TilePos screen2tilepos( const Point& point, int mapsize );
  void decode( Tile& tile, const int bitset);
  Tile& getInvalid();
  void clear( Tile& tile );
  void fixPlateauFlags( Tile& tile );
}//end namespace tile

namespace tilemap
{
  constants::Direction getDirection( const TilePos& b, const TilePos& e );
  void initTileBase( int width );
  const Point& cellCenter();
  const Size& cellPicSize();
  const Size& cellSize();
}

}//end namespace gfx

#endif //__CAESARIA_HELPER_H_INCLUDED__
