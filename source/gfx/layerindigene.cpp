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

#include "layerindigene.hpp"
#include "objects/constants.hpp"
#include "objects/native.hpp"
#include "game/resourcegroup.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"
#include "core/event.hpp"
#include "tilemap_camera.hpp"

using namespace constants;

namespace gfx
{

namespace layer
{

int Indigene::type() const
{
  return citylayer::aborigen;
}

void Indigene::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  Point screenPos = tile.mappos() + offset;

  if( tile.overlay().isNull() )
  {
    //draw background
    engine.draw( tile.picture(), screenPos );
  }
  else
  {
    bool needDrawAnimations = false;
    TileOverlayPtr overlay = tile.overlay();
    int discontentLevel = 0;
    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == objects::native_hut )
    {
      NativeHutPtr hut = ptr_cast<NativeHut>( overlay );
      discontentLevel = (int)hut->discontent();
      needDrawAnimations = false;

      city::Helper helper( _city() );
      drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
    }
    else
    {
      city::Helper helper( _city() );
      drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( discontentLevel >= 0 )
    {
      _addColumn( screenPos, discontentLevel );
    }
  }

  tile.setWasDrawn();
}

LayerPtr Indigene::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Indigene( camera, city ) );
  ret->drop();

  return ret;
}

void Indigene::handleEvent(NEvent& event)
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

Indigene::Indigene( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 15 )
{
  _visibleWalkers() << walker::indigene << walker::missioner;
  _fillVisibleObjects( citylayer::aborigen );
}

}

}//end namespace gfx
