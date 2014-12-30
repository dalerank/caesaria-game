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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_TILEMAP_H_INCLUDED__
#define __CAESARIA_TILEMAP_H_INCLUDED__

#include "core/serializer.hpp"
#include "predefinitions.hpp"
#include "core/scopedptr.hpp"
#include "core/direction.hpp"
#include "gfx/tilesarray.hpp"

namespace gfx
{

// Square Map of the Tiles.
class Tilemap : public Serializable
{
public:  
  static const bool checkCorners = true;

  Tilemap();
  virtual ~Tilemap();
  void resize(const unsigned int size);

  bool isInside( const TilePos& pos ) const;

  Tile& at( const int i, const int j );
  Tile& at( const TilePos& ij );
  
  const Tile& at( const int i, const int j ) const;
  const Tile& at( const TilePos& ij ) const;
  TilesArray allTiles() const;
  const TilesArray& borderTiles() const;
  void addBorder();

  // returns all tiles on a rectangular perimeter
  // (i1, j1) : left corner of the rectangle (minI, minJ)
  // (i2, j2) : right corner of the rectangle (maxI, maxJ)
  // corners  : if false, don't return corner tiles
  TilesArray getRectangle(TilePos start, TilePos stop, const bool corners = true);
  TilesArray getRectangle(TilePos pos, Size size, const bool corners = true );
  TilesArray getRectangle(unsigned int range, TilePos center );

  enum TileNeighbors
  {
    FourNeighbors,
    //Corners,
    AllNeighbors
  };

  TilesArray getNeighbors( TilePos pos, TileNeighbors type = AllNeighbors);

  // returns all tiles in a rectangular area
  // (i1, j1) : left corner of the rectangle (minI, minJ)
  // (i2, j2) : right corner of the rectangle (maxI, maxJ)
  TilesArray getArea(const TilePos& start, const TilePos& stop ) const;
  TilesArray getArea(const TilePos& start, const Size& size ) const;
  TilesArray getArea(int range, const TilePos& center ) const;

  int size() const;

  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);

  void turnRight();
  void turnLeft();

  constants::Direction direction() const;

  TilePos fit( const TilePos& pos ) const;

  Tile* at(const Point& pos, bool overborder);
  TilePos p2tp( const Point& pos );
private: 
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gfx
#endif //__OPENCAESAR3_TILEMAP_H_INCLUDED__
