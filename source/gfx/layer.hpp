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
#include "game/predefinitions.hpp"
#include "core/signals.hpp"

#include <set>

namespace gfx
{

class Layer : public ReferenceCounted
{
public:
  typedef std::set<int> VisibleWalkers;

  virtual int type() const = 0;
  virtual VisibleWalkers getVisibleWalkers() const = 0;

  //draw gfx before walkers
  virtual void drawTileR( Engine& engine, Tile& tile, const Point& offset, const int depth, bool force );

  //draw gfx active tile
  virtual void drawTile( Engine& engine, Tile& tile, Point offset );

  //draw gfx after walkers
  virtual void drawTileW( Engine& engine, Tile& tile, const Point& offset, const int depth );

  virtual void handleEvent( NEvent& event );
  virtual void drawPass( Engine& engine, Tile& tile, Point offset, Renderer::Pass pass );
  virtual void drawArea( Engine& engine, const TilesArray& area, Point offset,
                         std::string resourceGroup, int tileId );

  virtual void drawColumn( Engine& engine, const Point& pos, const int percent );
  virtual void init( Point cursor );

  virtual void beforeRender( Engine& engine) {}
  virtual void afterRender( Engine& engine);
  virtual void render( Engine& engine);
  virtual void renderPass( Engine& engine, Renderer::Pass pass);

  virtual void registerTileForRendering(Tile&);
  virtual int getNextLayer() const;

  virtual ~Layer();
protected:
  void _setLastCursorPos( Point pos );
  Point _lastCursorPos() const;
  WalkerList _getVisibleWalkerList( const VisibleWalkers& aw, const TilePos& pos );
  void _setStartCursorPos( Point pos );
  Point _startCursorPos() const;
  void _drawWalkers( Engine& engine, const Tile& tile, const Point& camOffset );
  void _setTooltipText( std::string text );
  void _loadColumnPicture( int picId );

  TilesArray _getSelectedArea();

  Layer(Camera* camera, PlayerCityPtr city );
  Camera* _camera();
  PlayerCityPtr _city();
  void _setNextLayer(int layer);

  __DECLARE_IMPL(Layer)
};

typedef SmartPtr<Layer> LayerPtr;

}

#endif //__CAESAIRAS_LAYER_H_INCLUDED__
