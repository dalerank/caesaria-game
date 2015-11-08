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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "market_access.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "game/resourcegroup.hpp"
#include "walker/market_buyer.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "objects/market.hpp"
#include "gfx/textured_path.hpp"
#include "core/event.hpp"
#include "core/color_list.hpp"
#include "gfx/tilemap_camera.hpp"
#include "game/gamedate.hpp"

using namespace gfx;

namespace citylayer
{

enum { maxAccessLevel=10, accessColumnIndex=15 };
static const char* marketLevelName[maxAccessLevel] = {
                                         "##none_market_access##", "##extremelow_market_access##",
                                         "##verylow_market_access##", "##low_market_access##",
                                         "##bad_market_access##",   "##simple_market_access##",
                                         "##good_market_access##", "##verygood_market_access##",
                                         "##high_market_access##", "##awesome_market_access##"
                                       };

struct ColoredWay
{
  NColor color;
  TilesArray tiles;
};

class MarketAccess::Impl
{
public:
  struct
  {
    OverlayPtr selected;
    OverlayPtr underMouse;
  } overlay;

  DateTime lastUpdate;
  std::vector<ColoredWay> ways;
};

int MarketAccess::type() const {  return citylayer::market; }

void MarketAccess::drawTile(const RenderInfo& rinfo, Tile& tile)
{
  if( tile.overlay().isNull() )
  {
    drawLandTile( rinfo, tile );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();
    int accessLevel = 0;

    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();
      accessLevel = (int)house->getServiceValue( Service::market );
      needDrawAnimations = (house->level() <= HouseLevel::hovel) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( rinfo, overlay->area(), ResourceGroup::foodOverlay, config::id.overlay.inHouseBase );
      }
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
    else if( accessLevel >= 0 )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, accessLevel );
    }
  }

  tile.setRendered();
}

LayerPtr MarketAccess::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new MarketAccess( camera, city ) );
  ret->drop();

  return ret;
}

void MarketAccess::handleEvent(NEvent& event)
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
        if( house.isValid() )
        {
          int accessLevel = house->getServiceValue( Service::market );
          accessLevel = math::clamp<int>( accessLevel / maxAccessLevel, 0, maxAccessLevel-1 );
          text = marketLevelName[ accessLevel ];
        }
      }

      _dfunc()->overlay.underMouse = tile->overlay();

      _setTooltipText( text );
    }
    break;

    case mouseLbtnPressed:
    {
      _updatePaths();
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

void MarketAccess::_updatePaths()
{
  __D_REF(d,MarketAccess)
  if( d.overlay.underMouse.is<Market>() )
  {
    d.overlay.selected = d.overlay.underMouse;
  }

  auto wbuilding = d.overlay.selected.as<Market>();
  if( wbuilding.isValid() )
  {
    d.ways.clear();
    const WalkerList& walkers = wbuilding->walkers();
    for( auto walker : walkers )
    {
      NColor color = walker.is<MarketBuyer>() ? ColorList::red : ColorList::blue;
      d.ways.push_back( ColoredWay{ color, walker->pathway().allTiles() } );
    }
  }
}

void MarketAccess::render(Engine& engine)
{
  Info::render( engine );

  RenderInfo rinfo{ engine, _camera()->offset() };
  for( auto& wayinfo : _dfunc()->ways )
  {
    TexturedPath::draw( wayinfo.tiles, rinfo, wayinfo.color );
  }
}

void MarketAccess::afterRender(Engine& engine)
{
  Info::afterRender(engine);

  if( game::Date::isDayChanged() )
    _updatePaths();
}

MarketAccess::MarketAccess( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, accessColumnIndex ), __INIT_IMPL(MarketAccess)
{
  _visibleWalkers() << walker::marketBuyer
                    << walker::marketLady
                    << walker::marketKid;

  _initialize();
}

}//end namespace city
