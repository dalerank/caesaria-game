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

#include "layerhealth.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"

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
    case construction::plaza:
      needDrawAnimations = true;
      engine.drawPicture( tile.getPicture(), screenPos );
    break;

    case building::doctor:
    case building::hospital:
    case building::barber:
    case building::baths:
      needDrawAnimations = _flags.count( overlay->getType() );
      if( needDrawAnimations )
      {
        engine.drawPicture( tile.getPicture(), screenPos );
      }
      else
      {
        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;

      //houses
    case building::house:
      {
        HousePtr house = ptr_cast<House>( overlay );

        if( _flags.count( building::doctor ) ) { healthLevel = house->getHealthLevel(); }
        else if( _flags.count( building::hospital ) ) { healthLevel = house->getServiceValue( Service::hospital ); }
        else if( _flags.count( building::barber ) ) { healthLevel = house->getServiceValue( Service::barber ); }
        else if( _flags.count( building::baths ) ) { healthLevel = house->getServiceValue( Service::baths ); }

        needDrawAnimations = (house->getSpec().getLevel() == 1) && (house->getHabitants().empty());

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
    else if( healthLevel > 0 )
    {
      drawColumn( engine, screenPos, healthLevel );
    }
  }
}

LayerPtr LayerHealth::create(TilemapCamera& camera, PlayerCityPtr city, int type )
{
  LayerPtr ret( new LayerHealth( camera, city, type ) );
  ret->drop();

  return ret;
}

LayerHealth::LayerHealth( TilemapCamera& camera, PlayerCityPtr city, int type)
  : Layer( camera, city )
{
  _loadColumnPicture( 9 );
  _type = type;

  switch( type )
  {
  case citylayer::health:
  case citylayer::doctor:
    _flags.insert( building::doctor ); _walkers.insert( walker::doctor );
  break;

  case citylayer::hospital:
    _flags.insert( building::hospital ); _walkers.insert( walker::surgeon );
  break;

  case citylayer::barber:
    _flags.insert( building::barber ); _walkers.insert( walker::barber );
  break;

  case citylayer::baths:
    _flags.insert( building::baths ); _walkers.insert( walker::bathlady );
  break;
  }
}
