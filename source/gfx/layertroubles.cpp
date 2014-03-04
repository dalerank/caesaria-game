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

#include "layertroubles.hpp"
#include "layerconstants.hpp"
#include "core/gettext.hpp"
#include "objects/house.hpp"
#include "objects/constants.hpp"
#include "city/helper.hpp"
#include "objects/house_level.hpp"
#include "good/goodhelper.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "objects/watersupply.hpp"
#include "tilemap_camera.hpp"
#include "objects/factory.hpp"

using namespace constants;

int LayerTroubles::getType() const{  return citylayer::troubles;}
Layer::VisibleWalkers LayerTroubles::getVisibleWalkers() const{  return std::set<int>();}

void LayerTroubles::drawTile(GfxEngine& engine, Tile& tile, Point offset)
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

    int educationLevel = -1;
    switch( overlay->type() )
    {
      //fire buildings and roads
    case construction::road:
    case construction::plaza:
      needDrawAnimations = true;
      engine.drawPicture( tile.getPicture(), screenPos );
    break;

      //houses
    case building::house:
      {
        HousePtr house = ptr_cast<House>( overlay );

        needDrawAnimations = (house->getSpec().level() == 1) && (house->getHabitants().empty());

        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );              
      }
    break;

      //other buildings
    default:
      {
        CityHelper helper( _getCity() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;
    }

    if( needDrawAnimations )
    {
      registerTileForRendering( tile );
    }
    else if( educationLevel > 0 )
    {
      //drawColumn( engine, screenPos, educationLevel );
    }
  }
}

LayerPtr LayerTroubles::create(TilemapCamera& camera, PlayerCityPtr city )
{
  LayerPtr ret( new LayerTroubles( camera, city ) );
  ret->drop();

  return ret;
}

void LayerTroubles::handleEvent(NEvent& event)
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
        if( constr.isValid() )
        {
          text = constr->getTrouble();
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

LayerTroubles::LayerTroubles( TilemapCamera& camera, PlayerCityPtr city)
  : Layer( camera, city )
{
  _loadColumnPicture( 9 );
}
