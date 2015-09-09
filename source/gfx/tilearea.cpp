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

#include "tilearea.hpp"
#include "tilemap.hpp"

namespace gfx
{

TilesArea::TilesArea(const Tilemap &tmap, const TilePos& leftup, const TilePos& rightdown)
{
  append( tmap.getArea( leftup, rightdown ) );
}

TilesArea::TilesArea(const Tilemap &tmap, int distance, const TilePos& center)
{
  add( tmap, center, distance );
}

TilesArea::TilesArea(const Tilemap &tmap, const TilePos& leftup, const Size& size)
{
  append( tmap.getArea( leftup, size ) );
}

void TilesArea::add(const Tilemap& tmap, const TilePos& center, int distance)
{
  TilePos offset( distance, distance );
  append( tmap.getArea( center - offset, center + offset ) );
}

TilesArea& TilesArea::operator=(const TilesArray& other)
{
  clear();
  append( other );
  return *this;
}

TilesArea::TilesArea() {}

} //end namespace gfx

