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

int Damage::type() const {  return citylayer::damage; }

void Damage::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  Point screenPos = tile.mappos() + offset;

  if( tile.overlay().isNull() )
  {
    //draw background
    //engine.draw( tile.picture(), screenPos );

    drawPass( engine, tile, offset, Renderer::ground );
    drawPass( engine, tile, offset, Renderer::groundAnimation );
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
      HousePtr house = ptr_cast<House>( overlay );
      damageLevel = (int)house->state( pr::damage );
      needDrawAnimations = (house->spec().level() == 1) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    }
    else
    {
      BuildingPtr building = ptr_cast<Building>( overlay );
      if( building.isValid() )
      {
        damageLevel = (int)building->state( pr::damage );
      }

      drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, OverlayPic::base );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( damageLevel >= 0 )
    {
      _addColumn( screenPos, damageLevel );
    }
  }

  tile.setWasDrawn();
}

LayerPtr Damage::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Damage( camera, city ) );
  ret->drop();

  return ret;
}

void Damage::handleEvent(NEvent& event)
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
        ConstructionPtr construction = ptr_cast<Construction>( tile->overlay() );
        if( construction.isValid() )
        {
          int damageLevel = math::clamp<int>( construction->state( pr::damage ) / maxDamageLevel, 0, maxDamageLevel-1 );
          text = damageLevelName[ damageLevel ];
        }
      }

      _setTooltipText( text );
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

Damage::Damage( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, damageColumnIndex )
{
  _addWalkerType( walker::engineer );
  _fillVisibleObjects( type() );
}

}//end namespace city
