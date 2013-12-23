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

#include "layerentertainment.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"

using namespace constants;

int LayerEntertainment::getType() const
{
  return _type;
}

Layer::VisibleWalkers LayerEntertainment::getVisibleWalkers() const
{
  return _visibleWalkers;
}

void LayerEntertainment::drawTile(GfxEngine& engine, Tile& tile, Point offset)
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

    int entertainmentLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::plaza:
      needDrawAnimations = true;
      engine.drawPicture( tile.getPicture(), screenPos );
    break;

    case building::theater:
    case building::amphitheater:
    case building::colloseum:
    case building::hippodrome:
    case building::lionsNursery:
    case building::actorColony:
    case building::gladiatorSchool:
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
        HousePtr house = overlay.as< House >();
        if( _flags.count( building::unknown ) ) { entertainmentLevel = house->getSpec().computeEntertainmentLevel( house ); }
        else if( _flags.count( building::theater ) ) { entertainmentLevel = house->getServiceValue( Service::theater ); }
        else if( _flags.count( building::amphitheater ) ) { entertainmentLevel = house->getServiceValue( Service::amphitheater ); }
        else if( _flags.count( building::colloseum ) ) { entertainmentLevel = house->getServiceValue( Service::colloseum ); }
        else if( _flags.count( building::hippodrome ) ) { entertainmentLevel = house->getServiceValue( Service::hippodrome ); }

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
    else if( entertainmentLevel > 0 )
    {
      drawColumn( engine, screenPos, 9, entertainmentLevel );
    }
  }
}

LayerPtr LayerEntertainment::create(TilemapCamera& camera, PlayerCityPtr city, int type )
{
  LayerPtr ret( new LayerEntertainment( camera, city, type ) );
  ret->drop();

  return ret;
}

LayerEntertainment::LayerEntertainment(TilemapCamera& camera, PlayerCityPtr city, int type )
  : Layer( camera, city )
{
  _type = type;

  switch( type )
  {
  case citylayer::entertainmentAll:
    _flags.insert( building::unknown ); _flags.insert( building::theater );
    _flags.insert( building::amphitheater ); _flags.insert( building::colloseum );
    _flags.insert( building::hippodrome ); _flags.insert( building::actorColony );
    _flags.insert( building::gladiatorSchool ); _flags.insert( building::lionsNursery );
    _flags.insert( building::chariotSchool );

    _visibleWalkers.insert( walker::actor );
    _visibleWalkers.insert( walker::gladiator );
    _visibleWalkers.insert( walker::lionTamer );
    _visibleWalkers.insert( walker::charioter );
  break;

  case citylayer::theater:
    _flags.insert( building::theater );
    _flags.insert( building::actorColony );

    _visibleWalkers.insert( walker::actor );
  break;

  case citylayer::amphitheater:
    _flags.insert( building::amphitheater );
    _flags.insert( building::actorColony );
    _flags.insert( building::gladiatorSchool );

    _visibleWalkers.insert( walker::actor );
    _visibleWalkers.insert( walker::gladiator );
  break;

  case citylayer::colloseum:
    _flags.insert( building::colloseum );
    _flags.insert( building::gladiatorSchool );
    _flags.insert( building::lionsNursery );

    _visibleWalkers.insert( walker::gladiator );
    _visibleWalkers.insert( walker::lionTamer );
  break;


  case citylayer::hippodrome:
    _flags.insert( building::hippodrome );
    _flags.insert( building::chariotSchool );

    _visibleWalkers.insert( walker::charioter );
  break;

  default: break;
  }
}
