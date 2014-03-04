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

#include "layerfood.hpp"
#include "tileoverlay.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "layerconstants.hpp"

using namespace constants;

int LayerFood::getType() const
{
  return citylayer::food;
}

Layer::VisibleWalkers LayerFood::getVisibleWalkers() const
{
  VisibleWalkers ret;
  ret.insert( walker::marketLady );
  ret.insert( walker::marketKid );

  return ret;
}

void LayerFood::drawTile(GfxEngine& engine, Tile& tile, Point offset)
{
  Point screenPos = tile.mapPos() + offset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.overlay().isNull() )
  {
    //draw background
    engine.drawPicture( tile.getPicture(), screenPos );
  }
  else
  {
    TileOverlayPtr overlay = tile.overlay();
    Picture pic;
    int foodLevel = -1;
    switch( overlay->type() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::plaza:
    case building::market:
    case building::granary:
      pic = tile.getPicture();
      needDrawAnimations = true;
    break;

      //houses
    case building::house:
      {
        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
        HousePtr house = ptr_cast<House>( overlay );
        foodLevel = house->getState( (Construction::Param)House::food );
        needDrawAnimations = (house->getSpec().getLevel() == 1) && (house->getHabitants().empty());
      }
      break;

      //other buildings
    default:
      {
        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base);
      }
      break;
    }

    if ( pic.isValid())
    {
      engine.drawPicture( pic, screenPos );
    }

    if( needDrawAnimations )
    {
      registerTileForRendering( tile );
    }
    else if( foodLevel >= 0 )
    {
      drawColumn( engine, screenPos, math::clamp( 100 - foodLevel, 0, 100 ) );
    }
  }
}

LayerPtr LayerFood::create(TilemapCamera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerFood( camera, city ) );
  ret->drop();

  return ret;
}

LayerFood::LayerFood(TilemapCamera& camera, PlayerCityPtr city)
  : Layer( camera, city )
{
  _loadColumnPicture( 18 );
}
