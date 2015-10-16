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
#include "objects/overlay.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "game/resourcegroup.hpp"
#include "city/statistic.hpp"
#include "constants.hpp"
#include "core/event.hpp"
#include "core/priorities.hpp"
#include "gfx/tilemap_camera.hpp"
#include "core/gettext.hpp"
#include "good/store.hpp"
#include "walker/cart_pusher.hpp"

using namespace gfx;

namespace citylayer
{

int Food::type() const {  return citylayer::food; }

void Food::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  Point screenPos = tile.mappos() + offset;

  if( tile.overlay().isNull() )
  {
    drawPass( engine, tile, offset, Renderer::ground );
    drawPass( engine, tile, offset, Renderer::groundAnimation );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();
    int foodLevel = -1;
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;     
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();
      foodLevel = (int) house->state( pr::food );
      needDrawAnimations = (house->level() <= HouseLevel::hovel) && (house->habitants().empty());
      if( !needDrawAnimations )
      {
        drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, config::id.overlay.inHouseBase );
      }
    }
    else      //other buildings
    {
      drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, config::id.overlay.base);
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

  tile.setRendered();
}

void Food::drawWalkers(Engine &engine, const Tile &tile, const Point &camOffset)
{
  Pictures pics;
  const WalkerList& walkers = _city()->walkers( tile.pos() );

  for( auto wlk : walkers )
  {
    if( wlk->type() == walker::cartPusher )
    {
      auto cartPusher = wlk.as<CartPusher>();
      good::Product gtype = cartPusher->stock().type();
      if( gtype == good::none || gtype > good::vegetable )
        continue;
    }
    pics.clear();
    wlk->getPictures( pics );
    engine.draw( pics, wlk->mappos() + camOffset );
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
        auto house = tile->overlay<House>();
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

  _initialize();
}

}//end namespace citylayer
