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

#ifndef __CAESARIA_TERRAIN_H_INCLUDE__
#define __CAESARIA_TERRAIN_H_INCLUDE__

#include "objects/overlay.hpp"

class Terrain : public Overlay
{
public:
  Terrain();

  virtual gfx::Picture computePicture();
  void updatePicture();
  RiverList neighbors() const;

  virtual bool build( const city::AreaInfo& info );
  virtual void initTerrain( gfx::Tile &terrain);
  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual void destroy();
  virtual bool isDestructible() const;

  virtual gfx::Renderer::PassQueue passQueue() const;
  static gfx::Picture randomPicture();
};

#endif //__CAESARIA_TERRAIN_H_INCLUDE__
