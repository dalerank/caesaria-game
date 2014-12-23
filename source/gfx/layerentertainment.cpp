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
//
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "layerentertainment.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "core/event.hpp"
#include "tilemap_camera.hpp"
#include "city/helper.hpp"
#include "core/gettext.hpp"
#include "core/utils.hpp"

using namespace constants;

namespace gfx
{

namespace layer
{

int Entertainment::type() const {  return _type; }

int Entertainment::_getLevelValue( HousePtr house )
{
  switch( _type )
  {
  case citylayer::entertainment:
  {
    int entLevel = house->spec().computeEntertainmentLevel( house );
    int minLevel = house->spec().minEntertainmentLevel();
    return math::percentage( entLevel, minLevel );
  }
  case citylayer::theater: return (int) house->getServiceValue( Service::theater );
  case citylayer::amphitheater: return (int) house->getServiceValue( Service::amphitheater );
  case citylayer::colloseum: return (int) house->getServiceValue( Service::colloseum );
  case citylayer::hippodrome: return (int) house->getServiceValue( Service::hippodrome );
  }

  return 0;
}

void Entertainment::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  Point screenPos = tile.mappos() + offset;

  if( tile.overlay().isNull() )
  {
    //draw background
    engine.draw( tile.picture(), screenPos );
  }
  else
  {
    bool needDrawAnimations = false;
    TileOverlayPtr overlay = tile.overlay();

    int entertainmentLevel = -1;
    switch( overlay->type() )
    {
    // Base set of visible objects
    case objects::road:
    case objects::plaza:
    case objects::garden:

    case objects::burnedRuins:
    case objects::collapsedRuins:

    case objects::lowBridge:
    case objects::highBridge:

    case objects::elevation:
    case objects::rift:
      needDrawAnimations = true;
    break;

    case objects::theater:
    case objects::amphitheater:
    case objects::colloseum:
    case objects::hippodrome:
    case objects::lionsNursery:
    case objects::actorColony:
    case objects::gladiatorSchool:
      needDrawAnimations = _flags.count( overlay->type() ) > 0;
      if( !needDrawAnimations )
      {
        city::Helper helper( _city() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;

      //houses
    case objects::house:
      {
        HousePtr house = ptr_cast<House>( overlay );
        entertainmentLevel = _getLevelValue( house );

        needDrawAnimations = (house->spec().level() == 1) && (house->habitants().empty());
        city::Helper helper( _city() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    break;

      //other buildings
    default:
      {
        city::Helper helper( _city() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( entertainmentLevel > 0 )
    {
      _addColumn( screenPos, entertainmentLevel );
    }
  }

  tile.setWasDrawn();
}

LayerPtr Entertainment::create(TilemapCamera& camera, PlayerCityPtr city, int type )
{
  LayerPtr ret( new Entertainment( camera, city, type ) );
  ret->drop();

  return ret;
}

void Entertainment::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      std::string text = "";
      if( tile != 0 )
      {
        HousePtr house = ptr_cast<House>( tile->overlay() );
        if( house != 0 )
        {
          std::string typeName;
          switch( _type )
          {
          case citylayer::entertainment: typeName = "entertainment"; break;
          case citylayer::theater: typeName = "theater"; break;
          case citylayer::amphitheater: typeName = "amphitheater"; break;
          case citylayer::colloseum: typeName = "colloseum"; break;
          case citylayer::hippodrome: typeName = "hippodrome"; break;
          }

          int lvlValue = _getLevelValue( house );
          if( _type == citylayer::entertainment )
          {
            text = utils::format( 0xff, "##%d_entertainment_access##", lvlValue / 10 );
          }
          else
          {
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
            else
            {
              text = levelName + "_no_access";
            }
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

Entertainment::Entertainment( Camera& camera, PlayerCityPtr city, int type )
  : Info( camera, city, 9 )
{
  _type = type;

  switch( type )
  {
  case citylayer::entertainment:
    _flags.insert( objects::unknown ); _flags.insert( objects::theater );
    _flags.insert( objects::amphitheater ); _flags.insert( objects::colloseum );
    _flags.insert( objects::hippodrome ); _flags.insert( objects::actorColony );
    _flags.insert( objects::gladiatorSchool ); _flags.insert( objects::lionsNursery );
    _flags.insert( objects::chariotSchool );

    _addWalkerType( walker::actor );
    _addWalkerType( walker::gladiator );
    _addWalkerType( walker::lionTamer );
    _addWalkerType( walker::charioteer );
  break;

  case citylayer::theater:
    _flags.insert( objects::theater );
    _flags.insert( objects::actorColony );

    _addWalkerType( walker::actor );
  break;

  case citylayer::amphitheater:
    _flags.insert( objects::amphitheater );
    _flags.insert( objects::actorColony );
    _flags.insert( objects::gladiatorSchool );

    _addWalkerType( walker::actor );
    _addWalkerType( walker::gladiator );
  break;

  case citylayer::colloseum:
    _flags.insert( objects::colloseum );
    _flags.insert( objects::gladiatorSchool );
    _flags.insert( objects::lionsNursery );

    _addWalkerType( walker::gladiator );
    _addWalkerType( walker::lionTamer );
  break;


  case citylayer::hippodrome:
    _flags.insert( objects::hippodrome );
    _flags.insert( objects::chariotSchool );

    _addWalkerType( walker::charioteer );
  break;

  default: break;
  }
}
}
}//end namespace gfx
