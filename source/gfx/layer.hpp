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

#ifndef __CAESARIA_LAYER_H_INCLUDED__
#define __CAESARIA_LAYER_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "engine.hpp"
#include "tile.hpp"
#include "renderer.hpp"
#include "game/predefinitions.hpp"
#include "core/signals.hpp"
#include "walker/constants.hpp"
#include "core/flagholder.hpp"

#include <set>

namespace gfx
{

namespace layer
{

class DrawOptions : public FlagHolder<int>
{
public:
  typedef enum { drawGrid=0x1, shadowOverlay=0x2, showPath=0x4, windowActive=0x8, showRoads=0x10,
                 showObjectArea=0x20, showWalkableTiles=0x40, showLockedTiles=0x80, showFlatTiles=0x100,
                 borderMoving=0x200, mayChangeLayer=0x400, oldGraphics=0x800 } Flags;
  static DrawOptions& instance();

private:
  DrawOptions() {}
};

class Layer : public ReferenceCounted
{
public:
  typedef std::set<int> WalkerTypes;

  virtual int type() const = 0;
  virtual const WalkerTypes& visibleTypes() const;

  //draw gfx tprominent ile
  virtual void drawProminentTile(Engine& engine, Tile& tile, const Point& offset, const int depth, bool force );

  //draw gfx active tile
  virtual void drawTile(Engine& engine, Tile& tile, const Point& offset );

  //draw gfx after walkers
  virtual void drawWalkerOverlap(Engine& engine, Tile& tile, const Point& offset, const int depth );

  virtual void handleEvent( NEvent& event );
  virtual void drawPass(Engine& engine, Tile& tile, const Point& offset, Renderer::Pass pass );
  virtual void drawArea(Engine& engine, const TilesArray& area, const Point& offset,
                        const std::string& resourceGroup, int tileId );

  virtual void drawLands( Engine& engine, Camera* camera );
  virtual void drawWalkers( Engine& engine, const Tile& tile, const Point& camOffset );
  virtual void init( Point cursor );
  virtual void beforeRender( Engine& engine);
  virtual void afterRender( Engine& engine);
  virtual void render( Engine& engine);
  virtual void renderUi( Engine& engine );
  virtual void registerTileForRendering(Tile&);
  virtual void changeLayer( int type );
  virtual int nextLayer() const;

  virtual ~Layer();

protected:
  void _setLastCursorPos( Point pos );
  Point _lastCursorPos() const;
  void _setStartCursorPos( Point pos );
  Point _startCursorPos() const;
  Tile* _currentTile() const;
  void _setTooltipText( const std::string& text );
  void _addWalkerType( constants::walker::Type wtype );
  void _fillVisibleObjects( int ltype );
  WalkerTypes& _visibleWalkers();
  bool _isVisibleObject( int ovType );

  TilesArray _getSelectedArea( TilePos startPos=TilePos(-1,-1) );

  Layer( Camera* camera, PlayerCityPtr city );
  Camera* _camera();
  PlayerCityPtr _city();
  void _setNextLayer(int layer);

  __DECLARE_IMPL(Layer)
};

typedef SmartPtr<Layer> LayerPtr;

}//end namespace layer

}//end namespace gfx

#endif //__CAESAIRAS_LAYER_H_INCLUDED__
