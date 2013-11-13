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

#include "layerhealth.hpp"
#include "building/constants.hpp"
#include "game/resourcegroup.hpp"
#include "building/house.hpp"
#include "game/house_level.hpp"
#include "layerconstants.hpp"

using namespace constants;

int LayerHealth::getType() const
{
  return _type;
}

Layer::VisibleWalkers LayerHealth::getVisibleWalkers() const
{
  return _walkers;
}

void LayerHealth::drawTile(GfxEngine& engine, Tile& tile, Point offset)
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

    int healthLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::B_PLAZA:
      needDrawAnimations = true;
      engine.drawPicture( tile.getPicture(), screenPos );
    break;

    case building::B_DOCTOR:
    case building::B_HOSPITAL:
    case building::B_BARBER:
    case building::B_BATHS:
      needDrawAnimations = _flags.count( overlay->getType() );
      if( needDrawAnimations )
      {
        engine.drawPicture( tile.getPicture(), screenPos );
      }
      else
      {
        CityHelper helper( _city );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;

      //houses
    case building::house:
      {
        HousePtr house = overlay.as< House >();

        if( _flags.count( building::B_DOCTOR ) ) { healthLevel = house->getHealthLevel(); }
        else if( _flags.count( building::B_HOSPITAL ) ) { healthLevel = house->getServiceValue( Service::hospital ); }
        else if( _flags.count( building::B_BARBER ) ) { healthLevel = house->getServiceValue( Service::barber ); }
        else if( _flags.count( building::B_BATHS ) ) { healthLevel = house->getServiceValue( Service::baths ); }

        needDrawAnimations = (house->getSpec().getLevel() == 1) && (house->getHabitants().size() == 0);

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
    else if( healthLevel > 0 )
    {
      drawColumn( engine, screenPos, 9, healthLevel );
    }
  }
}

LayerPtr LayerHealth::create(CityRenderer* renderer, PlayerCityPtr city, int type )
{
  LayerHealth* l = new LayerHealth();
  l->_renderer = renderer;
  l->_city = city;
  l->_type = type;

  switch( type )
  {
  case citylayer::health:
  case citylayer::doctor:
    l->_flags.insert( building::B_DOCTOR ); l->_walkers.insert( walker::doctor );
  break;

  case citylayer::hospital:
    l->_flags.insert( building::B_HOSPITAL ); l->_walkers.insert( walker::surgeon );
  break;

  case citylayer::barber:
    l->_flags.insert( building::B_BARBER ); l->_walkers.insert( walker::barber );
  break;

  case citylayer::baths:
    l->_flags.insert( building::B_BATHS ); l->_walkers.insert( walker::bathlady );
  break;
  }

  LayerPtr ret( l );
  ret->drop();

  return ret;
}
