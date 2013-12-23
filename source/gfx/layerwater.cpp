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

#include "layerwater.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "layerconstants.hpp"
#include "city/helper.hpp"
#include "tilemap.hpp"

using namespace constants;

int LayerWater::getType() const
{
  return citylayer::water;
}

std::set<int> LayerWater::getVisibleWalkers() const
{
  return std::set<int>();
}

void LayerWater::drawTile(GfxEngine& engine, Tile& tile, Point offset)
{
  Point screenPos = tile.getXY() + offset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  Size areaSize(1);

  if( tile.getOverlay().isNull() )
  {
    //draw background
    engine.drawPicture( tile.getPicture(), screenPos );
  }
  else
  {
    TileOverlayPtr overlay = tile.getOverlay();
    Picture pic;
    switch( overlay->getType() )
    {
      //water buildings
    case construction::road:
    case construction::plaza:
    case building::reservoir:
    case building::fountain:
    case building::well:
    case building::aqueduct:
      pic = tile.getPicture();
      needDrawAnimations = true;
      areaSize = overlay->getSize();
    break;

    default:
    {
      int tileNumber = 0;
      bool haveWater = tile.getWaterService( WTR_FONTAIN ) > 0 || tile.getWaterService( WTR_WELL ) > 0;
      if ( overlay->getType() == building::house )
      {
        HousePtr h = overlay.as<House>();
        tileNumber = OverlayPic::inHouse;
        haveWater = haveWater || h->hasServiceAccess(Service::fontain) || h->hasServiceAccess(Service::well);
      }
      tileNumber += (haveWater ? OverlayPic::haveWater : 0);
      tileNumber += tile.getWaterService( WTR_RESERVOIR ) > 0 ? OverlayPic::reservoirRange : 0;

      CityHelper helper( _getCity() );
      drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::waterOverlay, OverlayPic::base + tileNumber );

      pic = Picture::getInvalid();
      areaSize = 0;
      needDrawAnimations = false;
    }
    break;
    }

    if ( pic.isValid() )
    {
      engine.drawPicture( pic, screenPos );
    }

    if( needDrawAnimations )
    {
      registerTileForRendering( tile );
    }
  }

  if( !needDrawAnimations && ( tile.isWalkable(true) || tile.getFlag( Tile::tlBuilding ) ) )
  {
    Tilemap& tilemap = _getCity()->getTilemap();
    TilesArray area = tilemap.getArea( tile.getIJ(), areaSize );

    foreach( Tile* rtile, area )
    {
      int reservoirWater = rtile->getWaterService( WTR_RESERVOIR );
      int fontainWater = rtile->getWaterService( WTR_FONTAIN );

      if( (reservoirWater + fontainWater > 0) && ! rtile->getFlag( Tile::tlWater ) && rtile->getOverlay().isNull() )
      {
        int picIndex = reservoirWater ? OverlayPic::reservoirRange : 0;
        picIndex |= fontainWater > 0 ? OverlayPic::haveWater : 0;
        picIndex |= OverlayPic::skipLeftBorder | OverlayPic::skipRightBorder;
        engine.drawPicture( Picture::load( ResourceGroup::waterOverlay, picIndex + OverlayPic::base ), rtile->getXY() + offset );
      }
    }
  }

}

LayerPtr LayerWater::create(TilemapCamera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerWater( camera, city ) );
  ret->drop();

  return ret;
}

LayerWater::LayerWater(TilemapCamera& camera, PlayerCityPtr city)
  : Layer( camera, city )
{
}
