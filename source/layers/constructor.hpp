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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_LAYER_CONSTRUCTOR_H_INCLUDED__
#define __CAESARIA_LAYER_CONSTRUCTOR_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "layer.hpp"
#include "gfx/renderer.hpp"

namespace citylayer
{

class Constructor : public Layer
{
public:
  virtual void handleEvent(NEvent &event);
  virtual int type() const;
  virtual void drawTile(const gfx::RenderInfo& rinfo, gfx::Tile& tile);
  virtual void render( gfx::Engine &engine);
  virtual void init(Point cursor);
  virtual void drawProminentTile( const gfx::RenderInfo& renderInfo, gfx::Tile& tile, const int depth, bool force);
  virtual void beforeRender(gfx::Engine &engine);
  virtual void drawPass( const gfx::RenderInfo& rinfo, gfx::Tile &tile, gfx::Renderer::Pass pass);
  virtual void afterRender(gfx::Engine &engine);
  virtual const WalkerTypes& visibleTypes() const;
  virtual void renderUi(gfx::Engine &engine);
  virtual void changeLayer(int type);

  static LayerPtr create(gfx::Renderer &renderer, PlayerCityPtr city );

  virtual ~Constructor();
public signals:
  Signal3<object::Type,TilePos,int>& onBuild();

private:
  void _updatePreviewTiles(bool force);
  void _checkPreviewBuild(const TilePos& pos);
  void _checkBuildArea();
  void _discardPreview();
  void _buildAll();
  void _finishBuild();
  void _initBuildMode();
  void _drawBuildTiles( const gfx::RenderInfo& renderInfo );
  void _drawBuildTile( const gfx::RenderInfo& renderInfo, gfx::Tile* tile);
  void _tryDrawBuildTile( const gfx::RenderInfo& renderInfo, gfx::Tile& tile);
  void _exitBuildMode();

  Constructor( gfx::Renderer& renderer, PlayerCityPtr city );

  __DECLARE_IMPL(Constructor)
};

}//end namespace citylayer

#endif //__CAESARIA_LAYER_CONSTRUCTOR_H_INCLUDED__
