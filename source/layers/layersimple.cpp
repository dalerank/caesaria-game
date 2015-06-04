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

#include "layersimple.hpp"
#include "constants.hpp"
#include "walker/constants.hpp"
#include "gfx/city_renderer.hpp"
#include "city/city.hpp"
#include "core/priorities.hpp"
#include "gfx/camera.hpp"
#include "gui/senate_popup_info.hpp"
#include "objects/senate.hpp"

using namespace gfx;

namespace citylayer
{

class Simple::Impl
{
public:
  int ticks;
  SenatePopupInfo senateInfo;
  Picture selectedBuildingPic;
  OverlayPtr lastOverlay;
  object::TypeSet inacceptable;
  bool highlightAny, mayHighlight;
};

int Simple::type() const { return citylayer::simple; }

LayerPtr Simple::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Simple( camera, city ) );
  ret->drop();

  return ret;
}

void Simple::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  OverlayPtr curOverlay = tile.overlay();

  if( _d->mayHighlight )
  {
    bool blowTile = (curOverlay.isValid() && curOverlay == _d->lastOverlay) && _d->highlightAny;
    if( blowTile )
      engine.setColorMask( 0x007f0000, 0x00007f00, 0x0000007f, 0xff000000 );

    Layer::drawTile(engine, tile, offset);

    if( blowTile )
      engine.resetColorMask();
  }
  else
  {
    Layer::drawTile(engine, tile, offset);
  }
}

void Simple::afterRender(Engine& engine)
{
  Layer::afterRender( engine );

  _d->lastOverlay = 0;
  Tile* tile = _currentTile();
  if( tile )
  {
    _d->lastOverlay = tile->overlay();
    _d->highlightAny = false;

    if( is_kind_of<Building>( tile->overlay() )
        && !_d->inacceptable.count( tile->overlay()->type() ) )
      _d->highlightAny = true;
  }

  if( (_d->ticks % 30) == 0 )
  {
    _d->mayHighlight = _city()->getOption( PlayerCity::highlightBuilding );
    _d->ticks++;
  }
}

void Simple::renderUi(Engine &engine)
{
  Layer::renderUi( engine );

  Tile* lastTile = _currentTile();
  if( lastTile )
  {
    OverlayPtr ov = lastTile->overlay();
    SenatePtr senate = ptr_cast<Senate>( ov );
    if( senate.isValid() )
    {
      _d->senateInfo.draw( _lastCursorPos(), Engine::instance(), senate );
    }
  }
}

Simple::Simple( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city ), _d( new Impl )
{
  _addWalkerType( walker::all );

  _d->ticks = 0;
  _d->inacceptable << object::fortification << object::wall;
}

}//end namespace citylayer
