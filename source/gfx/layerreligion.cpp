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

#include "layerreligion.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"
#include "core/stringhelper.hpp"

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
  Point screenPos = tile.mapPos() + offset;

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
    case construction::plaza:
    case building::templeCeres: case building::templeMars:
    case building::templeMercury: case building::templeNeptune: case building::templeVenus:
    case building::oracle:
    case building::cathedralCeres: case building::cathedralMars:
    case building::cathedralMercury: case building::cathedralNeptune: case building::cathedralVenus:
      needDrawAnimations = true;
      engine.drawPicture( tile.getPicture(), screenPos );
    break;

      //houses
    case building::house:
      {
        HousePtr house = ptr_cast<House>( overlay );
        religionLevel = house->getServiceValue(Service::religionMercury);
        religionLevel += house->getServiceValue(Service::religionVenus);
        religionLevel += house->getServiceValue(Service::religionMars);
        religionLevel += house->getServiceValue(Service::religionNeptune);
        religionLevel += house->getServiceValue(Service::religionCeres);
        religionLevel = math::clamp( religionLevel / (house->getSpec().getMinReligionLevel()+1), 0, 100 );
        needDrawAnimations = (house->getSpec().getLevel() == 1) && house->getHabitants().empty();

        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    break;

      //other buildings
    default:
      {
        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;
    }

    if( needDrawAnimations )
    {
      registerTileForRendering( tile );
    }
    else if( religionLevel > 0 )
    {
      drawColumn( engine, screenPos, religionLevel );
    }
  }
}

void LayerReligion::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      Tile* tile = _getCamera()->at( event.mouse.getPosition(), false );  // tile under the cursor (or NULL)
      std::string text = "";
      if( tile != 0 )
      {
        HousePtr house = ptr_cast<House>( tile->getOverlay() );
        if( house.isValid() )
        {
          int templeAccess = house->getSpec().computeReligionLevel( house );
          bool oracleAccess = house->hasServiceAccess( Service::oracle );

          text = (templeAccess == 5 && oracleAccess )
                  ? "##religion_access_full##"
                  : StringHelper::format( 0xff, "##religion_access_%d_temple##", templeAccess );
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

LayerPtr LayerReligion::create(TilemapCamera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerReligion( camera, city ) );
  ret->drop();

  return ret;
}

LayerReligion::LayerReligion(TilemapCamera& camera, PlayerCityPtr city)
  : Layer( camera, city )
{
  _loadColumnPicture( 9 );
}
