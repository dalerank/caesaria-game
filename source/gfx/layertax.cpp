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

#include "layertax.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"

using namespace constants;

int LayerTax::getType() const
{
  return citylayer::tax;
}

Layer::VisibleWalkers LayerTax::getVisibleWalkers() const
{
  VisibleWalkers ret;
  ret.insert( walker::taxCollector );

  return ret;
}

void LayerTax::drawTile(GfxEngine& engine, Tile& tile, Point offset)
{
  Point screenPos = tile.mapPos() + offset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.overlay().isNull() )
  {
    //draw background
    engine.drawPicture( tile.picture(), screenPos );
  }
  else
  {
    TileOverlayPtr overlay = tile.overlay();

    int taxLevel = -1;
    switch( overlay->type() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::plaza:
    case building::senate:
    case building::forum:
      needDrawAnimations = true;
      engine.drawPicture( tile.picture(), screenPos );
    break;

      //houses
    case building::house:
      {
        city::Helper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
        HousePtr house = ptr_cast<House>( overlay );
        taxLevel = house->getServiceValue( Service::forum );
        needDrawAnimations = (house->getSpec().level() == 1) && (house->getHabitants().empty());
      }
    break;

      //other buildings
    default:
      {
        city::Helper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;
    }

    if( needDrawAnimations )
    {
      registerTileForRendering( tile );
    }
    else if( taxLevel > 0 )
    {
      drawColumn( engine, screenPos, taxLevel );
    }
  }
}

LayerPtr LayerTax::create(TilemapCamera& camera, PlayerCityPtr city )
{
  LayerPtr ret( new LayerTax( camera, city ) );
  ret->drop();

  return ret;
}

LayerTax::LayerTax( TilemapCamera& camera, PlayerCityPtr city)
  : Layer( camera, city )
{
  _loadColumnPicture( 9 );
}
