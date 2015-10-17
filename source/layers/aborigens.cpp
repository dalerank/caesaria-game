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

#include "aborigens.hpp"
#include "objects/constants.hpp"
#include "objects/native.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "core/priorities.hpp"
#include "core/event.hpp"
#include "gfx/tilemap_camera.hpp"

using namespace gfx;

namespace citylayer
{

int Aborigens::type() const { return citylayer::aborigen; }

void Aborigens::drawTile( const RenderInfo& rinfo, Tile& tile )
{
  if( tile.overlay().isNull() )
  {
    drawPass( rinfo, tile, Renderer::ground );
    drawPass( rinfo, tile, Renderer::groundAnimation );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();
    int discontentLevel = 0;
    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::native_hut )
    {
      auto nativeHut = overlay.as<NativeHut>();
      discontentLevel = (int)nativeHut->discontent();
      needDrawAnimations = false;

      drawArea( rinfo, overlay->area(), ResourceGroup::foodOverlay, config::id.overlay.inHouseBase );
    }
    else
    {
      drawArea( rinfo, overlay->area(), ResourceGroup::foodOverlay, config::id.overlay.base );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else if( discontentLevel >= 0 )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, discontentLevel );
    }
  }

  tile.setRendered();
}

LayerPtr Aborigens::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Aborigens( camera, city ) );
  ret->drop();

  return ret;
}

void Aborigens::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      std::string text = "";
      if( tile != 0 )
      {
      }

      _setTooltipText( text );
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

Aborigens::Aborigens( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 15 )
{
  _visibleWalkers() << walker::indigene << walker::missioner;
  _initialize();
}

}//end namespace citylayer
