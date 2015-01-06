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

#include "layerwater.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"
#include "tilemap.hpp"
#include "core/event.hpp"
#include "core/gettext.hpp"
#include "tilemap_camera.hpp"
#include "objects/aqueduct.hpp"
#include "core/font.hpp"
#include "core/utils.hpp"

using namespace constants;

namespace gfx
{

namespace layer
{

int Water::type() const{  return citylayer::water;}

void Water::drawTile( Engine& engine, Tile& tile, const Point& offset)
{
  Point screenPos = tile.mappos() + offset;

  bool needDrawAnimations = false;
  Size areaSize(1);

  if( tile.overlay().isNull() )
  {
    //draw background
    engine.draw( tile.picture(), screenPos );
  }
  else
  {
    TileOverlayPtr overlay = tile.overlay();
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;
      areaSize = overlay->size();      
    }
    else
    {
      int tileNumber = 0;
      bool haveWater = tile.param( Tile::pFountainWater ) > 0 || tile.param( Tile::pWellWater ) > 0;
      needDrawAnimations = false;

      if ( overlay->type() == objects::house )
      {
        HousePtr h = ptr_cast<House>( overlay );
        needDrawAnimations = (h->spec().level() == 1) && h->habitants().empty();

        tileNumber = OverlayPic::inHouse;
        haveWater = haveWater || h->hasServiceAccess(Service::fountain) || h->hasServiceAccess(Service::well);
      }

      if( !needDrawAnimations )
      {
        tileNumber += (haveWater ? OverlayPic::haveWater : 0);
        tileNumber += tile.param( Tile::pReservoirWater ) > 0 ? OverlayPic::reservoirRange : 0;

        city::Helper helper( _city() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::waterOverlay, OverlayPic::base + tileNumber );

        areaSize = 0;
      }
    }

    if ( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );

      if( _showWaterValue )
      {
        AqueductPtr aq = ptr_cast<Aqueduct>( tile.overlay() );
        if( aq.isValid() )
        {
          Font f = Font::create( FONT_2 );
          f.setColor( 0xffff0000 );
          int df = aq->water();
          f.draw( engine.screen(), utils::format( 0xff, "%x", df), screenPos + Point( 20, -80 ), false );
        }

        int wellValue = tile.param( Tile::pWellWater );
        int fountainValue = tile.param( Tile::pFountainWater );
        int reservoirWater = tile.param( Tile::pReservoirWater );
        if( wellValue > 0 || fountainValue > 0 || reservoirWater > 0 )
        {
          std::string text = utils::format( 0xff, "%d/%d/%d", wellValue, fountainValue, reservoirWater );
          Font f = Font::create( FONT_2 );
          f.setColor( 0xffff0000 );
          f.draw( engine.screen(), text, screenPos + Point( 20, -80 ), false );
        }
      }
      registerTileForRendering( tile );
    }
  }

  if( !needDrawAnimations && ( tile.isWalkable(true) || tile.getFlag( Tile::tlOverlay ) ) )
  {
    Tilemap& tilemap = _city()->tilemap();
    TilesArray area = tilemap.getArea( tile.pos(), areaSize );

    foreach( it, area )
    {
      Tile* rtile = *it;
      int reservoirWater = rtile->param( Tile::pReservoirWater );
      int fontainWater = rtile->param( Tile::pFountainWater );

      if( (reservoirWater + fontainWater > 0) && ! rtile->getFlag( Tile::tlWater ) && rtile->overlay().isNull() )
      {
        int picIndex = reservoirWater ? OverlayPic::reservoirRange : 0;
        picIndex |= fontainWater > 0 ? OverlayPic::haveWater : 0;
        picIndex |= OverlayPic::skipLeftBorder | OverlayPic::skipRightBorder;
        engine.draw( Picture::load( ResourceGroup::waterOverlay, picIndex + OverlayPic::base ), rtile->mappos() + offset );
      }
    }
  }

  tile.setWasDrawn();
}

void Water::drawWalkerOverlap(Engine& engine, Tile& tile, const Point& offset, const int depth)
{

}

void Water::handleEvent(NEvent& event)
{
  if( event.EventType == sEventKeyboard )
  {
    if( event.keyboard.control && !event.keyboard.pressed && event.keyboard.key == KEY_KEY_E )
    {
      _showWaterValue = !_showWaterValue;
    }
  }

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
        bool isWater = tile->getFlag( Tile::tlWater ) || tile->getFlag( Tile::tlDeepWater );
        if( !isWater )
        {
          int wtrSrvc = (tile->param( Tile::pWellWater ) > 0 ? 1 : 0);
          wtrSrvc |= (tile->param( Tile::pFountainWater ) > 0 ? 2 : 0);
          wtrSrvc |= (tile->param( Tile::pReservoirWater ) > 0 ? 4 : 0);

          switch( wtrSrvc )
          {
          case 0:         /*text = "##water_srvc_no_water##";*/ break;
          case 1:         text = "##water_srvc_well##"; break;
          case 2: case 6: text = "##water_srvc_fountain##"; break;
          case 3:         text = "##water_srvc_fountain_and_well##"; break;
          case 7:         text = "##water_srvc_fountain_and_reservoir##"; break;
          case 4: case 5: text = "##water_srvc_reservoir##"; break;
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

LayerPtr Water::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Water( camera, city ) );
  ret->drop();

  return ret;
}

Water::Water( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city )
{
  _showWaterValue = false;
  _fillVisibleObjects( citylayer::water );
}

}//end namespace layer

}//end namespace gfx
