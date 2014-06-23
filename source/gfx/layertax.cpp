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

#include "layertax.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"

using namespace constants;

namespace gfx
{

int LayerTax::type() const
{
  return citylayer::tax;
}

Layer::VisibleWalkers LayerTax::getVisibleWalkers() const
{
  VisibleWalkers ret;
  ret.insert( walker::taxCollector );

  return ret;
}

void LayerTax::drawTile(Engine& engine, Tile& tile, Point offset)
{
  Point screenPos = tile.mapPos() + offset;

  if( tile.overlay().isNull() )
  {
    //draw background
    engine.draw( tile.picture(), screenPos );
  }
  else
  {
    bool needDrawAnimations = false;
    TileOverlayPtr overlay = tile.overlay();

    int taxLevel = -1;
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

    // Tax-related
    case building::senate:
    case building::forum:
      needDrawAnimations = true;
    break;

      //houses
    case building::house:
      {
        HousePtr house = ptr_cast<House>( overlay );
        //taxLevel = house->getServiceValue( Service::forum );
        taxLevel = (int) (house->taxesThisYear() / 10);
        needDrawAnimations = (house->spec().level() == 1) && (house->habitants().empty());

        if( !needDrawAnimations )
        {
          city::Helper helper( _city() );
          drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
        }
      }
    break;

      //other buildings
    default:
      {
        city::Helper helper( _city() );
        drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( taxLevel > 0 )
    {
      drawColumn( engine, screenPos, taxLevel );
    }
  }

  tile.setWasDrawn();
}

LayerPtr LayerTax::create( Camera& camera, PlayerCityPtr city )
{
  LayerPtr ret( new LayerTax( camera, city ) );
  ret->drop();

  return ret;
}

LayerTax::LayerTax( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city )
{
  _loadColumnPicture( 9 );
}

}//end namespace gfx
