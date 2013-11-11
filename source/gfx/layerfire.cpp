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

#include "layerfire.hpp"
#include "tileoverlay.hpp"
#include "building/constants.hpp"
#include "building/house.hpp"
#include "game/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "game/city.hpp"
#include "layerconstants.hpp"

using namespace constants;

int LayerFire::getType() const
{
  return citylayer::fire;
}

std::set<int> LayerFire::getVisibleWalkers() const
{
  std::set<int> ret;
  ret.insert( walker::prefect );
  return ret;
}

void LayerFire::drawTile(GfxEngine& engine, Tile& tile, Point offset)
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
    int fireLevel = 0;
    switch( overlay->getType() )
    {
    //fire buildings and roads
    case construction::road:
    case construction::B_PLAZA:
    case building::burningRuins:
    case building::B_BURNED_RUINS:
    case building::B_COLLAPSED_RUINS:
    case building::prefecture:
    case building::B_WELL:
    case building::B_FOUNTAIN:
      engine.drawPicture( tile.getPicture(), screenPos );
      needDrawAnimations = true;
    break;

      //houses
    case building::house:
      {
        HousePtr house = overlay.as< House >();
        fireLevel = (int)house->getState( Construction::fire );
        needDrawAnimations = (house->getSpec().getLevel() == 1) && (house->getHabitants().size() ==0);

        CityHelper helper( _city );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase  );
      }
    break;

      //other buildings
    default:
      {
        BuildingPtr building = overlay.as< Building >();
        if( building != 0 )
        {
          fireLevel = (int)building->getState( Construction::fire );
        }

        CityHelper helper( _city );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base  );
      }
    break;
    }

    if( needDrawAnimations )
    {
      _renderer->registerTileForRendering( tile );
    }
    else if( fireLevel >= 0)
    {
      drawColumn( engine, screenPos, 18, fireLevel );
    }
  }
}

LayerPtr LayerFire::create(CityRenderer* renderer, CityPtr city)
{
  LayerFire* l = new LayerFire();
  l->_renderer = renderer;
  l->_city = city;

  LayerPtr ret( l );
  ret->drop();

  return ret;
}
