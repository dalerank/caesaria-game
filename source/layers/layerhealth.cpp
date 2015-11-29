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
#include "objects/house_spec.hpp"
#include "constants.hpp"
#include "objects/health.hpp"
#include "gfx/tilemap_camera.hpp"
#include "city/statistic.hpp"
#include "core/priorities.hpp"
#include "gfx/textured_path.hpp"
#include "core/gettext.hpp"
#include "core/event.hpp"

using namespace gfx;

namespace citylayer
{

class Health::Impl
{
public:
  struct
  {
    OverlayPtr selected;
    OverlayPtr underMouse;
  } overlay;

  DateTime lastUpdate;
  std::vector<TilesArray> ways;

  object::TypeSet flags;
  int type;
};

int Health::type() const { return _d->type; }

int Health::_getLevelValue( HousePtr house )
{
  switch(_d->type)
  {
  case citylayer::health:   return (int) house->state( pr::health );
  case citylayer::doctor:   return (int) house->getServiceValue( Service::doctor );
  case citylayer::hospital: return (int) house->getServiceValue( Service::hospital );
  case citylayer::barber:   return (int) house->getServiceValue( Service::barber );
  case citylayer::baths:    return (int) house->getServiceValue( Service::baths );
  }

  return 0;
}

void Health::drawTile( const RenderInfo& rinfo, Tile& tile)
{
  if( tile.overlay().isNull() )
  {
    drawLandTile( rinfo, tile );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();

    int healthLevel = -1;
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;
    }
    else if( _d->flags.count( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();
      healthLevel = _getLevelValue( house );

      needDrawAnimations = (house->level() <= HouseLevel::hovel) && (house->habitants().empty());

      if( !needDrawAnimations )
      {
        drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.house );
      }
    }
    else  //other buildings
    {
      drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.constr );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else if( healthLevel > 0 )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, healthLevel );
    }
  }

  tile.setRendered();
}

void Health::_updatePaths()
{
  auto wbuilding = _d->overlay.selected.as<HealthBuilding>();
  if( wbuilding.isValid() )
  {
    _d->ways.clear();
    const WalkerList& walkers = wbuilding->walkers();
    for( auto walker : walkers )
      _d->ways.push_back( walker->pathway().allTiles() );
  }
}

void Health::handleEvent(NEvent& event)
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
        auto house = tile->overlay<House>();
        if( house != 0 )
        {
          std::string typeName;
          switch( _d->type )
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

        _d->overlay.underMouse = tile->overlay();
      }

      _setTooltipText( _(text) );
    }
    break;

    case mouseLbtnPressed:
    {
      if( _d->overlay.underMouse.is<HealthBuilding>() )
      {
        if( _d->flags.count( _d->overlay.underMouse->type() ) )
        {
          _d->overlay.selected = _d->overlay.underMouse;
          _updatePaths();
        }
      }
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

void Health::render(Engine& engine)
{
  Info::render( engine );

  RenderInfo rinfo{ engine, _camera()->offset() };
  for( auto& tiles : _d->ways )
    TexturedPath::draw( tiles, rinfo );
}

Health::Health(Camera& camera, PlayerCityPtr city, int type)
  : Info( camera, city, 9 ), _d( new Impl )
{
  _d->type = type;

  switch( type )
  {
  case citylayer::health:
    _d->flags << object::clinic << object::hospital
              << object::barber << object::baths;
    _visibleWalkers() << walker::doctor << walker::surgeon
                      << walker::barber << walker::bathlady;
  break;

  case citylayer::doctor:
    _d->flags << object::clinic;
    _visibleWalkers() << walker::doctor;
  break;

  case citylayer::hospital:
    _d->flags << object::hospital;
    _visibleWalkers() << walker::surgeon;
  break;

  case citylayer::barber:
    _d->flags << object::barber;
    _visibleWalkers() << walker::barber;
  break;

  case citylayer::baths:
    _d->flags << object::baths;
    _visibleWalkers() << walker::bathlady;
  break;
  }

  _initialize();
}

}//end namespace citylayer
