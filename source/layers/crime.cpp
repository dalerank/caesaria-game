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

#include "crime.hpp"
#include "objects/overlay.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "game/resourcegroup.hpp"
#include "city/statistic.hpp"
#include "constants.hpp"
#include "core/gettext.hpp"
#include "core/event.hpp"
#include "gfx/camera.hpp"

using namespace gfx;

namespace citylayer
{

static const std::string crimeDesc[] =
{
  "##none_crime_risk##",
  "##very_low_crime_risk##",
  "##low_crime_risk##",
  "##few_crime_risk##"
  "##some_crime_risk##",
  "##peaceful_crime_risk##",
  "##several_crimes_but_area_secure##",
  "##dangerous_crime_risk##"
  "##averange_crime_risk##",
  "##high_crime_risk##",
  "##extreme_crime_risk##"
};

int Crime::type() const { return citylayer::crime; }

void Crime::drawTile( const RenderInfo& rinfo, Tile& tile)
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
    int crime = -1;

    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();
      crime = (int)house->getServiceValue( Service::crime );
      needDrawAnimations = (house->level() <= HouseLevel::hovel) && house->habitants().empty(); // In case of vacant terrain

      drawArea( rinfo, overlay->area(), ResourceGroup::foodOverlay, config::id.overlay.inHouseBase  );
    }
    else
    {
      drawArea( rinfo, overlay->area(), ResourceGroup::foodOverlay, config::id.overlay.base  );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else if( crime >= 0)
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, crime );
    }
  }

  tile.setRendered();
}

LayerPtr Crime::create(Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Crime( camera, city ) );
  ret->drop();

  return ret;
}

void Crime::handleEvent(NEvent& event)
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
        auto house = tile->overlay<House>();
        if( house != 0 )
        {
          int crime = (int)house->getServiceValue( Service::crime );
          text = crimeDesc[ math::clamp<int>( crime / 11, 0, 7 ) ];
        }
      }

      _setTooltipText( _(text) );
    }
    break;

    default:

    break;
    }
  }

  Layer::handleEvent( event );
}

Crime::Crime( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 18 )
{
  _addWalkerType( walker::prefect );
  _initialize();
}

}//end namespace citylayer
