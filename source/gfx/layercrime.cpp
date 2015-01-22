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

#include "layercrime.hpp"
#include "tileoverlay.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "layerconstants.hpp"
#include "core/gettext.hpp"
#include "core/event.hpp"
#include "camera.hpp"

using namespace constants;

namespace gfx
{

namespace layer
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
  "##high_crime_risk##"
};

int Crime::type() const {  return citylayer::crime; }

void Crime::drawTile( Engine& engine, Tile& tile, const Point& offset)
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
    int crime = -1;

    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == objects::house )
    {
      HousePtr house = ptr_cast<House>( overlay );
      crime = (int)house->getServiceValue( Service::crime );
      needDrawAnimations = (house->spec().level() == 1) && house->habitants().empty(); // In case of vacant terrain

      city::Helper helper( _city() );
      drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase  );
    }
    else
    {
      city::Helper helper( _city() );
      drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base  );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( crime >= 0)
    {
      _addColumn( screenPos, crime );
    }
  }

  tile.setWasDrawn();
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
        HousePtr house = ptr_cast<House>( tile->overlay() );
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
  _fillVisibleObjects( type() );
}

}//end namespace layer

}//end namespace gfx
