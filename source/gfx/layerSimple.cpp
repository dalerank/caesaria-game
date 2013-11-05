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

#include "layersimple.hpp"
#include "building/watersupply.hpp"
#include "layerconstants.hpp"

int LayerSimple::getType() const
{
  return constants::citylayer::simple;
}

std::set<int> LayerSimple::getVisibleWalkers() const
{
  std::set<int> ret;
  ret.insert( WT_ALL );

  return ret;
}

void LayerSimple::drawTile( GfxEngine& engine, Tile& tile, Point offset )
{
  Point screenPos = tile.getXY() + offset;

  TileOverlayPtr overlay = tile.getOverlay();
  const TilemapTiles& postTiles = _renderer->getPostTiles();

  if( overlay.isValid() )
  {
    if( overlay.is<Aqueduct>() && postTiles.size() > 0 )
    {
      // check, do we have any aqueducts there... there can be empty items
      bool isAqueducts = false;
      for( TilemapTiles::const_iterator it=postTiles.begin(); it != postTiles.end(); it++ )
      {
        if( (*it)->getOverlay().is<Aqueduct>() )
        {
          isAqueducts = true;
          break;
        }
      }

      if( isAqueducts )
      {
        tile.setWasDrawn();
        Picture& pic = overlay.as<Aqueduct>()->computePicture( _city, &postTiles, tile.getIJ());
        engine.drawPicture( pic, screenPos );
      }
    }

    _renderer->registerTileForRendering( tile );
  }

  if( !tile.getFlag( Tile::wasDrawn ) )
  {
    tile.setWasDrawn();
    engine.drawPicture( tile.getPicture(), screenPos );

    if( tile.getAnimation().isValid() )
    {
      engine.drawPicture( tile.getAnimation().getCurrentPicture(), screenPos );
    }

    drawTilePass( engine, tile, offset, Renderer::foreground );
  }
}

LayerPtr LayerSimple::create(CityRenderer* renderer, CityPtr city)
{
  LayerSimple* l = new LayerSimple();
  l->_renderer = renderer;
  l->_city = city;

  LayerPtr ret( l );
  ret->drop();

  return ret;
}
