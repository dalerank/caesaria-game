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

#include "damage.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"
#include "objects/engineer_post.hpp"
#include "game/gamedate.hpp"
#include "gfx/textured_path.hpp"
#include "city/statistic.hpp"
#include "core/event.hpp"
#include "gfx/tilemap_camera.hpp"

using namespace gfx;

namespace citylayer
{

enum { maxDamageLevel=10, damageColumnIndex=15 };
static const char* damageLevelName[maxDamageLevel] = {
                                         "##none_damage_risk##", "##some_defects_damage_risk##",
                                         "##very_low_damage_risk##", "##low_damage_risk##",
                                         "##little_damage_risk##",   "##some_damage_risk##",
                                         "##high_damage_risk##", "##collapse_available_damage_risk##",
                                         "##very_high_damage_risk##", "##extreme_damage_risk##"
                                       };

class Damage::Impl
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

int Damage::type() const {  return citylayer::damage; }

void Damage::drawTile( const RenderInfo& rinfo, Tile& tile )
{
  if( tile.overlay().isNull() )
  {
    drawPass( rinfo, tile, Renderer::ground );
    drawPass( rinfo, tile, Renderer::groundAnimation );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();
    int damageLevel = 0;

    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();
      damageLevel = (int)house->state( pr::damage );
      needDrawAnimations = (house->level() <= HouseLevel::hovel) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.house );
      }
    }
    else
    {
      auto building = overlay.as<Building>();
      if( building.isValid() )
      {
        damageLevel = (int)building->state( pr::damage );
      }

      drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.constr );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else if( damageLevel >= 0 )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, damageLevel );
    }
  }

  tile.setRendered();
}

void Damage::onEvent( const NEvent& event)
{
  __D_REF(d,Damage)
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
        auto construction = tile->overlay<Construction>();
        if( construction.isValid() )
        {
          int damageLevel = math::clamp<int>( construction->state( pr::damage ) / maxDamageLevel, 0, maxDamageLevel-1 );
          text = damageLevelName[ damageLevel ];
        }

        d.overlay.underMouse = tile->overlay();
      }

      _setTooltipText( text );
    }
    break;

    case NEvent::Mouse::btnLeftPressed:
    {
      if( d.overlay.underMouse.is<EngineerPost>() )
      {
        d.overlay.selected = d.overlay.underMouse;
        _updatePaths();
      }
    }
    break;

    default: break;
    }
  }

  Layer::onEvent( event );
}

void Damage::afterRender(Engine& engine)
{
  Info::afterRender(engine);

  if( game::Date::isDayChanged() )
    _updatePaths();
}

void Damage::render(Engine& engine)
{
  Info::render( engine );

  RenderInfo rinfo{ engine, _camera()->offset() };
  for( auto& tiles : _dfunc()->ways )
    TexturedPath::draw( tiles, rinfo );
}

void Damage::_updatePaths()
{
  __D_REF(d,Damage)
  auto wbuilding = d.overlay.selected.as<EngineerPost>();
  if( wbuilding.isValid() )
  {
    d.ways.clear();
    const WalkerList& walkers = wbuilding->walkers();
    for( auto walker : walkers )
      d.ways.push_back( walker->pathway().allTiles() );
  }
}

Damage::Damage( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, damageColumnIndex ), __INIT_IMPL(Damage)
{
  _addWalkerType( walker::engineer );
  _initialize();
}

}//end namespace city
