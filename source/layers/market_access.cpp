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

#include "market_access.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "core/event.hpp"
#include "gfx/tilemap_camera.hpp"

using namespace gfx;

namespace citylayer
{

enum { maxAccessLevel=10, accessColumnIndex=15 };
static const char* marketLevelName[maxAccessLevel] = {
                                         "##none_market_access##", "##extremelow_market_access##",
                                         "##verylow_market_access##", "##low_market_access##",
                                         "##bad_market_access##",   "##simple_market_access##",
                                         "##good_market_access##", "##verygood_market_access##",
                                         "##high_market_access##", "##awesome_market_access##"
                                       };

int MarketAccess::type() const {  return citylayer::market; }

void MarketAccess::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  Point screenPos = tile.mappos() + offset;

  if( tile.overlay().isNull() )
  {
    drawPass( engine, tile, offset, Renderer::ground );
    drawPass( engine, tile, offset, Renderer::groundAnimation );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();
    int accessLevel = 0;

    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      HousePtr house = overlay.as<House>();
      accessLevel = (int)house->getServiceValue( Service::market );
      needDrawAnimations = (house->spec().level() <= HouseLevel::hovel) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    }
    else
    {
      drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, OverlayPic::base );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( accessLevel >= 0 )
    {
      drawColumn( engine, screenPos, accessLevel );
    }
  }

  tile.setWasDrawn();
}

LayerPtr MarketAccess::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new MarketAccess( camera, city ) );
  ret->drop();

  return ret;
}

void MarketAccess::handleEvent(NEvent& event)
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
        HousePtr house = tile->overlay().as<House>();
        if( house.isValid() )
        {
          int accessLevel = house->getServiceValue( Service::market );
          accessLevel = math::clamp<int>( accessLevel / maxAccessLevel, 0, maxAccessLevel-1 );
          text = marketLevelName[ accessLevel ];
        }
      }

      _setTooltipText( text );
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

MarketAccess::MarketAccess( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, accessColumnIndex )
{
  _addWalkerType( walker::marketBuyer );
  _addWalkerType( walker::marketLady );
  _addWalkerType( walker::marketKid );

  _initialize();
}

}//end namespace city
