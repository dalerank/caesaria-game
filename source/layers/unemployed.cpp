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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "unemployed.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"
#include "core/priorities.hpp"
#include "city/statistic.hpp"
#include "core/event.hpp"
#include "gfx/tilemap_camera.hpp"
#include "core/gettext.hpp"

using namespace gfx;

namespace citylayer
{

int Unemployed::type() const {  return citylayer::unemployed; }

void Unemployed::drawTile(Engine& engine, Tile& tile, const Point& offset)
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
    WorkingBuildingPtr workBuilding = overlay.as<WorkingBuilding>();
    int worklessPercent = 0;

    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      HousePtr house = overlay.as<House>();

      int worklessNumber = (int)house->getServiceValue( Service::recruter );
      int matureNumber = (int)house->habitants().mature_n();
      worklessPercent = math::percentage( worklessNumber, matureNumber );
      needDrawAnimations = (house->spec().level() == 1) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    }
    else if( workBuilding.isValid() )
    {
      worklessPercent = math::percentage( workBuilding->needWorkers(), workBuilding->maximumWorkers() );
      needDrawAnimations = workBuilding->needWorkers() > 0;
      if( !needDrawAnimations )
        drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, OverlayPic::base );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( worklessPercent > 0 )
    {
      drawColumn( engine, screenPos, worklessPercent );
    }
  }

  tile.setWasDrawn();
}

LayerPtr Unemployed::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Unemployed( camera, city ) );
  ret->drop();

  return ret;
}

void Unemployed::handleEvent(NEvent& event)
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
        WorkingBuildingPtr workBuilding = tile->overlay().as<WorkingBuilding>();

        if( house.isValid() )
        {
          int workless = house->getServiceValue( Service::recruter);

          if( workless > 0 )
            text = utils::format( 0xff, "%s %d %s", _("##this_house_have##"), workless, _("##unemployers##") );
          else
            text = "##this_house_haveno_unemployers##";
        }
        else if( workBuilding.isValid() )
        {
          int need = workBuilding->needWorkers();

          if( need > 0 )
            text = utils::format( 0xff, "%s %d %s", _("##this_building_need##"), need, _("##workers##") );
          else
            text = "##this_building_have_all_workers##";
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

Unemployed::Unemployed( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 15 )
{
  _visibleWalkers() << walker::recruter;
  _initialize();
}

}//end namespace citylayer
