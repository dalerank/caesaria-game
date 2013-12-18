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


#ifndef _CAESARIA_TILEMAP_CAMERA_H_INCLUDE_
#define _CAESARIA_TILEMAP_CAMERA_H_INCLUDE_

#include "core/size.hpp"
#include "core/scopedptr.hpp"
#include "tilemap.hpp"
#include "core/position.hpp"
#include "core/signals.hpp"

/* A subset of the tilemap, this is the visible area. Has convenient methods to sort tiles per depth */
class TilemapCamera
{
public:
  TilemapCamera();
  ~TilemapCamera();

  void init( Tilemap& tilemap );

  // size of the view in pixel
  void setViewport(Size newSize);
  void setCenter(TilePos pos);

  void move(PointF relative);
  void moveRight(const int amount);
  void moveLeft(const int amount);
  void moveUp(const int amount);
  void moveDown(const int amount);

  Point getOffset() const;

  // return tile coordinates (i, j), in order of depth
  const TilesArray& getTiles() const;

  int getCenterX() const;
  int getCenterZ() const;
  TilePos getCenter() const;

  void setScrollSpeed( int speed );
  int getScrollSpeed() const;

  Tile* at(Point pos, bool overborder ) const;
  Tile* getCenterTile() const;

  void startFrame();

public oc3_signals:
  Signal1<Point>& onPositionChanged();
  
private:
  void setCenter(Point pos);

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_TILEMAP_CAMERA_H_INCLUDE_
