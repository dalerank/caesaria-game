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

#include "layerentertainment.hpp"
#include "building/constants.hpp"
#include "building/house.hpp"
#include "game/resourcegroup.hpp"
#include "game/house_level.hpp"
#include "layerconstants.hpp"

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
    case construction::B_PLAZA:
      needDrawAnimations = true;
      engine.drawPicture( tile.getPicture(), screenPos );
    break;

    case building::theater:
    case building::amphitheater:
    case building::colloseum:
    case building::hippodrome:
    case building::lionHouse:
    case building::actorColony:
    case building::gladiatorSchool:
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
        if( _flags.count( building::unknown ) ) { entertainmentLevel = house->getSpec().computeEntertainmentLevel( house ); }
        else if( _flags.count( building::theater ) ) { entertainmentLevel = house->getServiceValue( Service::theater ); }
        else if( _flags.count( building::amphitheater ) ) { entertainmentLevel = house->getServiceValue( Service::amphitheater ); }
        else if( _flags.count( building::colloseum ) ) { entertainmentLevel = house->getServiceValue( Service::colloseum ); }
        else if( _flags.count( building::hippodrome ) ) { entertainmentLevel = house->getServiceValue( Service::hippodrome ); }

        needDrawAnimations = (house->getSpec().getLevel() == 1) && (house->getHabitants().empty());
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
    else if( entertainmentLevel > 0 )
    {
      drawColumn( engine, screenPos, 9, entertainmentLevel );
    }
  }
}

LayerPtr LayerEntertainment::create(CityRenderer* renderer, PlayerCityPtr city, int type )
{
  LayerEntertainment* l = new LayerEntertainment();
  l->_renderer = renderer;
  l->_city = city;
  l->_type = type;

  switch( type )
  {
  case citylayer::entertainmentAll:
    l->_flags.insert( building::unknown ); l->_flags.insert( building::theater );
    l->_flags.insert( building::amphitheater ); l->_flags.insert( building::colloseum );
    l->_flags.insert( building::hippodrome ); l->_flags.insert( building::actorColony );
    l->_flags.insert( building::gladiatorSchool ); l->_flags.insert( building::lionHouse );
    l->_flags.insert( building::chariotSchool );

    l->_visibleWalkers.insert( walker::actor );
    l->_visibleWalkers.insert( walker::gladiator );
    l->_visibleWalkers.insert( walker::tamer );
    l->_visibleWalkers.insert( walker::charioter );
  break;

  case citylayer::theater:
    l->_flags.insert( building::theater );
    l->_flags.insert( building::actorColony );

    l->_visibleWalkers.insert( walker::actor );
  break;

  case citylayer::amphitheater:
    l->_flags.insert( building::amphitheater );
    l->_flags.insert( building::actorColony );
    l->_flags.insert( building::gladiatorSchool );

    l->_visibleWalkers.insert( walker::actor );
    l->_visibleWalkers.insert( walker::gladiator );
  break;

  case citylayer::colloseum:
    l->_flags.insert( building::colloseum );
    l->_flags.insert( building::gladiatorSchool );
    l->_flags.insert( building::lionHouse );

    l->_visibleWalkers.insert( walker::gladiator );
    l->_visibleWalkers.insert( walker::tamer );
  break;


  case citylayer::hippodrome:
    l->_flags.insert( building::hippodrome );
    l->_flags.insert( building::chariotSchool );

    l->_visibleWalkers.insert( walker::charioter );
  break;

  default: break;
  }

  LayerPtr ret( l );
  ret->drop();

  return ret;
}
