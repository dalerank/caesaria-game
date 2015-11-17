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
#include "objects/house_spec.hpp"
#include "constants.hpp"
#include "core/event.hpp"
#include "gfx/tilemap_camera.hpp"
#include "city/statistic.hpp"
#include "core/priorities.hpp"
#include "core/gettext.hpp"
#include "game/gamedate.hpp"
#include "core/utils.hpp"
#include "core/color_list.hpp"
#include "core/logger.hpp"
#include "objects/entertainment.hpp"
#include "gfx/textured_path.hpp"

using namespace gfx;

namespace citylayer
{

class Entertainment::Impl
{
public:
  struct
  {
    OverlayPtr selected;
    OverlayPtr underMouse;
  } overlay;

  DateTime lastUpdate;
  std::vector<ColoredWay> ways;
  std::set<object::Type> flags;
  int type;
};

int Entertainment::type() const {  return _d->type; }

int Entertainment::_getLevelValue( HousePtr house )
{
  switch( _d->type )
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

void Entertainment::drawTile(const RenderInfo& rinfo, Tile& tile)
{
  if( tile.overlay().isNull() )
  {
    drawLandTile( rinfo, tile );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();

    int entertainmentLevel = -1;
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;
    }
    else if( _d->flags.count( overlay->type() ) > 0 )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();
      entertainmentLevel = _getLevelValue( house );

      needDrawAnimations = (house->level() <= HouseLevel::hovel) && (house->habitants().empty());
      drawArea( rinfo, overlay->area(), ResourceGroup::foodOverlay, config::id.overlay.inHouseBase );
    }
    else
    {
      drawArea( rinfo, overlay->area(), ResourceGroup::foodOverlay, config::id.overlay.base );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else if( entertainmentLevel > 0 )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, entertainmentLevel );
    }
  }

  tile.setRendered();
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
        HousePtr house = tile->overlay<House>();
        if( house != 0 )
        {
          std::string typeName;
          switch( _d->type )
          {
          case citylayer::entertainment:  typeName = "entertainment";  break;
          case citylayer::theater:        typeName = "theater";        break;
          case citylayer::amphitheater:   typeName = "amphitheater";   break;
          case citylayer::colloseum:      typeName = "colloseum";      break;
          case citylayer::hippodrome:     typeName = "hippodrome";     break;
          }

          int lvlValue = _getLevelValue( house );
          if( _d->type == citylayer::entertainment )
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

      _d->overlay.underMouse = tile->overlay();

      _setTooltipText( _(text) );
    }
    break;

    case mouseLbtnPressed:
    {
      if( _d->overlay.underMouse.is<EntertainmentBuilding>() )
      {
        _d->overlay.selected = _d->overlay.underMouse;
        _updatePaths();
      }
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

void Entertainment::render(Engine& engine)
{
  Info::render( engine );

  RenderInfo rinfo{ engine, _camera()->offset() };
  for( auto& item : _d->ways )
    TexturedPath::draw( item.tiles, rinfo, item.color, item.offset );
}

void Entertainment::afterRender(Engine& engine)
{
  Info::afterRender(engine);

  if( game::Date::isDayChanged() )
    _updatePaths();
}

void Entertainment::_updatePaths()
{
  auto entBuilding = _d->overlay.selected.as<EntertainmentBuilding>();
  if( entBuilding.isValid() && _d->flags.count( entBuilding->type() ) )
  {
    _d->ways.clear();
    const WalkerList& walkers = entBuilding->walkers();
    for( auto walker : walkers )
      _d->ways.push_back( ColoredWay{ walker->pathway().allTiles(), ColorList::red, Point( 0, 2 ) } );

    const EntertainmentBuilding::IncomeWays& incomes = entBuilding->incomes();
    for( const auto& way : incomes )
    {
      bool isOk = true;
      PathwayCondition condition;
      isOk &= condition.append( _map().overlay( way.base ) );
      isOk &= condition.append( _map().overlay( way.destination ) );

      if( isOk )
      {
        Pathway pathway = PathwayHelper::create( way.base, way.destination, condition.byRoads() );
        if( pathway.isValid() )
        {
          _d->ways.push_back( ColoredWay{ pathway.allTiles(), ColorList::blue, Point( 0, -2 ) } );
        }
        else
        {
          isOk = false;
        }
      }

      if( !isOk )
      {
        OverlayPtr b1 = _map().overlay( way.base );
        OverlayPtr b2 = _map().overlay( way.destination );
        Logger::warning( "EntertainmentLayer: cant create path from [{},{}]:{} to [{},{}]:{}",
                         way.base.i(),        way.base.j(),        b1.isValid() ? b1->info().name() : "unknown",
                         way.destination.i(), way.destination.j(), b2.isValid() ? b2->info().name() : "unknown" );
      }
    }
  }
}

Entertainment::Entertainment(Camera& camera, PlayerCityPtr city, Type type )
  : Info( camera, city, 9 ), _d( new Impl )
{
  _d->type = type;

  switch( _d->type )
  {
  case citylayer::entertainment:
    _d->flags << object::unknown << object::theater
              << object::amphitheater << object::colloseum
              << object::hippodrome << object::actorColony
              << object::gladiatorSchool << object::lionsNursery
              << object::chariotSchool;

    _visibleWalkers() << walker::actor << walker::gladiator
                      << walker::lionTamer << walker::charioteer;
  break;

  case citylayer::theater:
    _d->flags << object::theater << object::actorColony;
    _visibleWalkers() << walker::actor;
  break;

  case citylayer::amphitheater:
    _d->flags << object::amphitheater << object::actorColony << object::gladiatorSchool;
    _visibleWalkers() << walker::actor << walker::gladiator;
  break;

  case citylayer::colloseum:
    _d->flags << object::colloseum << object::gladiatorSchool << object::lionsNursery;
    _visibleWalkers() << walker::gladiator << walker::lionTamer;
  break;

  case citylayer::hippodrome:
    _d->flags << object::hippodrome << object::chariotSchool;
    _addWalkerType( walker::charioteer );
  break;

  default: break;
  }

  _initialize();
}

}//end namespace citylayer
