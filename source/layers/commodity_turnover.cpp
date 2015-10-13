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

#include "commodity_turnover.hpp"
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

class CommodityTurnover::Impl
{
public:
  struct {
    OverlayPtr selected;
    OverlayPtr current;
  } overlay;
};

int CommodityTurnover::type() const {  return citylayer::comturnover; }

void CommodityTurnover::drawTile(Engine& engine, Tile& tile, const Point& offset)
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
      auto house = overlay.as<House>();
      accessLevel = (int)house->getServiceValue( Service::market );
      needDrawAnimations = (house->level() <= HouseLevel::hovel) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, config::id.overlay.inHouseBase );
      }
    }
    else
    {
      drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, config::id.overlay.base );
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

  tile.setRendered();
}

LayerPtr CommodityTurnover::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new CommodityTurnover( camera, city ) );
  ret->drop();

  return ret;
}

void CommodityTurnover::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {    
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      if( tile != 0 )
        _d->overlay.current = tile->overlay();
    }
    break;

    case mouseLbtnPressed:
    {
      _d->overlay.selected = _d->overlay.current;
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

CommodityTurnover::CommodityTurnover( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, accessColumnIndex )
{
  _addWalkerType( walker::cartPusher );
  _addWalkerType( walker::supplier );
  _addWalkerType( walker::marketBuyer );
  _addWalkerType( walker::marketLady );
  _addWalkerType( walker::marketKid );

  _initialize();
}

}//end namespace city
