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

#include "layertax.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "constants.hpp"
#include "core/event.hpp"
#include "gfx/camera.hpp"
#include "core/gettext.hpp"
#include "game/gamedate.hpp"
#include "city/statistic.hpp"

using namespace gfx;

namespace citylayer
{

int Tax::type() const {  return citylayer::tax; }

void Tax::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  Point screenPos = tile.mappos() + offset;

  if( tile.overlay().isNull() )
  {
    //draw background
    //engine.draw( tile.picture(), screenPos );

    drawPass( engine, tile, offset, Renderer::ground );
    drawPass( engine, tile, offset, Renderer::groundAnimation );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();

    int taxLevel = -1;
    if( _isVisibleObject( overlay->type() ) )
    {
    // Base set of visible objects
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      HousePtr house = ptr_cast<House>( overlay );
      int taxAccess = house->getServiceValue( Service::forum );
      taxLevel = math::clamp<int>( house->taxesThisYear(), 0, 100 );
      needDrawAnimations = ((house->spec().level() == 1 && house->habitants().empty())
                            || taxAccess < 25);

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
    else if( taxLevel > 0 )
    {
      drawColumn( engine, screenPos, taxLevel );
    }
  }

  tile.setWasDrawn();
}

LayerPtr Tax::create( Camera& camera, PlayerCityPtr city )
{
  LayerPtr ret( new Tax( camera, city ) );
  ret->drop();

  return ret;
}

void Tax::handleEvent(NEvent& event)
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
        BuildingPtr bld = ptr_cast<Building>( tile->overlay() );

        if( bld.isNull() )
        {
          text = "##no_people_in_this_locality##";
        }
        else
        {
          HousePtr house = tile->overlay().as<House>();
          if( house.isValid() )
          {
            bool lastTaxationTooOld = house->lastTaxationDate().monthsTo( game::Date::current() ) > DateTime::monthsInYear / 2;
            if( lastTaxationTooOld )
              text = "##house_not_registered_for_taxes##";
          }
        }
      }

      _setTooltipText( _(text) );
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

Tax::Tax( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 9 )
{
  if( !city->getOption( PlayerCity::c3gameplay ) )
    _loadColumnPicture( ResourceGroup::sprites, 124 );

  _addWalkerType( walker::taxCollector );
  _initialize();
}

}//end namespace citylayer
