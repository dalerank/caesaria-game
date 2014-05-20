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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

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

int LayerTroubles::getType() const{  return citylayer::troubles;}
Layer::VisibleWalkers LayerTroubles::getVisibleWalkers() const{  return std::set<int>();}

void LayerTroubles::drawTile( Engine& engine, Tile& tile, Point offset)
{
  Point screenPos = tile.mapPos() + offset;

  if( tile.overlay().isNull() )
  {
    //draw background
    engine.draw( tile.picture(), screenPos );
  }
  else
  {
    bool needDrawAnimations = false;
    TileOverlayPtr overlay = tile.overlay();

    int educationLevel = -1;
    switch( overlay->type() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::plaza:
    case construction::garden:
    case building::elevation:
      needDrawAnimations = true;
    break;

    //other buildings
    default:
    {
      ConstructionPtr c = ptr_cast<Construction>( overlay );
      if( c.isValid() )
      {
        std::string trouble = c->troubleDesc();
        needDrawAnimations = trouble.empty();
        if( !needDrawAnimations )
        {
          city::Helper helper( _city() );
          drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
        }
      }
    }
    break;
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( educationLevel > 0 )
    {
      //drawColumn( engine, screenPos, educationLevel );
    }
  }

  tile.setWasDrawn();
}

LayerPtr LayerTroubles::create( Camera& camera, PlayerCityPtr city )
{
  LayerPtr ret( new LayerTroubles( camera, city ) );
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

LayerTroubles::LayerTroubles( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city )
{
  _loadColumnPicture( 9 );
}

}//end namespace gfx
