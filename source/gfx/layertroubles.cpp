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

#include "layertroubles.hpp"
#include "layerconstants.hpp"
#include "core/gettext.hpp"
#include "objects/house.hpp"
#include "objects/constants.hpp"
#include "city/helper.hpp"
#include "objects/house_level.hpp"
#include "good/goodhelper.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "objects/watersupply.hpp"
#include "tilemap_camera.hpp"
#include "objects/factory.hpp"

using namespace constants;

namespace gfx
{

int LayerTroubles::type() const{  return _type;}

void LayerTroubles::drawTile(Engine& engine, Tile& tile, const Point& offset)
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

    switch( overlay->type() )
    {
    // Base set of visible objects
    case construction::road:
    case construction::plaza:
    case construction::garden:

    case building::burnedRuins:
    case building::collapsedRuins:

    case building::lowBridge:
    case building::highBridge:

    case building::elevation:
    case building::rift:
      needDrawAnimations = true;
    break;

    //other buildings
    default:
    {
      ConstructionPtr c = ptr_cast<Construction>( overlay );
      if( c.isValid() )
      {
        std::string trouble = c->troubleDesc();
        needDrawAnimations = !trouble.empty();
      }
    }
    break;
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else
    {
      city::Helper helper( _city() );
      drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
    }
  }

  tile.setWasDrawn();
}

LayerPtr LayerTroubles::create(Camera& camera, PlayerCityPtr city , int type)
{
  LayerPtr ret( new LayerTroubles( camera, city, type ) );
  ret->drop();

  return ret;
}

void LayerTroubles::handleEvent(NEvent& event)
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
        ConstructionPtr constr = ptr_cast<Construction>( tile->overlay() );
        if( constr.isValid() )
        {
          text = constr->troubleDesc();

          if( text.empty() )
          {
            WorkingBuildingPtr wb = ptr_cast<WorkingBuilding>( constr );
            if( wb.isValid() )
            {
              int laborAccess = wb->laborAccessPercent();
              if( wb->getAccessRoads().empty() || laborAccess == 0 )
              {
                text = "##working_have_no_labor_access##";
              }
              else
              {
                if( laborAccess < 20 ) { text = "##working_have_bad_labor_access##"; }
                else if( laborAccess < 40 ) { text = "##working_have_very_little_labor_access##"; }
                else if( laborAccess < 60 ) { text = "##working_have_some_labor_access##"; }
                else if( laborAccess < 80 ) { text = "##working_have_good_labor_access##"; }
                else if( laborAccess <= 100 ) { text = "##working_have_awsesome_labor_access##"; }
              }
            }
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

LayerTroubles::LayerTroubles( Camera& camera, PlayerCityPtr city, int type )
  : Layer( &camera, city ), _type( type )
{
  //_loadColumnPicture( 9 );
}

}//end namespace gfx
