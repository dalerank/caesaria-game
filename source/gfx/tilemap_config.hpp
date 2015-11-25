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

namespace gfx {class Tilemap;}

namespace config
{

struct _Tilemap
{
  struct _Cell
  {
    struct {
      const int oldw = 30;
      const int neww = 60;
    } width;
    const Point& center();
    const Size& picSize();
    const Size& size();
    void setWidth( int width );
  } cell;

  const int maxBuildingSide = 5;
  const int maxSide = 168;
  const int maxArea = maxSide * maxSide;

  Direction getDirection( const TilePos& b, const TilePos& e );
  unsigned int picWidth2CellSize( int width );

  gfx::Tilemap& invalid();
  const TilePos& invalidLocation();
  const TilePos& unitLocation();
  bool isValidLocation( const TilePos& pos );
};

static _Tilemap tilemap;

}//end namespace gfx

#endif //__CAESARIA_TILEMAP_CONFIG_H_INCLUDED__
