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

#include "layereducation.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "tilemap_camera.hpp"
#include "city/helper.hpp"
#include "core/event.hpp"
#include "core/gettext.hpp"

using namespace constants;

int LayerEducation::getType() const
{
  return _type;
}

Layer::VisibleWalkers LayerEducation::getVisibleWalkers() const
{
  return _walkers;
}

int LayerEducation::_getLevelValue( HousePtr house )
{
  switch(_type)
  {
  case citylayer::education:
  {
    switch( house->getSpec().getMinEducationLevel() )
    {
    case 1: return house->getServiceValue( Service::school );
    case 2: return ( house->getServiceValue( Service::school ) +
                      house->getServiceValue( Service::library ) ) / 2;
    case 3: return ( house->getServiceValue( Service::school ) +
                     house->getServiceValue( Service::library ) +
                     house->getServiceValue( Service::academy ) ) / 3;

    default: return 0;
    }
  }
  break;

  case citylayer::school: return house->getServiceValue( Service::school );
  case citylayer::library: return house->getServiceValue( Service::library );
  case citylayer::academy: return house->getServiceValue( Service::academy );
  }

  return 0;
}

void LayerEducation::drawTile(GfxEngine& engine, Tile& tile, Point offset)
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

    int educationLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::plaza:
      needDrawAnimations = true;
      engine.drawPicture( tile.getPicture(), screenPos );
    break;

    case building::school:
    case building::library:
    case building::academy:
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

        educationLevel = _getLevelValue( house );

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
    else if( educationLevel > 0 )
    {
      drawColumn( engine, screenPos, educationLevel );
    }
  }
}

LayerPtr LayerEducation::create(TilemapCamera& camera, PlayerCityPtr city, int type )
{
  LayerPtr ret( new LayerEducation( camera, city, type ) );
  ret->drop();

  return ret;
}

void LayerEducation::handleEvent(NEvent& event)
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
        if( house != 0 )
        {
          std::string typeName;
          switch( _type )
          {
          case citylayer::education: typeName = "education"; break;
          case citylayer::school: typeName = "school"; break;
          case citylayer::library: typeName = "library"; break;
          case citylayer::academy: typeName = "academy"; break;
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

LayerEducation::LayerEducation( TilemapCamera& camera, PlayerCityPtr city, int type)
  : Layer( camera, city )
{
  _loadColumnPicture( 9 );
  _type = type;

  switch( type )
  {
  case citylayer::education:
  case citylayer::school: _flags.insert( building::school ); _walkers.insert( walker::scholar ); break;
  case citylayer::library: _flags.insert( building::library ); _walkers.insert( walker::librarian ); break;
  case citylayer::academy: _flags.insert( building::academy ); _walkers.insert( walker::teacher ); break;
  }
}
