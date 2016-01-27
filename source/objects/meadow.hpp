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

#ifndef __CAESARIA_MEADOW_H_INCLUDE__
#define __CAESARIA_MEADOW_H_INCLUDE__

#include "objects/overlay.hpp"

class Meadow : public Overlay
{
public:
  Meadow();
  ~Meadow();

  gfx::TilesArray neighbors() const;

  virtual bool build( const city::AreaInfo& info );
  virtual void initTerrain( gfx::Tile &terrain);
  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual bool isDestructible() const;

  virtual gfx::Renderer::PassQueue passQueue() const;
};

#endif //__CAESARIA_MEADOW_H_INCLUDE__
