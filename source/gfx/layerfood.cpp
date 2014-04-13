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

int LayerFood::getType() const {  return citylayer::food; }

Layer::VisibleWalkers LayerFood::getVisibleWalkers() const
{
  VisibleWalkers ret;
  ret.insert( walker::marketLady );
  ret.insert( walker::marketKid );
  ret.insert( walker::marketBuyer );

  return ret;
}

void LayerFood::drawTile( Engine& engine, Tile& tile, Point offset)
{
  Point screenPos = tile.mapPos() + offset;

  tile.setWasDrawn();

  if( tile.overlay().isNull() )
  {
    //draw background
    engine.drawPicture( tile.picture(), screenPos );
  }
  else
  {
    bool needDrawAnimations = false;
    TileOverlayPtr overlay = tile.overlay();
    Picture pic;
    int foodLevel = -1;
    switch( overlay->type() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::plaza:
    case building::market:
    case building::granary:
      pic = tile.picture();
      needDrawAnimations = true;
      drawTilePass( engine, tile, offset, Renderer::foreground );
    break;

      //houses
    case building::house:
      {
        city::Helper helper( _city() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
        HousePtr house = ptr_cast<House>( overlay );
        foodLevel = house->getState( (Construction::Param)House::food );
        needDrawAnimations = (house->getSpec().level() == 1) && (house->getHabitants().empty());
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

    if ( pic.isValid())
    {
      engine.drawPicture( pic, screenPos );
    }

    if( needDrawAnimations )
    {
      registerTileForRendering( tile );
    }
    else if( foodLevel >= 0 )
    {
      drawColumn( engine, screenPos, math::clamp( 100 - foodLevel, 0, 100 ) );
    }
  }
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
          GoodStore& st = house->getGoodStore();
          int foodQty = 0;
          for( int k=Good::wheat; k <= Good::vegetable; k++ )
          {
            foodQty += st.qty( (Good::Type)k );
          }
          int monthWithFood = 2 * foodQty / house->getHabitants().count();

          switch( monthWithFood )
          {
          case 0: text = "##house_have_not_food##"; break;
          case 1: text = "##house_food_only_for_month##"; break;
          case 2: case 3: text = "##house_have_some_food##"; break;
          default: text = "##house_have_much_food##"; break;
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
