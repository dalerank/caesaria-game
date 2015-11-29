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

void Unemployed::drawTile(const RenderInfo& rinfo, Tile& tile)
{
  if( tile.overlay().isNull() )
  {
    drawLandTile( rinfo, tile );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();
    auto workingBuilding = overlay.as<WorkingBuilding>();
    int worklessPercent = 0;

    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();

      int worklessNumber = (int)house->getServiceValue( Service::recruter );
      int matureNumber = (int)house->habitants().mature_n();
      worklessPercent = math::percentage( worklessNumber, matureNumber );
      needDrawAnimations = (house->level() <= HouseLevel::hovel) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.house );
      }
    }
    else if( workingBuilding.isValid() )
    {
      worklessPercent = math::percentage( workingBuilding->needWorkers(), workingBuilding->maximumWorkers() );
      needDrawAnimations = workingBuilding->needWorkers() > 0;
      if( !needDrawAnimations )
        drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.constr );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else if( worklessPercent > 0 )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, worklessPercent );
    }
  }

  tile.setRendered();
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
        auto house = tile->overlay<House>();
        auto working = tile->overlay<WorkingBuilding>();

        if( house.isValid() )
        {
          int workless = house->getServiceValue( Service::recruter );

          if( workless > 0 )
            text = utils::format( 0xff, "%s %d %s", _("##this_house_have##"), workless, _("##unemployers##") );
          else
            text = "##this_house_haveno_unemployers##";
        }
        else if( working.isValid() )
        {
          int need = working->needWorkers();

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
