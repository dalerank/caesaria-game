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
#include "objects/overlay.hpp"
#include "core/time.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "core/gettext.hpp"
#include "city/city.hpp"
#include "objects/constants.hpp"
#include "gfx/camera.hpp"
#include "walker/walker.hpp"
#include "core/tilerect.hpp"
#include "texturedbutton.hpp"
#include "gfx/helper.hpp"
#include "gfx/IMG_savepng.h"
#include "gfx/decorator.hpp"
#include "city/states.hpp"

using namespace gfx;
using namespace constants;

namespace gui
{

class Minimap::Impl
{
public:
  Picture landRockWaterMap;
  Picture objectsMap;
  Picture walkersMap;
  Picture background;

  PlayerCityPtr city;
  Camera const* camera;

  unsigned int lastObjectsCount;
  ScopedPtr<minimap::Colors> colors;

  int lastTimeUpdate;
  Point center;
  TexturedButton* btnZoomIn;
  TexturedButton* btnZoomOut;

public:
  void getTileColours(const Tile& tile, int &c1, int &c2);
  void getTerrainColours(const Tile& tile, bool staticTiles, int& c1, int& c2 );
  void getObjectColours(const Tile& tile, int &c1, int &c2);
  void updateImage();
  void initStaticMmap();
  void drawStaticMmap(Picture& canvas , bool clear);
  void drawObjectsMmap(Picture& canvas, bool clear, bool force);
  void drawWalkersMmap(Picture& canvas, bool clear);

public signals:
  Signal1<TilePos> onCenterChangeSignal;
  Signal1<int>     onZoomChangeSignal;
};

Minimap::Minimap(Widget* parent, Rect rect, PlayerCityPtr city, const gfx::Camera& camera)
  : Widget( parent, Hash(CAESARIA_STR_A(Minimap)), rect ), _d( new Impl )
{
  setupUI( ":/gui/minimap.gui" );

  _d->city = city;
  _d->camera = &camera;
  _d->lastTimeUpdate = 0;
  _d->lastObjectsCount = 0;
  _d->background = Picture( Size( 144, 110 ), 0, true );
  _d->colors.reset( new minimap::Colors( city->climate() ) );
  _d->btnZoomIn =  new TexturedButton( this, righttop() - Point( 28, -2  ), Size( 24 ), -1, 605 );
  _d->btnZoomOut = new TexturedButton( this, righttop() - Point( 28, -26 ), Size( 24 ), -1, 601 );
  _d->initStaticMmap();
  _d->drawStaticMmap( _d->landRockWaterMap, true );
  setTooltipText( _("##minimap_tooltip##") );
}

Point getBitmapCoordinates(int x, int y, int mapsize ) { return Point( x + y, x + mapsize - y - 1 ); }
void getBuildingColours( const Tile& tile, int &c1, int &c2 );

void Minimap::Impl::getTerrainColours( const Tile& tile, bool staticTiles, int& c1, int& c2 )
{
  int rndData = tile.originalImgId();
  int num3 = rndData & 0x3;
  int num7 = rndData & 0x7;

  if( !staticTiles && tile.getFlag( Tile::tlTree ) )
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
  else if ( !staticTiles && tile.getFlag( Tile::tlRoad ))
  {
    c1 = colors->colour(minimap::Colors::MAP_ROAD, 0);
    c2 = colors->colour(minimap::Colors::MAP_ROAD, 1);
  }
  else if (tile.getFlag( Tile::tlMeadow ))
  {
    c1 = colors->colour(minimap::Colors::MAP_FERTILE1, num3);
    c2 = colors->colour(minimap::Colors::MAP_FERTILE2, num7);
  }
  else if ( !staticTiles && tile.getFlag( Tile::tlWall ))
  {
    c1 = colors->colour(minimap::Colors::MAP_WALL, 0);
    c2 = colors->colour(minimap::Colors::MAP_WALL, 1);
  }
  else // plain terrain
  {
    c1 = colors->colour(minimap::Colors::MAP_EMPTY1, num3);
    c2 = colors->colour(minimap::Colors::MAP_EMPTY2, num7);
  }

  c1 |= 0xff000000;
  c2 |= 0xff000000;
}

void Minimap::Impl::getTileColours(const Tile& tile, int &c1, int &c2)
{
  if( !gfx::tilemap::isValidLocation( tile.pos() ) )
  {
    c1 = c2 = 0xff000000;
    return;
  }

  if( tile.getFlag( Tile::tlOverlay ) )
  {
    getObjectColours(tile, c1, c2);
  }
  else
  {
    getTerrainColours(tile, false, c1, c2);
  }

  c1 |= 0xff000000;
  c2 |= 0xff000000;

#ifdef CAESARIA_PLATFORM_ANDROID
  c1 = NColor( c1 ).abgr();
  c2 = NColor( c2 ).abgr();
#endif
}

void Minimap::Impl::getObjectColours(const Tile& tile, int &c1, int &c2)
{
  OverlayPtr overlay = tile.overlay();

  if (overlay == NULL)
    return;

  object::Type type = overlay->type();  

  bool colorFound = false;
  switch(type)
  {
  case object::house:
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
      break;
    }
    colorFound = true;
  }
  break;
  case object::reservoir:
  case object::aqueduct:
  {
    c1 = colors->colour(minimap::Colors::MAP_AQUA, tile.originalImgId() & 0x3);
    c2 = colors->colour(minimap::Colors::MAP_AQUA, tile.originalImgId() & 0x7);
    colorFound = true;
  }
  break;

