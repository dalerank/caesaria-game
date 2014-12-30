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

#ifndef __CAESARIA_LAYERDESIRABILITY_H_INCLUDED__
#define __CAESARIA_LAYERDESIRABILITY_H_INCLUDED__

#include "layerinfo.hpp"

namespace gfx
{

namespace layer
{

class Desirability : public Info
{
public:
  virtual int type() const;
  virtual void drawTile( Engine& engine, Tile& tile, const Point& offset );
  virtual void beforeRender( Engine& engine );

  static LayerPtr create( Camera& camera, PlayerCityPtr city );
  virtual void handleEvent(NEvent& event);
private:
  Desirability( Camera& camera, PlayerCityPtr city );

  class Impl;
  ScopedPtr<Impl> _d;
};

}//

}//end namespace gfx

#endif //__CAESARIA_LAYERDESIRABILITY_H_INCLUDED__
