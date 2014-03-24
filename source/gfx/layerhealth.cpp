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
#include "tilemap_camera.hpp"
#include "city/helper.hpp"
#include "core/gettext.hpp"
#include "core/event.hpp"

using namespace constants;

int LayerHealth::getType() const
{
  return _type;
}

Layer::VisibleWalkers LayerHealth::getVisibleWalkers() const
{
  return _walkers;
}

int LayerHealth::_getLevelValue( HousePtr house )
{
  switch(_type)
  {
  case citylayer::health: return house->getState( (Construction::Param)House::health );
  case citylayer::hospital: return house->getServiceValue( Service::hospital );
  case citylayer::barber: return house->getServiceValue( Service::barber );
  case citylayer::baths: return house->getServiceValue( Service::baths );
  }

  return 0;
}

void LayerHealth::drawTile(GfxEngine& engine, Tile& tile, Point offset)
{
  Point screenPos = tile.mapPos() + offset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.overlay().isNull() )
  {
    //draw background
    engine.drawPicture( tile.picture(), screenPos );
  }
  else
  {
    TileOverlayPtr overlay = tile.overlay();

    int healthLevel = -1;
    switch( overlay->type() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::plaza:
      needDrawAnimations = true;
      engine.drawPicture( tile.picture(), screenPos );
    break;

    case building::doctor:
    case building::hospital:
    case building::barber:
    case building::baths:
      needDrawAnimations = _flags.count( overlay->type() );
      if( needDrawAnimations )
      {
        engine.drawPicture( tile.picture(), screenPos );
      }
      else
      {
        city::Helper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;

      //houses
    case building::house:
      {
        HousePtr house = ptr_cast<House>( overlay );

        healthLevel = _getLevelValue( house );

        needDrawAnimations = (house->getSpec().level() == 1) && (house->getHabitants().empty());

        city::Helper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    break;

      //other buildings
    default:
      {
        city::Helper helper( _getCity() );
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

void LayerHealth::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      Tile* tile = _getCamera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      std::string text = "";
      if( tile != 0 )
      {
        HousePtr house = ptr_cast<House>( tile->overlay() );
        if( house != 0 )
        {
          std::string typeName;
          switch( _type )
          {
          case citylayer::health: typeName = "health"; break;
          case citylayer::doctor: typeName = "doctor"; break;
          case citylayer::hospital: typeName = "hospital"; break;
          case citylayer::barber: typeName = "barber"; break;
          case citylayer::baths: typeName = "baths"; break;
          }

          int lvlValue = _getLevelValue( house );
          std::string levelName;
          if( lvlValue > 0 )
          {
            if( lvlValue < 20 ) { levelName = "##warning_"; }
            else if( lvlValue < 40 ) { levelName = "##bad_"; }
            else if( lvlValue < 60 ) { levelName = "##simple_"; }
            else if( lvlValue < 80 ) { levelName = "##good_"; }
            else { levelName = "##awesome_"; }

            text = levelName + typeName + "_access##";
          }
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
