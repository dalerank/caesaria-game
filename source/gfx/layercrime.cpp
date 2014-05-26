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

#include "layercrime.hpp"
#include "tileoverlay.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "layerconstants.hpp"

using namespace constants;

namespace gfx
{

int LayerCrime::type() const
{
  return citylayer::crime;
}

Layer::VisibleWalkers LayerCrime::getVisibleWalkers() const
{
  VisibleWalkers ret;
  ret.insert( walker::prefect );
  return ret;
}

void LayerCrime::drawTile( Engine& engine, Tile& tile, Point offset)
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
    int fireLevel = -1;
    switch( overlay->type() )
    {
    //fire buildings and roads
    case construction::road:
    case construction::plaza:
    case building::prefecture:
      needDrawAnimations = true;
    break;

      //houses
    case building::house:
      {
        HousePtr house = ptr_cast<House>( overlay );
        fireLevel = (int)house->getServiceValue( Service::crime );
        needDrawAnimations = (house->spec().level() == 1) && house->habitants().empty();

        city::Helper helper( _city() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase  );
      }
    break;

      //other buildings
    default:
      {
        city::Helper helper( _city() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base  );
      }
    break;
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( fireLevel >= 0)
    {
      drawColumn( engine, screenPos, fireLevel );
    }
  }

  tile.setWasDrawn();
}

LayerPtr LayerCrime::create(Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerCrime( camera, city ) );
  ret->drop();

  return ret;
}

LayerCrime::LayerCrime( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city )
{
  _loadColumnPicture( 18 );
}

}//end namespace gfx
