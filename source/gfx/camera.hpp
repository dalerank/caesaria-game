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

#ifndef __CAESARIA_GFX_CAMERA_H_INCLUDED__
#define __CAESARIA_GFX_CAMERA_H_INCLUDED__

#include "engine.hpp"
#include "core/direction.hpp"
#include "core/signals.hpp"

namespace gfx
{

class Tile;
class TilesArray;

class Camera
{
public:
  virtual Tile* at( const TilePos& p ) const = 0;
  virtual Tile* at( const Point& p, bool overborder ) const = 0;
  virtual Point offset() const = 0;
  virtual void move(PointF relative) = 0;
  virtual void moveRight(const int amount) = 0;
  virtual void moveLeft(const int amount) = 0;
  virtual void moveUp(const int amount) = 0;
  virtual void moveDown(const int amount) = 0;
  virtual const TilesArray& tiles() const = 0;
  virtual const TilesArray& flatTiles() const = 0;
  virtual int centerX() const = 0;
  virtual int centerZ() const = 0;
  virtual TilePos center() const = 0;
  virtual void setScrollSpeed( int speed ) = 0;
  virtual int scrollSpeed() const = 0;
  virtual Tile* centerTile() const = 0;
  virtual void startFrame() = 0;
  virtual void setCenter( TilePos pos ) = 0;
  virtual void refresh() = 0;

  virtual ~Camera() {}

public signals:
  virtual Signal1<Point>& onPositionChanged() = 0;
  virtual Signal1<constants::Direction>& onDirectionChanged() = 0;
};

} //end namespace gfx

#endif //__CAESARIA_GFX_CAMERA_H_INCLUDED__
