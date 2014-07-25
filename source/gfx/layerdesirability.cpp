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

#include "layerdesirability.hpp"
#include "game/resourcegroup.hpp"
#include "objects/constants.hpp"
#include "city/helper.hpp"
#include "core/stringhelper.hpp"
#include "layerconstants.hpp"

using namespace constants;

namespace gfx
{

int LayerDesirability::type() const
{
  return citylayer::desirability;
}

std::set<int> LayerDesirability::visibleWalkers() const
{
  return std::set<int>();
}

void LayerDesirability::drawTile( Engine& engine, Tile& tile, Point offset)
{
  //Tilemap& tilemap = _city->getTilemap();
  Point screenPos = tile.mappos() + offset;

  int desirability = tile.param( Tile::pDesirability );
  if( tile.overlay().isNull() )
  {
    //draw background
    if( tile.getFlag( Tile::isConstructible ) && desirability != 0 )
    {
      int picOffset = desirability < 0
                          ? math::clamp( desirability / 25, -3, 0 )
                          : math::clamp( desirability / 15, 0, 6 );
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

      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    break;

    //other buildings
    default:
      {
        int picOffset = desirability < 0
                          ? math::clamp( desirability / 25, -3, 0 )
                          : math::clamp( desirability / 15, 0, 6 );
        Picture& pic = Picture::load( ResourceGroup::land2a, 37 + picOffset );

        city::Helper helper( _city() );
        TilesArray tiles4clear = helper.getArea( overlay );

        foreach( tile, tiles4clear )
        {
          engine.draw( pic, (*tile)->mappos() + offset );
        }
      }
    break;
    }
  }

  if( desirability != 0 )
  {
    _debugFont.draw( engine.screen(), StringHelper::format( 0xff, "%d", desirability), screenPos + Point( 20, -15 ), false );
  }

  tile.setWasDrawn();
}

LayerPtr LayerDesirability::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerDesirability( camera, city ) );
  ret->drop();

  return ret;
}

LayerDesirability::LayerDesirability( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city )
{
  _debugFont = Font::create( "FONT_1" );
}

}//end namespace gfx
