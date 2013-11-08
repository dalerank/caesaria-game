// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "layerfood.hpp"
#include "tileoverlay.hpp"
#include "building/constants.hpp"
#include "building/house.hpp"
#include "game/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "game/city.hpp"
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
  ret.insert( walker::marketLadyHelper );

  return ret;
}

void LayerFood::drawTile(GfxEngine& engine, Tile& tile, Point offset)
{
  Point screenPos = tile.getXY() + offset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.getOverlay().isNull() )
  {
    //draw background
    engine.drawPicture( tile.getPicture(), screenPos );
  }
  else
  {
    TileOverlayPtr overlay = tile.getOverlay();
    Picture pic;
    int foodLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::B_PLAZA:
    case building::B_MARKET:
    case building::granary:
      pic = tile.getPicture();
      needDrawAnimations = true;
    break;

      //houses
    case building::house:
      {
        CityHelper helper( _city );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
        HousePtr house = overlay.as< House >();
        foodLevel = house->getFoodLevel();
        needDrawAnimations = (house->getSpec().getLevel() == 1) && (house->getHabitants().size() == 0);
      }
      break;

      //other buildings
    default:
      {
        CityHelper helper( _city );
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
      _renderer->registerTileForRendering( tile );
    }
    else if( foodLevel >= 0 )
    {
      drawColumn( engine, screenPos, 18, math::clamp( 100 - foodLevel, 0, 100 ) );
    }
  }
}

LayerPtr LayerFood::create(CityRenderer* renderer, CityPtr city)
{
  LayerFood* l = new LayerFood();
  l->_renderer = renderer;
  l->_city = city;

  LayerPtr ret( l );
  ret->drop();

  return ret;
}
