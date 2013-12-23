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

#include "layerdamage.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"

using namespace constants;

int LayerDamage::getType() const
{
  return citylayer::damage;
}

std::set<int> LayerDamage::getVisibleWalkers() const
{
  std::set<int> ret;
  ret.insert( walker::engineer );
  return ret;
}

void LayerDamage::drawTile(GfxEngine& engine, Tile& tile, Point offset)
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
    int damageLevel = 0;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::plaza:
    case building::collapsedRuins:
    case building::engineerPost:
      needDrawAnimations = true;
      engine.drawPicture( tile.getPicture(), screenPos );
      break;

      //houses
    case building::house:
      {
        HousePtr house = overlay.as< House >();
        damageLevel = (int)house->getState( Construction::damage );
        needDrawAnimations = (house->getSpec().getLevel() == 1) && house->getHabitants().empty();

        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
      break;

      //other buildings
    default:
      {
        BuildingPtr building = overlay.as< Building >();
        if( building.isValid() )
        {
          damageLevel = (int)building->getState( Construction::damage );
        }

        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
      break;
    }

    if( needDrawAnimations )
    {
      registerTileForRendering( tile );
    }
    else if( damageLevel >= 0 )
    {
      drawColumn( engine, screenPos, 15, damageLevel );
    }
  }
}

LayerPtr LayerDamage::create(TilemapCamera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerDamage( camera, city ) );
  ret->drop();

  return ret;
}

LayerDamage::LayerDamage( TilemapCamera& camera, PlayerCityPtr city)
  : Layer( camera, city )
{

}
