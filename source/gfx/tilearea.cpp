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
#include "objects/overlay.hpp"

namespace gfx
{

TilesArea::TilesArea(const Tilemap &tmap, const TilePos& leftup, const TilePos& rightdown)
{
  _size = Size( abs( rightdown.i() - leftup.i() ),
                abs( rightdown.j() - leftup.j() ) );
  append( tmap.area( leftup, rightdown ) );
}

TilesArea::TilesArea(const Tilemap &tmap, int distance, const TilePos& center)
{
  TilePos offset( distance, distance );
  _size = Size( distance ) * 2;
  append( tmap.area( center - offset, center + offset ) );
}

TilesArea::TilesArea(const Tilemap& tmap, int distance, OverlayPtr overlay)
{
  if( overlay.isNull() )
    return;

  TilePos offset( distance, distance  );
  TilePos size( overlay->size().width(), overlay->size().height() );
  TilePos start = overlay->tile().epos();
  _size = overlay->size();
  append( tmap.area( start - offset, start + size + offset ) );
}

TilesArea::TilesArea(const Tilemap &tmap, const TilePos& leftup, const Size& size)
{
  _size = size;
  append( tmap.area( leftup, size ) );
}

TilesArray TilesArea::northSide() const
{
  TilesArray ret;
  for( int i=0; i < _size.width(); i++ )
    ret.push_back( (*this)[i] );

  return ret;
}

TilesArea& TilesArea::operator=(const TilesArray& other)
{
  clear();
  append( other );
  return *this;
}

TilesArea::TilesArea() {}

} //end namespace gfx

