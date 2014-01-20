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

#ifndef __CAESARIA_LAYER_H_INCLUDED__
#define __CAESARIA_LAYER_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "engine.hpp"
#include "tile.hpp"
#include "renderer.hpp"
#include "game/predefinitions.hpp"
#include "core/signals.hpp"

#include <set>

class Layer : public ReferenceCounted
{
public:
  typedef std::set<int> VisibleWalkers;

  virtual int getType() const = 0;
  virtual VisibleWalkers getVisibleWalkers() const = 0;
  virtual void drawTile( GfxEngine& engine, Tile& tile, Point offset ) = 0;

  virtual void drawTileR( GfxEngine& engine, Tile& tile, const Point& offset, const int depth, bool force );

  virtual void handleEvent( NEvent& event );
  virtual void drawTilePass(GfxEngine& engine, Tile& tile, Point offset, Renderer::Pass pass );
  virtual void drawArea( GfxEngine& engine, const TilesArray& area, Point offset,
                         std::string resourceGroup, int tileId );

  virtual void drawColumn(GfxEngine& engine, const Point& pos, const int percent );
  virtual void init( Point cursor );

  virtual void beforeRender(GfxEngine& engine) {}
  virtual void afterRender(GfxEngine& engine);
  virtual void render(GfxEngine& engine);
  virtual void renderPass(GfxEngine& engine, Renderer::Pass pass);

  virtual void registerTileForRendering(Tile&);
  virtual int getNextLayer() const;

  ~Layer();
protected:
  void _setLastCursorPos( Point pos );
  Point _getLastCursorPos() const;
  WalkerList _getVisibleWalkerList();
  void _setStartCursorPos( Point pos );
  Point _getStartCursorPos() const;
  void _drawWalkers( GfxEngine& engine, const Tile& tile, const Point& camOffset);
  void _setTooltipText( std::string text );
  void _loadColumnPicture( int picId );

  TilesArray _getSelectedArea();

  Layer( TilemapCamera& camera, PlayerCityPtr city );
  TilemapCamera* _getCamera();
  PlayerCityPtr _getCity();
  void _setNextLayer(int layer);

  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr<Layer> LayerPtr;

#endif //__CAESAIRAS_LAYER_H_INCLUDED__
