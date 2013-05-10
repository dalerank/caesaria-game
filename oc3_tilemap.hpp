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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <vector>
#include <list>

#include "oc3_serializer.hpp"
#include "oc3_predefinitions.hpp"

// Square Map of the Tiles.
class Tilemap : public Serializable
{
public:
   static const bool checkCorners = true;
   
   Tilemap();
   void init(const int size);

   bool is_inside( const TilePos& pos ) const;

   Tile& at( const int i, const int j );
   Tile& at( const TilePos& ij );
   
   const Tile& at( const int i, const int j ) const;
   const Tile& at( const TilePos& ij ) const;

   // returns all tiles on a rectangular perimeter
   // (i1, j1) : left corner of the rectangle (minI, minJ)
   // (i2, j2) : right corner of the rectangle (maxI, maxJ)
   // corners  : if false, don't return corner tiles
   PtrTilesArea getRectangle(const TilePos& start, const TilePos& stope, const bool corners = true );
   PtrTilesArea getRectangle(const TilePos& pos, const Size& size, const bool corners = true );

   // returns all tiles in a rectangular area
   // (i1, j1) : left corner of the rectangle (minI, minJ)
   // (i2, j2) : right corner of the rectangle (maxI, maxJ)
   PtrTilesArea getFilledRectangle( const TilePos& start, const TilePos& stop );
   PtrTilesArea getFilledRectangle( const TilePos& start, const Size& size );
   int getSize() const;

   void save( VariantMap& stream) const;
   void load( const VariantMap& stream);

private:
   TileGrid _tile_array;
   int _size;
};


#endif
