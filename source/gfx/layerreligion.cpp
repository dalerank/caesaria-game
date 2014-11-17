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

#include "layerreligion.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"
#include "core/stringhelper.hpp"
#include "core/event.hpp"
#include "tilemap_camera.hpp"
#include "core/gettext.hpp"

using namespace constants;

namespace gfx
{

int LayerReligion::type() const
{
  return citylayer::religion;
}

void LayerReligion::drawTile(Engine& engine, Tile& tile, const Point& offset)
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

    int religionLevel = -1;
    switch( overlay->type() )
    {
    // Base set of visible objects
    case objects::road:
    case objects::plaza:
    case objects::garden:

    case objects::burnedRuins:
    case objects::collapsedRuins:

    case objects::lowBridge:
    case objects::highBridge:

    case objects::elevation:
    case objects::rift:

    // Religion-related
    case objects::templeCeres:
    case objects::templeMars:
    case objects::templeMercury:
    case objects::templeNeptune:
    case objects::templeVenus:

    case objects::cathedralCeres:
    case objects::cathedralMars:
    case objects::cathedralMercury:
    case objects::cathedralNeptune:
    case objects::cathedralVenus:

    case objects::oracle:
      needDrawAnimations = true;      
    break;

      //houses
    case objects::house:
      {
        HousePtr house = ptr_cast<House>( overlay );
        religionLevel = (int) house->getServiceValue(Service::religionMercury);
        religionLevel += house->getServiceValue(Service::religionVenus);
        religionLevel += house->getServiceValue(Service::religionMars);
        religionLevel += house->getServiceValue(Service::religionNeptune);
        religionLevel += house->getServiceValue(Service::religionCeres);
        religionLevel = math::clamp( religionLevel / (house->spec().minReligionLevel()+1), 0, 100 );
        needDrawAnimations = (house->spec().level() == 1) && house->habitants().empty();

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
    else if( religionLevel > 0 )
    {
      _addColumn( screenPos, religionLevel );
    }
  }

  tile.setWasDrawn();
}

void LayerReligion::handleEvent(NEvent& event)
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
          int templeAccess = house->spec().computeReligionLevel( house );
          bool oracleAccess = house->hasServiceAccess( Service::oracle );

          text = (templeAccess == 5 && oracleAccess )
                  ? "##religion_access_full##"
                  : StringHelper::format( 0xff, "##religion_access_%d_temple##", templeAccess );
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

LayerPtr LayerReligion::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerReligion( camera, city ) );
  ret->drop();

  return ret;
}

LayerReligion::LayerReligion( Camera& camera, PlayerCityPtr city)
  : LayerInfo( camera, city, 9 )
{
  _addWalkerType( walker::priest );
}

}//end namespace gfx
