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

#ifndef __CAESARIA_LAYERRELIGION_H_INCLUDED__
#define __CAESARIA_LAYERRELIGION_H_INCLUDED__

#include "layerinfo.hpp"
#include "city_renderer.hpp"

namespace gfx
{

namespace layer
{

class Religion : public Info
{
public:
  virtual int type() const;
  virtual void drawTile( Engine& engine, Tile& tile, const Point& offset );
  virtual void handleEvent(NEvent& event);

  static LayerPtr create( Camera& camera, PlayerCityPtr city );

private:
  Religion( Camera& camera, PlayerCityPtr city );
  std::set<int> _flags;
};

}//end namespace layer

}//end namespace gfx
#endif //__CAESARIA_LAYERRELIGION_H_INCLUDED__
