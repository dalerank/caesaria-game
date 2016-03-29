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
#include "core/spring.hpp"

using namespace gfx;

namespace citylayer
{

class Simple::Impl
{
public:
  int ticks;
  Picture selectedBuildingPic;
  OverlayPtr lastOverlay;
  object::TypeSet inacceptable;

  struct
  {
    bool any,
         may;
    math::SpringI alpha;
  } highlight;
};

int Simple::type() const { return citylayer::simple; }

void Simple::drawTile( const RenderInfo& rinfo, Tile& tile)
{
  if( _d->highlight.may )
  {
    OverlayPtr curOverlay = tile.overlay();
    bool blowTile = (curOverlay.isValid() && curOverlay == _d->lastOverlay) && _d->highlight.any;
    if( blowTile )
    {
      _d->highlight.alpha.update();
      int value = _d->highlight.alpha.value();
      rinfo.engine.setColorMask( value << 16, value << 8, value, 0xff000000 );
    }

    Layer::drawTile( rinfo, tile);

    if( blowTile )
      rinfo.engine.resetColorMask();
  }
  else
  {
    Layer::drawTile( rinfo, tile);
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
    _d->highlight.any = false;

    if( tile->overlay().is<Building>()
        && !_d->inacceptable.count( tile->overlay()->type() ) )
      _d->highlight.any = true;
  }

  if( (_d->ticks++ % 30) == 0 )
  {
    _d->highlight.may = _city()->getOption( PlayerCity::highlightBuilding );
  }
}

void Simple::renderUi(Engine &engine)
{
  Layer::renderUi( engine );
}

Simple::Simple( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city ), _d( new Impl )
{
  _addWalkerType( walker::all );

  _d->ticks = 0;
  _d->highlight.alpha.setCondition( 180, 254, 2 );
  _d->inacceptable << object::fortification << object::wall;

  LayerDrawPassPtr pass( new SenatePopupInfo() );
  pass->drop(); //auto delete

  addDrawPass( LayerDrawPass::gui, pass );
}

}//end namespace citylayer