  case object::tree:
  {
    c1 = colors->colour(minimap::Colors::MAP_TREE1, 1);
    c2 = colors->colour(minimap::Colors::MAP_TREE2, 0);
    colorFound = true;
  }
  break;

  default: break;
  }

  if( !colorFound )
  {
    object::Group group = overlay->group();
    switch( group )
    {
      case object::group::military:
      {
        c1 = colors->colourA(DefaultColors::indianRed.color,1);
        c2 = colors->colourA(DefaultColors::indianRed.color,0);
        colorFound = true;
      }
      break;

      case object::group::food:
      {
        c1 = colors->colourA(DefaultColors::green.color,1);
        c2 = colors->colourA(DefaultColors::green.color,0);
        colorFound = true;
      }
      break;

      case object::group::industry:
      {
        c1 = colors->colourA(DefaultColors::brown.color,1);
        c2 = colors->colourA(DefaultColors::brown.color,0);
        colorFound = true;
      }
      break;

      case object::group::obtain:
      {
        c1 = colors->colourA(DefaultColors::sandyBrown.color,1);
        c2 = colors->colourA(DefaultColors::sandyBrown.color,0);
        colorFound = true;
      }
      break;

      case object::group::religion:
      {
        c1 = colors->colourA(DefaultColors::snow.color,1);
        c2 = colors->colourA(DefaultColors::snow.color,0);
        colorFound = true;
      }
      break;

      default: break;
    }
  }

  if( !colorFound )
  {

  }

  if( !colorFound )
  {
    switch (overlay->size().width())
    {
      case 1:
      {
        c1 = colors->colour(minimap::Colors::MAP_BUILDING, 0);
        c2 = colors->colour(minimap::Colors::MAP_BUILDING, 1);
      }
      break;
      default:
      {
        c1 = colors->colour(minimap::Colors::MAP_BUILDING, 0);
        c2 = colors->colour(minimap::Colors::MAP_BUILDING, 2);
      }
      break;
    }
  }

  c1 |= 0xff000000;
  c2 |= 0xff000000;

#ifdef CAESARIA_PLATFORM_ANDROID
  c1 = NColor( c1 ).abgr();
  c2 = NColor( c2 ).abgr();
#endif
}

void Minimap::Impl::drawObjectsMmap( Picture& canvas, bool clear, bool force )
{
  Tilemap& tilemap = city->tilemap();
  int mapsize = tilemap.size();

  int c1, c2;
  const OverlayList& ovs = city->overlays();
  int mmapWidth = objectsMap.width();
  int mmapHeight = objectsMap.height();
  unsigned int* pixelsObjects = canvas.lock();

  if( lastObjectsCount != ovs.size() || force )
  {
    if( clear )
      canvas.fill( DefaultColors::clear );
    lastObjectsCount = ovs.size();

    foreach( it, ovs )
    {
      OverlayPtr overlay = *it;
      const Tile& tile = overlay->tile();

      getObjectColours( tile, c1, c2);

      TilePos pos = overlay->pos();
      const Size& size = overlay->size();
      for( int i=0; i < size.width(); i++ )
      {
        for( int j=0; j < size.height(); j++ )
        {
          Point pnt = getBitmapCoordinates( pos.i() + i, pos.j() + j, mapsize);
          if( pnt.y() < 0 || pnt.x() < 0 || pnt.x() > mmapWidth-1 || pnt.y() > mmapHeight )
            continue;

          unsigned int* bufp32;
          bufp32 = pixelsObjects + pnt.y() * mmapWidth + pnt.x();
          *bufp32 = c1;
          *(bufp32+1) = c2;
        }
      }
    }

    canvas.unlock();
    canvas.update();
  }
}

void Minimap::Impl::drawWalkersMmap( Picture& canvas, bool clear )
{
  Tilemap& tilemap = city->tilemap();
  int mapsize = tilemap.size();

  // here we can draw anything
  int mmapWidth = objectsMap.width();
  int mmapHeight = objectsMap.height();

  const WalkerList& walkers = city->walkers();
  if( clear )
    canvas.fill( DefaultColors::clear );

  unsigned int* pixelsObjects = canvas.lock();

  foreach( i, walkers )
  {
    const TilePos& pos = (*i)->pos();

    NColor cl;
    if ((*i)->agressive() != 0)
    {

      if ((*i)->agressive() > 0)
      {
        cl = DefaultColors::red;
      }
      else
      {
        cl = DefaultColors::blue;
      }
    }
    else if( (*i)->type() == walker::immigrant )
    {
      cl = DefaultColors::green;
    }

    if (cl.color != 0)
    {
      Point pnt = getBitmapCoordinates(pos.i(), pos.j(), mapsize);
      //canvas.fill(cl, Rect(pnt, Size(2)));

      if( pnt.y() < 0 || pnt.x() < 0 || pnt.x() > mmapWidth-1 || pnt.y() > mmapHeight )
        continue;

      unsigned int* bufp32;
      bufp32 = pixelsObjects + pnt.y() * mmapWidth + pnt.x();
      *bufp32 = cl.color;
      *(bufp32+1) = cl.color;
    }
  }

  canvas.unlock();
  canvas.update();
}

