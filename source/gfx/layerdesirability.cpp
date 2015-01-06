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

#include "layerdesirability.hpp"
#include "game/resourcegroup.hpp"
#include "camera.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "city/helper.hpp"
#include "core/font.hpp"
#include "core/event.hpp"
#include "core/utils.hpp"
#include "layerconstants.hpp"

using namespace constants;

namespace gfx
{

namespace layer
{

class Desirability::Impl
{
public:
  Font debugFont;
  std::vector<Picture*> debugText;
};

namespace {
  inline int __des2index( int desirability )
  {
    return desirability < 0
             ? math::clamp( desirability / 25, -3, 0 )
             : math::clamp( desirability / 10, 0, 6 );
  }
}

int Desirability::type() const {  return citylayer::desirability; }

void Desirability::drawTile( Engine& engine, Tile& tile, const Point& offset)
{
  //Tilemap& tilemap = _city->getTilemap();
  Point screenPos = tile.mappos() + offset;

  int desirability = tile.param( Tile::pDesirability );
  if( tile.overlay().isNull() )
  {
    //draw background
    if( tile.getFlag( Tile::isConstructible ) && desirability != 0 )
    {
      int picOffset = __des2index( desirability );
      Picture& pic = Picture::load( ResourceGroup::land2a, 37 + picOffset );

      engine.draw( pic, screenPos );
    }
    else
    {
      engine.draw( tile.picture(), screenPos );
    }
  }
  else
  {
    TileOverlayPtr overlay = tile.overlay();
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else
    {
      //other buildings
      int picOffset = __des2index( desirability );
      Picture& pic = Picture::load( ResourceGroup::land2a, 37 + picOffset );

      city::Helper helper( _city() );
      TilesArray tiles4clear = helper.getArea( overlay );

      foreach( tile, tiles4clear )
      {
        engine.draw( pic, (*tile)->mappos() + offset );
      }
    }
  }

  if( desirability != 0 )
  {
    Picture* tx = _d->debugFont.once( utils::format( 0xff, "%d", desirability) );
    _d->debugText.push_back( tx );

    _addPicture( screenPos + Point( 20, -15 ), *tx );
  }

  tile.setWasDrawn();
}

void Desirability::beforeRender( Engine& engine )
{
  foreach( it, _d->debugText ) { Picture::destroy( *it ); }
  _d->debugText.clear();

  Info::beforeRender( engine );
}

void Desirability::handleEvent(NEvent& event)
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
        int desirability = tile->param( Tile::pDesirability );

        if( desirability < -25 ) { text = "##no_citizens_desire_live_here##"; }
        else if( desirability >= 0 && desirability < 10 ) { text = "##desirability_indiffirent_area##"; }
        else if( desirability >= 10 && desirability < 20 ) { text = "##desirability_pretty_area##"; }
      }

      _setTooltipText( _(text) );
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

LayerPtr Desirability::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Desirability( camera, city ) );
  ret->drop();

  return ret;
}

Desirability::Desirability( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 0 ), _d( new Impl )
{
  _d->debugFont = Font::create( "FONT_1" );
  _fillVisibleObjects( type() );
}

}

}//end namespace gfx
