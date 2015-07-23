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

#ifndef __CAESARIA_TILEAREA_H_INCLUDED__
#define __CAESARIA_TILEAREA_H_INCLUDED__

#include "tilesarray.hpp"

namespace gfx
{

class Tilemap;

class TilesArea : public TilesArray
{
public:
  TilesArea();  
  TilesArea( const Tilemap& tmap, const TilePos& leftup, const TilePos& rightdown );
  TilesArea( const Tilemap& tmap, const TilePos& center, int distance );
  TilesArea( const Tilemap& tmap, const TilePos& leftup, const Size& size );

  void reset(const Tilemap& tmap, const TilePos& center, int distance );

  TilesArea& operator=(const TilesArray& other);
};

} //end namespace gfx

#endif //__CAESARIA_TILEAREA_H_INCLUDED__
