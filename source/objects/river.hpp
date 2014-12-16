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

#ifndef __CAESARIA_RIVER_H_INCLUDE__
#define __CAESARIA_RIVER_H_INCLUDE__

#include "gfx/tileoverlay.hpp"

class River : public gfx::TileOverlay
{
public:
  River();

  virtual gfx::Picture computePicture();
  void updatePicture();
  RiverList neighbors() const;

  virtual bool build( const CityAreaInfo& info );
  virtual void initTerrain( gfx::Tile &terrain);
  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual void destroy();
  virtual bool isDestructible() const;

  virtual void load(const VariantMap &stream);
  virtual gfx::Renderer::PassQueue passQueue() const;

private:
  int directionFlags;
};

#endif //__CAESARIA_RIVER_H_INCLUDE__
