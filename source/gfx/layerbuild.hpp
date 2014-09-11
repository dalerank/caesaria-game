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

#ifndef __CAESARIA_LAYERBUILD_H_INCLUDED__
#define __CAESARIA_LAYERBUILD_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "gfx/layer.hpp"
#include "renderer.hpp"

namespace gfx
{

class LayerBuild : public Layer
{
public:
  virtual void handleEvent(NEvent &event);
  virtual int type() const;
  virtual void drawTile( Engine& engine, Tile& tile, const Point& offset );
  virtual void render( Engine &engine);
  virtual void init(Point cursor);

  static LayerPtr create( Renderer* renderer, PlayerCityPtr city );

  virtual ~LayerBuild();
private:
  void _updatePreviewTiles(bool force);
  void _checkPreviewBuild(TilePos pos);
  void _discardPreview();
  void _buildAll();
  void _finishBuild();
  void _drawBuildTiles( Engine& engine );

  LayerBuild( Renderer* renderer, PlayerCityPtr city );

  __DECLARE_IMPL(LayerBuild)
};

}//end namespace gfx
#endif //__CAESARIA_LAYERBUILD_H_INCLUDED__
