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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "minimap_window.hpp"
#include "gfx/tilemap.hpp"
#include "game/minimap_colours.hpp"
#include "gfx/tile.hpp"
#include "gfx/tileoverlay.hpp"
#include "core/time.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "core/gettext.hpp"
#include "city/city.hpp"
#include "objects/constants.hpp"
#include "gfx/camera.hpp"
#include "walker/walker.hpp"
#include "core/tilerect.hpp"

using namespace gfx;
using namespace constants;

namespace gui
{

class Minimap::Impl
{
public:
  PictureRef minimap;

  PlayerCityPtr city;
  Camera const* camera;

  minimap::Colors* colors;

  int lastTimeUpdate;
  Point center;

  void getTerrainColours(const Tile& tile, int &c1, int &c2);
  void getBuildingColours(const Tile& tile, int &c1, int &c2);
  void updateImage();

public signals:
  Signal1<TilePos> onCenterChangeSignal;
};

Minimap::Minimap(Widget* parent, Rect rect, PlayerCityPtr city, const gfx::Camera& camera)
  : Widget( parent, -1, rect ), _d( new Impl )
{
  _d->city = city;
  _d->camera = &camera;
  _d->lastTimeUpdate = 0;
  _d->minimap.reset( Picture::create( Size( 144, 110 ), 0, true ) );
  _d->colors = new minimap::Colors( (ClimateType)city->climate() );
  setTooltipText( _("##minimap_tooltip##") );
}

Point getBitmapCoordinates(int x, int y, int mapsize ) {  return Point( x + y, x + mapsize - y - 1 ); }
void getBuildingColours( const Tile& tile, int &c1, int &c2 );

void Minimap::Impl::getTerrainColours(const Tile& tile, int &c1, int &c2)
{
  int rndData = tile.originalImgId();
  int num3 = rndData & 0x3;
  int num7 = rndData & 0x7;

  TileOverlay::Type ovType = objects::unknown;
  if( tile.overlay().isValid() )
    ovType = tile.overlay()->type();

  if( tile.i() < 0 || tile.j() < 0 )
  {
    c1 = c2 = 0xff000000;
    return;
  }

  if (tile.getFlag( Tile::tlTree ))
  {
    c1 = colors->colour(minimap::Colors::MAP_TREE1, num3);
    c2 = colors->colour(minimap::Colors::MAP_TREE2, num7);
  }
  else if (tile.getFlag( Tile::tlRock ))
  {
    c1 = colors->colour(minimap::Colors::MAP_ROCK1, num3);
    c2 = colors->colour(minimap::Colors::MAP_ROCK2, num3);
  }
  else if(tile.getFlag( Tile::tlDeepWater) )
  {
    c1 = colors->colour(minimap::Colors::MAP_WATER1, num3);
    c2 = colors->colour(minimap::Colors::MAP_WATER2, num3);
  }
  else if(tile.getFlag( Tile::tlWater ))
  {
    c1 = colors->colour(minimap::Colors::MAP_WATER1, num3);
    c2 = colors->colour(minimap::Colors::MAP_WATER2, num7);
  }
  else if (tile.getFlag( Tile::tlRoad ))
  {
    c1 = colors->colour(minimap::Colors::MAP_ROAD, 0);
    c2 = colors->colour(minimap::Colors::MAP_ROAD, 1);
  }
  else if (tile.getFlag( Tile::tlMeadow ))
  {
    c1 = colors->colour(minimap::Colors::MAP_FERTILE1, num3);
    c2 = colors->colour(minimap::Colors::MAP_FERTILE2, num7);
  }
  else if (tile.getFlag( Tile::tlWall ))
  {
    c1 = colors->colour(minimap::Colors::MAP_WALL, 0);
    c2 = colors->colour(minimap::Colors::MAP_WALL, 1);
  }
  else if( ovType == objects::aqueduct  )
  {
    c1 = colors->colour(minimap::Colors::MAP_AQUA, 0);
    c2 = colors->colour(minimap::Colors::MAP_AQUA, 1);
  }
  else if (tile.getFlag( Tile::tlOverlay ))
  {
    getBuildingColours(tile, c1, c2);
  }
  else // plain terrain
  {
    c1 = colors->colour(minimap::Colors::MAP_EMPTY1, num3);
    c2 = colors->colour(minimap::Colors::MAP_EMPTY2, num7);
  }

  c1 |= 0xff000000;
  c2 |= 0xff000000;

#ifdef CAESARIA_PLATFORM_ANDROID
  c1 = NColor( c1 ).abgr();
  c2 = NColor( c2 ).abgr();
#endif
}

void Minimap::Impl::getBuildingColours(const Tile& tile, int &c1, int &c2)
{
  TileOverlayPtr overlay = tile.overlay();

  if (overlay == NULL)
    return;

  TileOverlay::Type type = overlay->type();

  switch(type)
  {
    case objects::house:
    {
      switch (overlay->size().width())
      {
        case 1:
          {
            c1 = colors->colour(minimap::Colors::MAP_HOUSE, 0);
            c2 = colors->colour(minimap::Colors::MAP_HOUSE, 1);
          }
        break;

        default:
          {
            c1 = colors->colour(minimap::Colors::MAP_HOUSE, 2);
            c2 = colors->colour(minimap::Colors::MAP_HOUSE, 0);
          }
        }
        break;
      }
      break;

      case objects::reservoir:
      {
        c1 = colors->colour(minimap::Colors::MAP_AQUA, 1);
        c2 = colors->colour(minimap::Colors::MAP_AQUA, 0);
      }
      break;

      case objects::fort_javelin:
      case objects::fort_legionaries:
      case objects::fort_horse:
      {
        c1 = colors->colour(minimap::Colors::MAP_SPRITES, 1);
        c2 = colors->colour(minimap::Colors::MAP_SPRITES, 1);
      }
      break;

      default:
        {
          switch (overlay->size().width())
          {
          case 1:
          {
            c1 = colors->colour(minimap::Colors::MAP_BUILDING, 0);
            c2 = colors->colour(minimap::Colors::MAP_BUILDING, 1);
            break;
          }
          default:
          {
            c1 = colors->colour(minimap::Colors::MAP_BUILDING, 0);
            c2 = colors->colour(minimap::Colors::MAP_BUILDING, 2);
          }
        }
    }
  }

  c1 |= 0xff000000;
  c2 |= 0xff000000;

#ifdef CAESARIA_PLATFORM_ANDROID
  c1 = NColor( c1 ).abgr();
  c2 = NColor( c2 ).abgr();
#endif
}

void Minimap::Impl::updateImage()
{
  Tilemap& tilemap = city->tilemap();
  int mapsize = tilemap.size();

  // here we can draw anything
  mapsize = std::min( mapsize, 42 );
  TilePos tpos = camera->center();
  TilePos offset = TilePos( 80, 80 );
  TilePos startPos = tpos - offset;
  TilePos stopPos = tpos + offset;

  int w = minimap->width()-1;
  int h = minimap->height();
  unsigned int* pixels = minimap->lock();

  if( pixels != 0)
  {
    minimap->fill( 0xff000000, Rect() );
    for( int i = startPos.i(); i < stopPos.i(); i++)
    {
      for (int j = startPos.j(); j < stopPos.j(); j++)
      {
        const Tile& tile = tilemap.at(i, j);

        Point pnt = getBitmapCoordinates(i-startPos.i() - 40, j-startPos.j()-60, mapsize);
        int c1, c2;
        getTerrainColours( tile, c1, c2);

        if( pnt.y() < 0 || pnt.x() < 0 || pnt.x() >= w || pnt.y() >= h )
          continue;

        unsigned int* bufp32;
        bufp32 = pixels + pnt.y() * minimap->width() + pnt.x();
        *bufp32 = c1;
        *(bufp32+1) = c2;
      }
    }


    const WalkerList& walkers = city->walkers();
    TileRect trect( startPos, stopPos );
    //TilePos leftBottomPos = TilePos(std::min(startPos.i(), stopPos.i()), std::min(startPos.j(), stopPos.j()));
    //TilePos rightTopPos = TilePos(std::max(startPos.i(), stopPos.i()), std::max(startPos.j(), stopPos.j()));
    foreach( w, walkers)
    {
      TilePos pos = (*w)->pos();
      if( trect.contain( pos ) )
      {
        NColor cl;
        if ((*w)->agressive() != 0)
        {

          if ((*w)->agressive() > 0)
          {
            cl = DefaultColors::red;
          }
          else
          {
            cl = DefaultColors::blue;
          }

          if (cl.color != 0)
          {
            Point pnt = getBitmapCoordinates(pos.i() - startPos.i() - 40, pos.j() - startPos.j() - 60, mapsize);
            minimap->fill(cl, Rect(pnt, Size(2)));
          }
        }        
      }
    }
  }

  minimap->unlock();
  minimap->update();

  // show center of screen on minimap
  // Exit out of image size on small carts... please fix it

  /*sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX(),     mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ(), kWhite);
  sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX() + 1, mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ(), kWhite);
  sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX(),     mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() + 1, kWhite);
  sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX() + 1, mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() + 1, kWhite);

  for ( int i = TilemapRenderer::instance().getMapArea().getCenterX() - 18; i <= TilemapRenderer::instance().getMapArea().getCenterX() + 18; i++ )
  {
    sdlFacade.setPixel(surface, i, mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() + 34, kYellow);
    sdlFacade.setPixel(surface, i, mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() - 34, kYellow);
  }

  for ( int j = mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() - 34; j <= mapsize * 2 - TilemapRenderer::instance().getMapArea().getCenterZ() + 34; j++ )
  {
    sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX() - 18, j, kYellow);
    sdlFacade.setPixel(surface, TilemapRenderer::instance().getMapArea().getCenterX() + 18, j, kYellow);
  }
  */

  //fullmap->unlock();

  // this is window where minimap is displayed
}

/* end of helper functions */

namespace {
  static const int kWhite  = 0xFFFFFF;
  static const int kYellow = 0xFFFF00;
}

void Minimap::draw(Engine& painter)
{
  if( !visible() )
    return;

  if( DateTime::elapsedTime() - _d->lastTimeUpdate > 250 )
  {
    _d->updateImage();
    _d->lastTimeUpdate = DateTime::elapsedTime();
  }

  painter.draw( *_d->minimap, screenLeft(), screenTop() ); // 152, 145

  Widget::draw( painter );
}

void Minimap::setCenter( Point pos) {  _d->center = pos; }

bool Minimap::onEvent(const NEvent& event)
{
  if( sEventMouse == event.EventType
      && mouseLbtnRelease == event.mouse.type )
  {
    Point clickPosition = screenToLocal( event.mouse.pos() );

    int mapsize = _d->city->tilemap().size();
    Size minimapSize = _d->minimap->size();

    Point offset( minimapSize.width()/2 - _d->center.x(), minimapSize.height()/2 + _d->center.y() - mapsize*2 );
    clickPosition -= offset;
    TilePos tpos;
    tpos.setI( (clickPosition.x() + clickPosition.y() - mapsize + 1) / 2 );
    tpos.setJ( -clickPosition.y() + tpos.i() + mapsize - 1 );

    emit _d->onCenterChangeSignal( tpos );
  }

  return Widget::onEvent( event );
}

Signal1<TilePos>& Minimap::onCenterChange(){  return _d->onCenterChangeSignal; }

}//end namespace gui
