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

#ifndef __CAESARIA_TILEMAP_CONFIG_H_INCLUDED__
#define __CAESARIA_TILEMAP_CONFIG_H_INCLUDED__

#include "tilepos.hpp"
#include "core/direction.hpp"
#include "core/size.hpp"

namespace gfx
{

class Tilemap;

namespace tilemap
{
  enum { c3bldSize=5, c3CellWidth=30, caCellWidth=60, c3mapSize=162, c3mapSizeSq=c3mapSize*c3mapSize };

  Direction getDirection( const TilePos& b, const TilePos& e );
  void initTileBase( int width );
  unsigned int picWidth2CellSize( int width );
  Tilemap& getInvalid();
  const Point& cellCenter();
  const Size& cellPicSize();
  const Size& cellSize();
  const TilePos& invalidLocation();
  const TilePos& unitLocation();
  bool isValidLocation( const TilePos& pos );
}

}//end namespace gfx

#endif //__CAESARIA_TILEMAP_CONFIG_H_INCLUDED__
