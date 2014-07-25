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

#include "layerfood.hpp"
#include "tileoverlay.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "layerconstants.hpp"
#include "core/event.hpp"
#include "gfx/tilemap_camera.hpp"
#include "core/gettext.hpp"
#include "good/goodstore.hpp"

using namespace constants;

namespace gfx
{

int LayerFood::type() const {  return citylayer::food; }

Layer::VisibleWalkers LayerFood::visibleWalkers() const
{
  VisibleWalkers ret;
  ret.insert( walker::marketLady );
  ret.insert( walker::marketKid );
  ret.insert( walker::marketBuyer );

  return ret;
}

void LayerFood::drawTile( Engine& engine, Tile& tile, Point offset)
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
    int foodLevel = -1;
    switch( overlay->type() )
    {
    // Base set of visible objects
    case construction::road:
    case construction::plaza:
    case construction::garden:

    case building::burnedRuins:
    case building::collapsedRuins:

    case building::lowBridge:
    case building::highBridge:

    case building::elevation:
    case building::rift:

    // Food-related
    case building::market:
    case building::granary:
      needDrawAnimations = true;     
    break;

      //houses
    case building::house:
      {
        city::Helper helper( _city() );        
        HousePtr house = ptr_cast<House>( overlay );
        foodLevel = (int) house->state( (Construction::Param)House::food );
        needDrawAnimations = (house->spec().level() == 1) && (house->habitants().empty());
        if( !needDrawAnimations )
        {
          drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
        }
      }
    break;

      //other buildings
    default:
      {
        city::Helper helper( _city() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base);
      }
      break;
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( foodLevel >= 0 )
    {
      drawColumn( engine, screenPos, math::clamp( 100 - foodLevel, 0, 100 ) );
    }
  }

  tile.setWasDrawn();
}

void LayerFood::handleEvent(NEvent& event)
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
        if( house.isValid() )
        {
          int houseHabitantsCount = house->habitants().count();

          if( houseHabitantsCount > 0 )
          {
            GoodStore& st = house->goodStore();
            int foodQty = 0;
            for( int k=Good::wheat; k <= Good::vegetable; k++ )
            {
              foodQty += st.qty( (Good::Type)k );
            }
            int monthWithFood = 2 * foodQty / houseHabitantsCount;

            switch( monthWithFood )
            {
            case 0: text = "##house_have_not_food##"; break;
            case 1: text = "##house_food_only_for_month##"; break;
            case 2: case 3: text = "##house_have_some_food##"; break;
            default: text = "##house_have_much_food##"; break;
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

LayerPtr LayerFood::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerFood( camera, city ) );
  ret->drop();

  return ret;
}

LayerFood::LayerFood( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city )
{
  _loadColumnPicture( 18 );
}

}//end namespace gfx
