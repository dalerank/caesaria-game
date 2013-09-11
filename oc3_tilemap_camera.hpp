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


#ifndef TILEMAP_AREA_HPP
#define TILEMAP_AREA_HPP

#include "oc3_size.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_positioni.hpp"

/* A subset of the tilemap, this is the visible area. Has convenient methods to sort tiles per depth */
class TilemapCamera
{
public:
  TilemapCamera();
  ~TilemapCamera();

  void init( Tilemap& tilemap );

  // size of the view in pixel
  void setViewport( const Size& newSize );

  void setCenter( const TilePos& pos );

  void moveRight(const int amount);
  void moveLeft(const int amount);
  void moveUp(const int amount);
  void moveDown(const int amount);

  // return tile coordinates (i, j), in order of depth
  const TilemapArea& getTiles() const;

  int getCenterX() const;
  int getCenterZ() const;
  int getCenterI() const;
  int getCenterJ() const;
  
private:  
  void setCenter( const Point& pos );
  
  int _center_i;
  int _center_j;
  
  Tilemap* _tilemap;  // tile map to display
  int _map_size;      // size of the map  (in tiles)

  class Impl;
  ScopedPtr< Impl > _d;
};


#endif
