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

#include "layerreligion.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "core/utils.hpp"
#include "core/event.hpp"
#include "objects/religion.hpp"
#include "gfx/textured_path.hpp"
#include "gfx/tilemap_camera.hpp"
#include "core/gettext.hpp"

using namespace gfx;

namespace citylayer
{

class Religion::Impl
{
public:
  struct
  {
    OverlayPtr selected;
    OverlayPtr underMouse;
  } overlay;

  DateTime lastUpdate;
  std::vector<TilesArray> ways;
};

int Religion::type() const { return citylayer::religion; }

void Religion::drawTile( const RenderInfo& rinfo, Tile& tile)
{
  if( tile.overlay().isNull() )
  {
    drawLandTile( rinfo, tile );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();

    int religionLevel = -1;
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;      
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();
      religionLevel = (int) house->getServiceValue(Service::religionMercury);
      religionLevel += house->getServiceValue(Service::religionVenus);
      religionLevel += house->getServiceValue(Service::religionMars);
      religionLevel += house->getServiceValue(Service::religionNeptune);
      religionLevel += house->getServiceValue(Service::religionCeres);
      religionLevel = math::clamp( religionLevel / (house->spec().minReligionLevel()+1), 0, 100 );
      needDrawAnimations = (house->level() <= HouseLevel::hovel) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.house );
      }
    }
    else
    {
      drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.constr );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else if( religionLevel > 0 )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, religionLevel );
    }
  }

  tile.setRendered();
}

void Religion::render(Engine& engine)
{
  Info::render( engine );

  RenderInfo rinfo{ engine, _camera()->offset() };
  for( auto& tiles : _d->ways )
    TexturedPath::draw( tiles, rinfo );
}

void Religion::_updatePaths()
{
  auto wbuilding = _d->overlay.selected.as<Temple>();
  if( wbuilding.isValid() )
  {
    _d->ways.clear();
    const WalkerList& walkers = wbuilding->walkers();
    for( auto walker : walkers )
      _d->ways.push_back( walker->pathway().allTiles() );
  }
}

void Religion::onEvent( const NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case NEvent::Mouse::moved:
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      std::string text = "";
      if( tile != 0 )
      {
        auto house = tile->overlay<House>();
        if( house.isValid() )
        {
          int templeAccess = house->spec().computeReligionLevel( house );
          bool oracleAccess = house->hasServiceAccess( Service::oracle );

          text = (templeAccess == 5 && oracleAccess )
                  ? "##religion_access_full##"
                  : utils::format( 0xff, "##religion_access_%d_temple##", templeAccess );
        }

        _d->overlay.underMouse = tile->overlay();
      }

      _setTooltipText( _(text) );
    }
    break;

    case NEvent::Mouse::btnLeftPressed:
    {
      if( _d->overlay.underMouse.is<Temple>() )
      {
        _d->overlay.selected = _d->overlay.underMouse;
        _updatePaths();
      }
    }
    break;

    default: break;
    }
  }

  Layer::onEvent( event );
}

Religion::Religion( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 9 ), _d( new Impl )
{
  _addWalkerType( walker::priest );
  _initialize();
}

}//end namespace citylayer
