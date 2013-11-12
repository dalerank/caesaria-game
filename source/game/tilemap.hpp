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


#ifndef __OPENCAESAR3_TILEMAP_H_INCLUDED__
#define __OPENCAESAR3_TILEMAP_H_INCLUDED__

#include "core/serializer.hpp"
#include "predefinitions.hpp"
#include "core/scopedptr.hpp"

// Square Map of the Tiles.
class Tilemap : public Serializable
{
public:  
  static const bool checkCorners = true;

  Tilemap();
  virtual ~Tilemap();
  void resize(const int size);

  bool isInside( const TilePos& pos ) const;

  Tile& at( const int i, const int j );
  Tile& at( const TilePos& ij );
  
  const Tile& at( const int i, const int j ) const;
  const Tile& at( const TilePos& ij ) const;

  // returns all tiles on a rectangular perimeter
  // (i1, j1) : left corner of the rectangle (minI, minJ)
  // (i2, j2) : right corner of the rectangle (maxI, maxJ)
  // corners  : if false, don't return corner tiles
  TilemapArea getRectangle(const TilePos& start, const TilePos& stope, const bool corners = true );
  TilemapArea getRectangle(const TilePos& pos, const Size& size, const bool corners = true );

  // returns all tiles in a rectangular area
  // (i1, j1) : left corner of the rectangle (minI, minJ)
  // (i2, j2) : right corner of the rectangle (maxI, maxJ)
  TilemapArea getArea( const TilePos& start, const TilePos& stop );
  TilemapArea getArea( const TilePos& start, const Size& size );
  int getSize() const;

  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);

  TilePos fit( const TilePos& pos ) const;

private: 
  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //__OPENCAESAR3_TILEMAP_H_INCLUDED__
