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

#ifndef __CAESARIA_LAYERDESTROY_H_INCLUDED__
#define __CAESARIA_LAYERDESTROY_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "gfx/layer.hpp"
#include "city_renderer.hpp"

namespace gfx
{

class LayerDestroy : public Layer
{
public:
  virtual void handleEvent( NEvent& event );
  virtual int getType() const;
  virtual std::set<int> getVisibleWalkers() const;
  virtual void drawTile( Engine& engine, Tile& tile, Point offset );
  virtual void render( Engine& engine);

  static LayerPtr create( Camera& camera, PlayerCityPtr city );

private:
  LayerDestroy( Camera& camera, PlayerCityPtr city );

  void _drawTileInSelArea( Engine& engine, Tile& tile, Tile* master, const Point& offset);
  void _clearAll();

  Picture _clearPic;
};

}//end namespace gfx
#endif //__CAESARIA_LAYERDESTROY_H_INCLUDED__
