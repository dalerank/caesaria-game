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

#ifndef __CAESARIA_CITYRENDERER_H_INCLUDED__
#define __CAESARIA_CITYRENDERER_H_INCLUDED__

#include "core/signals.hpp"
#include "core/scopedptr.hpp"
#include "gfx/renderermode.hpp"
#include "gfx/renderer.hpp"
#include "predefinitions.hpp"
#include "gfx/engine.hpp"

namespace gfx
{

/* Draws the tilemap area on the screen thanks to the GfxEngine, and handle user events */
class CityRenderer : public gfx::Renderer
{
public:
  CityRenderer();
  ~CityRenderer();

  void initialize(PlayerCityPtr city, gfx::Engine* engine, gui::GuiEnv *guienv);

  gfx::Camera* camera();

  // draws the tilemap on the screen,
  // using a dumb back to front drawing of all pictures.
  void render();

  void handleEvent( NEvent& event);

  gfx::Tilemap& getTilemap();

  // sets the current command
  void setMode( Renderer::ModePtr command );
  Renderer::ModePtr getMode() const;

  void animate( unsigned int time );

  void addLayer( LayerPtr layer );
  void setLayer( int layertype );
  int  layerType() const;

  Point getOffset() const;

  TilePos getTilePos( Point point ) const;

  void setViewport( const Size& size );

public oc3_signals:
  Signal1<int>& onLayerSwitch();

public oc3_slots:
  void rotateRight();
  void rotateLeft();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gfx

#endif //__CAESARIA_CITYRENDERER_H_INCLUDED__
