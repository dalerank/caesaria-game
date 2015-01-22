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

namespace gfx
{

namespace layer
{


static const char* fireLevelName[] = {
                                       "##no_fire_risk##",
                                       "##very_low_fire_risk##", "##some_low_fire_risk##", "##low_fire_risk##",
                                       "##middle_file_risk##", "##some_fire_risk##", "##high_fire_risk##",
                                       "##very_high_fire_risk##",
                                       "##extreme_fire_risk##", "##moment_fire_risk##"
                                     };


int Fire::type() const {  return citylayer::fire; }

void Fire::drawTile(Engine& engine, Tile& tile, const Point& offset)
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
    int fireLevel = 0;
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;
    }
    else if( overlay->type() == objects::house )
    {
      HousePtr house = ptr_cast<House>( overlay );
      fireLevel = (int)house->state( Construction::fire );
      needDrawAnimations = (house->spec().level() == 1) && house->habitants().empty();
      city::Helper helper( _city() );
      drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase  );
    }
    else //other buildings
    {
      ConstructionPtr constr = ptr_cast<Construction>( overlay );
      if( constr != 0 )
      {
        fireLevel = (int)constr->state( Construction::fire );
      }

      city::Helper helper( _city() );
      drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base  );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( fireLevel >= 0)
    {
      _addColumn( screenPos, fireLevel );
    }
  }

  tile.setWasDrawn();
}

void Fire::handleEvent(NEvent& event)
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
        ConstructionPtr constr = ptr_cast<Construction>( tile->overlay() );
        if( constr != 0 )
        {
          int fireLevel = math::clamp<int>( constr->state( Construction::fire ), 0, 100 );
          text = fireLevelName[ math::clamp<int>( fireLevel / 10, 0, 9 ) ];
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

LayerPtr Fire::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Fire( camera, city ) );
  ret->drop();

  return ret;
}

Fire::Fire( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 18 )
{
  _addWalkerType( walker::prefect );
  _fillVisibleObjects( citylayer::fire );
}

}

}//end namespace gfx
