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

#include "layerreligion.hpp"
#include "building/constants.hpp"
#include "game/resourcegroup.hpp"
#include "building/house.hpp"
#include "game/house_level.hpp"
#include "layerconstants.hpp"

using namespace constants;

int LayerReligion::getType() const
{
  return citylayer::religion;
}

Layer::VisibleWalkers LayerReligion::getVisibleWalkers() const
{
  VisibleWalkers ret;
  ret.insert( walker::priest );

  return ret;
}

void LayerReligion::drawTile(GfxEngine& engine, Tile& tile, Point offset)
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

    int religionLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::B_PLAZA:
    case building::templeCeres: case building::templeMars:
    case building::templeMercury: case building::templeNeptune: case building::templeVenus:
    case building::oracle:
    case building::B_BIG_TEMPLE_CERES: case building::B_BIG_TEMPLE_MARS:
    case building::B_BIG_TEMPLE_MERCURE: case building::B_BIG_TEMPLE_NEPTUNE: case building::B_BIG_TEMPLE_VENUS:
      needDrawAnimations = true;
      engine.drawPicture( tile.getPicture(), screenPos );
    break;

      //houses
    case building::house:
      {
        HousePtr house = overlay.as< House >();
        religionLevel = house->getServiceValue(Service::religionMercury);
        religionLevel += house->getServiceValue(Service::religionVenus);
        religionLevel += house->getServiceValue(Service::religionMars);
        religionLevel += house->getServiceValue(Service::religionNeptune);
        religionLevel += house->getServiceValue(Service::religionCeres);
        religionLevel = math::clamp( religionLevel / (house->getSpec().getMinReligionLevel()+1), 0, 100 );
        needDrawAnimations = (house->getSpec().getLevel() == 1) && (house->getHabitants().size() ==0);

        CityHelper helper( _city );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    break;

      //other buildings
    default:
      {
        CityHelper helper( _city );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;
    }

    if( needDrawAnimations )
    {
      _renderer->registerTileForRendering( tile );
    }
    else if( religionLevel > 0 )
    {
      drawColumn( engine, screenPos, 9, religionLevel );
    }
  }
}

LayerPtr LayerReligion::create(CityRenderer* renderer, CityPtr city)
{
  LayerReligion* l = new LayerReligion();
  l->_renderer = renderer;
  l->_city = city;

  LayerPtr ret( l );
  ret->drop();

  return ret;
}
