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

#include "layer.hpp"
#include "tileoverlay.hpp"
#include "core/foreach.hpp"
#include "game/resourcegroup.hpp"

void Layer::drawTilePass( GfxEngine& engine, Tile& tile, Point offset, Renderer::Pass pass)
{
  if( tile.getOverlay().isNull() )
    return;

  const PicturesArray& pictures = tile.getOverlay()->getPictures( pass );

  for( PicturesArray::const_iterator it=pictures.begin(); it != pictures.end(); it++ )
  {
    engine.drawPicture( *it, tile.getXY() + offset );
  }
}

void Layer::drawArea( GfxEngine& engine, const TilemapArea& area, Point offset, std::string resourceGroup, int tileId)
{
  if( area.empty() )
    return;

  Tile* baseTile = area.front();
  TileOverlayPtr overlay = baseTile->getOverlay();
  Picture *pic = NULL;
  int leftBorderAtI = baseTile->getI();
  int rightBorderAtJ = overlay->getSize().getHeight() - 1 + baseTile->getJ();
  for( TilemapArea::const_iterator it=area.begin(); it != area.end(); it++ )
  {
    Tile* tile = *it;
    int tileBorders = ( tile->getI() == leftBorderAtI ? 0 : OverlayPic::skipLeftBorder )
                      + ( tile->getJ() == rightBorderAtJ ? 0 : OverlayPic::skipRightBorder );
    pic = &Picture::load(resourceGroup, tileBorders + tileId);
    engine.drawPicture( *pic, tile->getXY() + offset );
  }
}

void Layer::drawColumn( GfxEngine& engine, const Point& pos, const int startPicId, const int percent)
{
  engine.drawPicture( Picture::load( ResourceGroup::sprites, startPicId + 2 ), pos + Point( 5, 15 ) );

  int roundPercent = ( percent / 10 ) * 10;
  Picture& pic = Picture::load( ResourceGroup::sprites, startPicId + 1 );
  for( int offsetY=10; offsetY < roundPercent; offsetY += 10 )
  {
    engine.drawPicture( pic, pos - Point( -13, -5 + offsetY ) );
  }

  if( percent >= 10 )
  {
    engine.drawPicture( Picture::load( ResourceGroup::sprites, startPicId ), pos - Point( -1, -6 + roundPercent ) );
  }
}
