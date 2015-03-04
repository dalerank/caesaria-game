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

#ifndef __CAESARIA_LAYERSIMPLE_H_INCLUDED__
#define __CAESARIA_LAYERSIMPLE_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "layer.hpp"

namespace citylayer
{

class Simple : public Layer
{
public:
  virtual int type() const;
  static LayerPtr create(gfx::Camera& camera, PlayerCityPtr city );
  virtual void drawTile(gfx::Engine &engine, gfx::Tile &tile, const Point &offset);
  virtual void afterRender(gfx::Engine &engine);
  virtual void renderUi(gfx::Engine &engine);

protected:
  Simple(gfx::Camera& camera, PlayerCityPtr city );

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace citylayer
#endif //__CAESARIA_LAYERSIMPLE_H_INCLUDED__
