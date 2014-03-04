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

#include "layerfire.hpp"
#include "tileoverlay.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "layerconstants.hpp"
#include "tilemap_camera.hpp"
#include "core/event.hpp"
#include "core/gettext.hpp"

using namespace constants;

static const char* fireLevelName[] = { "##very_low_fire_risk##", "##very_low_fire_risk##", "##low_fire_risk##",
                                       "##some_fire_risk##", "##very_high_fire_risk##", "##extreme_fire_risk##" };

namespace {
  std::set<int> layerFireWalkers;
}

int LayerFire::getType() const {  return citylayer::fire; }

std::set<int> LayerFire::getVisibleWalkers() const {  return layerFireWalkers; }

void LayerFire::drawTile(GfxEngine& engine, Tile& tile, Point offset)
{
  Point screenPos = tile.mapPos() + offset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.overlay().isNull() )
  {
    //draw background
    engine.drawPicture( tile.getPicture(), screenPos );
  }
  else
  {
    TileOverlayPtr overlay = tile.overlay();
    int fireLevel = 0;
    switch( overlay->type() )
    {
    //fire buildings and roads
    case construction::road:
    case construction::plaza:
    case building::burningRuins:
    case building::burnedRuins:
    case building::collapsedRuins:
    case building::prefecture:
    case building::well:
    case building::fountain:
    case building::lowBridge:
    case building::highBridge:
      engine.drawPicture( tile.getPicture(), screenPos );
      drawTilePass( engine, tile, offset, Renderer::foreground );
      needDrawAnimations = true;
    break;

      //houses
    case building::house:
      {
        HousePtr house = ptr_cast<House>( overlay );
        fireLevel = (int)house->getState( Construction::fire );
        needDrawAnimations = (house->getSpec().getLevel() == 1) && house->getHabitants().empty();

        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase  );
      }
    break;

      //other buildings
    default:
      {
        ConstructionPtr constr = ptr_cast<Construction>( overlay );
        if( constr != 0 )
        {
          fireLevel = (int)constr->getState( Construction::fire );
        }

        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base  );
      }
    break;
    }

    if( needDrawAnimations )
    {
      registerTileForRendering( tile );
    }
    else if( fireLevel >= 0)
    {
      drawColumn( engine, screenPos, fireLevel );
    }
  }
}

void LayerFire::handleEvent(NEvent& event)
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
        ConstructionPtr constr = ptr_cast<Construction>( tile->overlay() );
        if( constr != 0 )
        {
          int fireLevel = math::clamp<int>( constr->getState( Construction::fire ), 0, 100 );
          text = fireLevelName[ fireLevel / 16 ];
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

LayerPtr LayerFire::create(TilemapCamera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerFire( camera, city ) );
  ret->drop();

  return ret;
}

LayerFire::LayerFire(TilemapCamera& camera, PlayerCityPtr city)
  : Layer( camera, city )
{
  _loadColumnPicture( 18 );
  layerFireWalkers.insert( walker::prefect );
}
