// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "layerdesirability.hpp"
#include "game/resourcegroup.hpp"
#include "building/constants.hpp"
#include "game/city.hpp"
#include "gfx/sdl_engine.hpp"
#include "core/stringhelper.hpp"
#include "layerconstants.hpp"

using namespace constants;

int LayerDesirability::getType() const
{
  return citylayer::desirability;
}

std::set<int> LayerDesirability::getVisibleWalkers() const
{
  return std::set<int>();
}

void LayerDesirability::drawTile(GfxEngine& engine, Tile& tile, Point offset)
{
  //Tilemap& tilemap = _city->getTilemap();
  Point screenPos = tile.getXY() + offset;

  tile.setWasDrawn();

  if( tile.getOverlay().isNull() )
  {
    //draw background
    if( tile.getFlag( Tile::isConstructible ) && tile.getDesirability() != 0 )
    {
      int picOffset = tile.getDesirability() < 0
                          ? math::clamp( tile.getDesirability() / 25, -3, 0 )
                          : math::clamp( tile.getDesirability() / 15, 0, 6 );
      Picture& pic = Picture::load( ResourceGroup::land2a, 37 + picOffset );

      engine.drawPicture( pic, screenPos );
    }
    else
    {
      engine.drawPicture( tile.getPicture(), screenPos );
    }
  }
  else
  {
    TileOverlayPtr overlay = tile.getOverlay();
    switch( overlay->getType() )
    {
    //roads
    case construction::road:
    case construction::B_PLAZA:
      engine.drawPicture( tile.getPicture(), screenPos );
      _renderer->registerTileForRendering( tile );
    break;

    //other buildings
    default:
      {
        int picOffset = tile.getDesirability() < 0
                          ? math::clamp( tile.getDesirability() / 25, -3, 0 )
                          : math::clamp( tile.getDesirability() / 15, 0, 6 );
        Picture& pic = Picture::load( ResourceGroup::land2a, 37 + picOffset );

        CityHelper helper( _city );
        TilesArray tiles4clear = helper.getArea( overlay );

        foreach( Tile* tile, tiles4clear )
        {
          engine.drawPicture( pic, tile->getXY() + offset );
        }
      }
    break;
    }
  }

  if( tile.getDesirability() != 0 )
  {
    GfxSdlEngine* painter = static_cast< GfxSdlEngine* >( &engine );
    _debugFont.draw( painter->getScreen(), StringHelper::format( 0xff, "%d", tile.getDesirability() ), screenPos + Point( 20, -15 ), false );
  }
}

LayerPtr LayerDesirability::create(CityRenderer* renderer, PlayerCityPtr city)
{
  LayerDesirability* l = new LayerDesirability();
  l->_renderer = renderer;
  l->_city = city;

  LayerPtr ret( l );
  ret->drop();

  return ret;
}
