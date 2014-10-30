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

#ifndef _CAESARIA_TILEMAP_CAMERA_H_INCLUDE_
#define _CAESARIA_TILEMAP_CAMERA_H_INCLUDE_

#include "core/size.hpp"
#include "core/scopedptr.hpp"
#include "predefinitions.hpp"
#include "tilemap.hpp"
#include "camera.hpp"
#include "core/position.hpp"
#include "core/signals.hpp"

namespace gfx
{

/* A subset of the tilemap, this is the visible area. Has convenient methods to sort tiles per depth */
class TilemapCamera : public Camera
{
public:
  TilemapCamera();
  virtual ~TilemapCamera();

  void init( Tilemap& tilemap, Size size );

  // size of the view in pixel
  void setViewport(Size newSize);
  void setCenter(TilePos pos);

  void move(PointF relative);
  void moveRight(const int amount);
  void moveLeft(const int amount);
  void moveUp(const int amount);
  void moveDown(const int amount);

  Point offset() const;

  // return tile coordinates (i, j), in order of depth
  virtual const TilesArray& tiles() const;
  virtual const TilesArray& flatTiles() const;

  int centerX() const;
  int centerZ() const;
  TilePos center() const;

  void setScrollSpeed( int speed );
  int scrollSpeed() const;

  Tile* at( const Point& pos, bool overborder ) const;
  Tile* at( const TilePos& pos ) const;
  Tile* centerTile() const;

  virtual void startFrame();
  virtual void refresh();

public signals:
  virtual Signal1<Point>& onPositionChanged();
  virtual Signal1<constants::Direction>& onDirectionChanged();
  
private:
  void _setCenter(Point pos, bool checkBorder);

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gfx

#endif //_CAESARIA_TILEMAP_CAMERA_H_INCLUDE_
