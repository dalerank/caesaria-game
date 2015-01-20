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
#include "walker/cart_pusher.hpp"

using namespace constants;

namespace gfx
{

namespace layer
{

int Food::type() const {  return citylayer::food; }

void Food::drawTile(Engine& engine, Tile& tile, const Point& offset)
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
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;     
    }
    else if( overlay->type() == objects::house )
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
    else      //other buildings
    {
      city::Helper helper( _city() );
      drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base);
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( foodLevel >= 0 )
    {
      _addColumn( screenPos, math::clamp( 100 - foodLevel, 0, 100 ) );
    }
  }

  tile.setWasDrawn();
}

void Food::drawWalkers(Engine &engine, const Tile &tile, const Point &camOffset)
{
  Pictures pics;
  const WalkerList& walkers = _city()->walkers( tile.pos() );

  foreach( w, walkers )
  {
    WalkerPtr wlk = *w;
    if( wlk->type() == walker::cartPusher )
    {
      CartPusherPtr cartp = ptr_cast<CartPusher>( wlk );
      good::Product gtype = cartp->stock().type();
      if( gtype == good::none || gtype > good::vegetable )
        continue;
    }
    pics.clear();
    (*w)->getPictures( pics );
    engine.draw( pics, (*w)->mappos() + camOffset );
  }
}

void Food::handleEvent(NEvent& event)
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
            good::Store& st = house->goodStore();
            int foodQty = 0;
            for( good::Product k=good::wheat; k <= good::vegetable; ++k )
            {
              foodQty += st.qty( k );
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

LayerPtr Food::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Food( camera, city ) );
  ret->drop();

  return ret;
}

Food::Food( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 18 )
{
  _visibleWalkers() << walker::marketLady << walker::marketKid
                    << walker::fishingBoat << walker::marketBuyer
                    << walker::cartPusher;

  _fillVisibleObjects( type() );
}

}//

}//end namespace gfx
