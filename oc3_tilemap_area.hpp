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


#include <list>
#include <vector>

#include "oc3_tilemap.hpp"

/* A subset of the tilemap, this is the visible area. Has convenient methods to sort tiles per depth */
class TilemapArea
{
public:
  TilemapArea();
  ~TilemapArea();

  void init(Tilemap& tilemap);

  // size of the view in pixel
  void setViewSize(const int width, const int height);

  void setCenterIJ( const TilePos& pos );

  void moveRight(const int amount);
  void moveLeft(const int amount);
  void moveUp(const int amount);
  void moveDown(const int amount);

  // return tile coordinates (i, j), in order of depth
  const std::vector< TilePos >& getTiles();

  int getCenterX() const;
  int getCenterZ() const;
  int getCenterI() const;
  int getCenterJ() const;
  
private:
  
  void setCenterXZ(const int x, const int z);
  
  int _center_i;
  int _center_j;
  
  Tilemap* _tilemap;  // tile map to display
  int _map_size;      // size of the map  (in tiles)
  int _view_width;    // width of the view (in tiles)  nb_tilesX = 1+2*_view_width
  int _view_height;   // height of the view (in tiles)  nb_tilesY = 1+2*_view_height
  int _center_x;      // horizontal center of the view (in tiles)
  int _center_z;      // vertical center of the view (in tiles)
  std::vector< TilePos > _coordinates;  // cached list of visible tiles
};


#endif
