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

#ifndef __CAESARIA_CITYRENDERER_H_INCLUDED__
#define __CAESARIA_CITYRENDERER_H_INCLUDED__

#include "core/signals.hpp"
#include "core/scopedptr.hpp"
#include "renderermode.hpp"
#include "renderer.hpp"
#include "predefinitions.hpp"
#include "gfx/engine.hpp"

/* Draws the tilemap area on the screen thanks to the GfxEngine, and handle user events */
class CityRenderer : public Renderer
{
public:
  CityRenderer();
  ~CityRenderer();

  void initialize(PlayerCityPtr city, GfxEngine* engine);

  TilemapCamera& getCamera();

  // draws the tilemap on the screen,
  // using a dumb back to front drawing of all pictures.
  void render();

  void handleEvent( NEvent& event);

  Tilemap& getTilemap();

  // sets the current command
  void setMode( Renderer::ModePtr command );
  Renderer::ModePtr getMode() const;

  void animate( unsigned int time );

  void addLayer( LayerPtr layer );

  Point getOffset() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_CITYRENDERER_H_INCLUDED__