void Minimap::Impl::updateImage()
{
  drawObjectsMmap( objectsMap, true, false );
  drawWalkersMmap( walkersMap, true );

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

void Minimap::Impl::initStaticMmap()
{
  Size size;
  Tilemap& tmap = city->tilemap();
  int mapSize = tmap.size();

  background.fill( 0xff000000, Rect() );
  background.update();

  size.setWidth( getBitmapCoordinates( mapSize-1, mapSize-1, mapSize ).x() );
  size.setHeight( getBitmapCoordinates( mapSize-1, 0, mapSize ).y() );

  landRockWaterMap = Picture( size, 0, true );
  objectsMap = Picture( size, 0, true );
  walkersMap = Picture( size, 0, true );
}

void Minimap::Impl::drawStaticMmap(Picture& canvas, bool clear)
{
  Tilemap& tmap = city->tilemap();
  int mapSize = tmap.size();

  if( clear )
    canvas.fill( 0xff000000, Rect() );

  int c1, c2;
  int mmapWidth = canvas.width();
  int mmapHeight = canvas.height();
  unsigned int* pixels = canvas.lock();

  for( int i = 0; i < mapSize; i++)
  {
    for (int j = 0; j < mapSize; j++)
    {
      const Tile& tile = tmap.at(i, j);

      Point pnt = getBitmapCoordinates(i, j, mapSize);
      getTerrainColours( tile, true, c1, c2);

      if( pnt.y() < 0 || pnt.x() < 0 || pnt.x() > mmapWidth-1 || pnt.y() > mmapHeight )
        continue;

      unsigned int* bufp32;
      bufp32 = pixels + pnt.y() * mmapWidth + pnt.x();
      *bufp32 = c1;
      *(bufp32+1) = c2;
    }
  }

  canvas.unlock();
  canvas.update();
}

/* end of helper functions */
namespace
{
  static const int kWhite  = 0xFFFFFF;
  static const int kYellow = 0xFFFF00;
}

void Minimap::draw(Engine& painter)
{
  if( !visible() )
    return;

  Tilemap& tilemap = _d->city->tilemap();
  int mapsize = tilemap.size();

  TilePos tpos = _d->camera->center();
  TilePos startPos = tpos;

  Point p = getBitmapCoordinates(startPos.i(), startPos.j(), mapsize);
  Point myCenter(width()/2,height()/2);

  painter.draw( _d->background, absoluteRect().lefttop(), &absoluteClippingRectRef() );
  painter.draw( _d->landRockWaterMap, absoluteRect().lefttop() + myCenter - p, &absoluteClippingRectRef() );
  painter.draw( _d->objectsMap, absoluteRect().lefttop() + myCenter - p, &absoluteClippingRectRef() );
  painter.draw( _d->walkersMap, absoluteRect().lefttop() + myCenter - p, &absoluteClippingRectRef() );

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
    Size minimapSize = _d->background.size();

    Point offset( minimapSize.width()/2 - _d->center.x(), minimapSize.height()/2 + _d->center.y() - mapsize*2 );
    clickPosition -= offset;
    TilePos tpos;
    tpos.setI( (clickPosition.x() + clickPosition.y() - mapsize + 1) / 2 );
    tpos.setJ( -clickPosition.y() + tpos.i() + mapsize - 1 );

    emit _d->onCenterChangeSignal( tpos );
    return true;
  }
  else if( sEventGui == event.EventType
           && guiButtonClicked == event.gui.type )
  {
    if( event.gui.caller == _d->btnZoomIn )
      emit _d->onZoomChangeSignal( -10 );
    else if( event.gui.caller == _d->btnZoomOut )
      emit _d->onZoomChangeSignal( +10 );
    return true;
  }

  return Widget::onEvent( event );
}

void Minimap::beforeDraw(Engine& painter)
{
  Widget::beforeDraw( painter );

  if( DateTime::elapsedTime() - _d->lastTimeUpdate > 250 )
  {
    _d->updateImage();
    _d->lastTimeUpdate = DateTime::elapsedTime();
  }
}

void Minimap::saveImage( const std::string& filename ) const
{
  Picture savePic( _d->landRockWaterMap.size(), 0, true );
  _d->drawStaticMmap( savePic, true );
  _d->drawObjectsMmap( savePic, false, true );
  _d->drawWalkersMmap( savePic, false );
  IMG_SavePNG( filename.c_str(), savePic.surface(), -1 );
}

Signal1<TilePos>& Minimap::onCenterChange() { return _d->onCenterChangeSignal; }
Signal1<int>& Minimap::onZoomChange() { return _d->onZoomChangeSignal; }

}//end namespace gui
