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

class TileHelper
{
public:
  static void initTileWidth( int width );
  static std::string convId2PicName( const unsigned int imgId );
  static int convPicName2Id( const std::string &pic_name);
  static Picture& pictureFromId( const unsigned int imgId );
  static int encode( const Tile& tt );
  static int turnCoastTile(int imgid , constants::Direction newDirection);
  static unsigned int hash( const TilePos& pos );
  static Point tilepos2screen( const TilePos& pos );
  static void decode( Tile& tile, const int bitset);
  static Tile& getInvalid();
  static void clear( Tile& tile );
  static const Point& cellCenter();
  static const Size& tilePicSize();
  static const Size& cellSize();
  static constants::Direction getDirection( const TilePos& b, const TilePos& e );
  static void fixPlateauFlags( Tile& tile );
};

}//end namespace gfx

#endif //__CAESARIA_HELPER_H_INCLUDED__
