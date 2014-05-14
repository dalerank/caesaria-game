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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "layer.hpp"
#include "tileoverlay.hpp"
#include "core/foreach.hpp"
#include "game/resourcegroup.hpp"
#include "tilesarray.hpp"
#include "core/event.hpp"
#include "city_renderer.hpp"
#include "events/showtileinfo.hpp"
#include "walker/constants.hpp"
#include "walker/walker.hpp"
#include "tilemap_camera.hpp"
#include "city/city.hpp"
#include "core/font.hpp"
#include "layerconstants.hpp"
#include "decorator.hpp"
#include "sdl_engine.hpp"
#include "core/stringhelper.hpp"

using namespace constants;

namespace gfx
{

class Layer::Impl
{
public:
  typedef std::vector<Tile*> TileQueue;
  typedef std::map<Renderer::Pass, TileQueue> RenderQueue;

  Point lastCursorPos;
  Point startCursorPos;
  Camera* camera;
  PlayerCityPtr city;
  PictureRef tooltipPic;
  int nextLayer;
  std::string tooltipText;
  RenderQueue renderQueue;

  bool drawGrid;
  int posMode;

  Picture footColumn;
  Picture bodyColumn;
  Picture headerColumn;
};

void Layer::registerTileForRendering(Tile& tile)
{
  __D_IMPL(_d,Layer)
  if( tile.overlay() != 0 )
  {
    Renderer::PassQueue passQueue = tile.overlay()->getPassQueue();
    foreach( pass, passQueue )
    {
      _d->renderQueue[ *pass ].push_back( &tile );
    }
  }
}

void Layer::renderPass( Engine& engine, Renderer::Pass pass )
{
  // building foregrounds and animations
  __D_IMPL(_d,Layer)
  Impl::TileQueue& tiles = _d->renderQueue[ pass ];
  Point offset = _d->camera->getOffset();
  foreach( tile, tiles )
  {
    drawTilePass( engine, *(*tile), offset, pass );
  }

  tiles.clear();
}

void Layer::handleEvent(NEvent& event)
{
  __D_IMPL(_d,Layer)
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      Point savePos = _d->lastCursorPos;
      _d->lastCursorPos = event.mouse.pos();
      if( !event.mouse.isLeftPressed() || _d->startCursorPos.x() < 0 )
      {
        _d->startCursorPos = _d->lastCursorPos;
      }

      if( event.mouse.isLeftPressed() )
      {
        Point delta = _d->lastCursorPos - savePos;
        _d->camera->move( PointF( -delta.x() * 0.1, delta.y() * 0.1 ) );
      }
    }
    break;

    case mouseLbtnPressed:
    {
      _d->startCursorPos = _d->lastCursorPos;
    }
    break;

    case mouseLbtnRelease:            // left button
    {
      Tile* tile = _d->camera->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      if( tile == 0 )
      {
        break;
      }

      if( event.mouse.control )
      {
        _d->camera->setCenter( tile->pos() );
        _d->city->setCameraPos( tile->pos() );
      }

      _d->startCursorPos = _d->lastCursorPos;
    }
    break;

    case mouseRbtnRelease:
    {
      Tile* tile = _d->camera->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      if( tile )
      {
        events::GameEventPtr e = events::ShowTileInfo::create( tile->pos() );
        e->dispatch();
      }
    }
    break;

    default:
    break;
    }
  }

  if( event.EventType == sEventKeyboard )
  {
    bool pressed = event.keyboard.pressed;
    int moveValue = _d->camera->getScrollSpeed() * ( event.keyboard.shift ? 4 : 1 ) * (pressed ? 1 : 0);

    switch( event.keyboard.key )
    {
    case KEY_UP: case KEY_KEY_W: _d->camera->moveUp   ( moveValue ); break;
    case KEY_DOWN: case KEY_KEY_S: _d->camera->moveDown ( moveValue ); break;
    case KEY_RIGHT: case KEY_KEY_D: _d->camera->moveRight( moveValue ); break;
    case KEY_LEFT:  case KEY_KEY_A: _d->camera->moveLeft ( moveValue ); break;
    case KEY_ESCAPE: _setNextLayer( citylayer::simple ); break;    
    default: break;
    }

    if( event.keyboard.control && event.keyboard.shift && event.keyboard.pressed )
    {
      switch( event.keyboard.key )
      {
      case KEY_KEY_1: _d->drawGrid = !_d->drawGrid; break;
      case KEY_KEY_2: _d->posMode = (++_d->posMode) % 3;
      default: break;
      }
    }
  }
}

TilesArray Layer::_getSelectedArea()
{
  __D_IMPL(_d,Layer)
  TilePos outStartPos, outStopPos;

  Tile* startTile = _d->camera->at( _d->startCursorPos, true );  // tile under the cursor (or NULL)
  Tile* stopTile  = _d->camera->at( _d->lastCursorPos, true );

  TilePos startPosTmp = startTile->pos();
  TilePos stopPosTmp  = stopTile->pos();

//  std::cout << "TilemapRenderer::_getSelectedArea" << " ";
//  std::cout << "(" << startPosTmp.getI() << " " << startPosTmp.getJ() << ") (" << stopPosTmp.getI() << " " << stopPosTmp.getJ() << ")" << std::endl;

  outStartPos = TilePos( std::min<int>( startPosTmp.i(), stopPosTmp.i() ), std::min<int>( startPosTmp.j(), stopPosTmp.j() ) );
  outStopPos  = TilePos( std::max<int>( startPosTmp.i(), stopPosTmp.i() ), std::max<int>( startPosTmp.j(), stopPosTmp.j() ) );

  return _city()->tilemap().getArea( outStartPos, outStopPos );
}

void Layer::drawTilePass( Engine& engine, Tile& tile, Point offset, Renderer::Pass pass)
{
  Point screenPos = tile.mapPos() + offset;
  switch( pass )
  {
  case Renderer::ground: engine.draw( tile.picture(), screenPos ); break;

  case Renderer::groundAnimation:
    if( tile.animation().isValid() )
    {
      engine.draw( tile.animation().currentFrame(), screenPos );
    }
  break;

  default:
    if( tile.overlay().isNull() )
      return;

    const Pictures& pictures = tile.overlay()->getPictures( pass );

    for( Pictures::const_iterator it=pictures.begin(); it != pictures.end(); ++it )
    {
      engine.draw( *it, screenPos );
    }
  break;
  }
}

WalkerList Layer::_getVisibleWalkerList(const VisibleWalkers& aw, const TilePos& pos)
{
  Layer::VisibleWalkers visibleWalkers = getVisibleWalkers();

  WalkerList walkerList;
  foreach( wtAct, visibleWalkers )
  {
    WalkerList foundWalkers = _city()->getWalkers( (walker::Type)*wtAct, pos );
    walkerList.insert( walkerList.end(), foundWalkers.begin(), foundWalkers.end() );
  }

  return walkerList;
}

void Layer::_drawWalkers( Engine& engine, const Tile& tile, const Point& camOffset )
{
  Pictures pictureList;
  WalkerList walkers = _getVisibleWalkerList( getVisibleWalkers(), tile.pos() );

  foreach( w, walkers )
  {
    pictureList.clear();
    (*w)->getPictureList( pictureList );
    foreach( picRef, pictureList )
    {
      if( (*picRef).isValid() )
      {
        engine.draw( *picRef, (*w)->screenpos() + camOffset );
      }
    }
  }
}

void Layer::_setTooltipText(std::string text)
{
  __D_IMPL(_d,Layer)
  if( !_d->tooltipPic.isNull() && (_d->tooltipText != text))
  {
    Font font = Font::create( FONT_2 );
    _d->tooltipText = text;
    Size size = font.getSize( text );

    if( _d->tooltipPic->size() != size )
    {
      _d->tooltipPic.reset( Picture::create( size ) );
    }

    _d->tooltipPic->fill( 0x00000000, Rect( Point( 0, 0 ), _d->tooltipPic->size() ) );
    _d->tooltipPic->fill( 0xff000000, Rect( Point( 0, 0 ), size ) );
    _d->tooltipPic->fill( 0xffffffff, Rect( Point( 1, 1 ), size - Size( 2, 2 ) ) );
    font.draw( *_d->tooltipPic, text, Point(), false );
  }
}

void Layer::render( Engine& engine)
{
  __D_IMPL(_d,Layer)
  // center the map on the screen
  const TilesArray& visibleTiles = _d->camera->getTiles();
  Point camOffset = _d->camera->getOffset();

  _camera()->startFrame();

  // FIRST PART: draw all flat land (walkable/boatable)
  foreach( it, visibleTiles )
  {
    Tile* tile = *it;
    Tile* master = tile->masterTile();

    if( !tile->isFlat() )
      continue;

    if( master==NULL )
    {
      // single-tile
      drawTile( engine, *tile, camOffset );
    }
    else
    {
      // multi-tile: draw the master tile.
      if( !master->getFlag( Tile::wasDrawn ) )
        drawTile( engine, *master, camOffset );
    }
  }

  // SECOND PART: draw all sprites, impassable land and buildings
  foreach( it, visibleTiles )
  {
    Tile* tile = *it;
    int z = tile->pos().z();

    drawTileR( engine, *tile, camOffset, z, false );

    _drawWalkers( engine, *tile, camOffset );

    drawTileW( engine, *tile, camOffset, z );
  }
}

void Layer::drawTileW( Engine& engine, Tile& tile, const Point& offset, const int depth)
{
  Tile* master = tile.masterTile();

  if( 0 == master )    // single-tile
  {
    drawTilePass( engine, tile, offset, Renderer::overWalker  );
    return;
  }

  // multi-tile: draw the master tile.
  // and it is time to draw the master tile
  //if( master->pos().z() == depth )
  {
    drawTilePass( engine, *master, offset, Renderer::overWalker );
  }
}

void Layer::drawTileR( Engine& engine, Tile& tile, const Point& offset, const int depth, bool force)
{
  if( tile.isFlat() && !force )
  {
    return;  // tile has already been drawn!
  }

  Tile* master = tile.masterTile();

  if( 0 == master )    // single-tile
  {
    drawTile( engine, tile, offset );
    return;
  }

  // multi-tile: draw the master tile.
  // and it is time to draw the master tile
  if( master->pos().z() == depth && !master->getFlag( Tile::wasDrawn ) )
  {
    drawTile( engine, *master, offset );
  }
}

void Layer::drawArea( Engine& engine, const TilesArray& area, Point offset, std::string resourceGroup, int tileId)
{
  if( area.empty() )
    return;

  Tile* baseTile = area.front();
  TileOverlayPtr overlay = baseTile->overlay();
  int leftBorderAtI = baseTile->i();
  int rightBorderAtJ = overlay->size().height() - 1 + baseTile->j();
  for( TilesArray::const_iterator it=area.begin(); it != area.end(); ++it )
  {
    Tile* tile = *it;
    int tileBorders = ( tile->i() == leftBorderAtI ? 0 : OverlayPic::skipLeftBorder )
                      + ( tile->j() == rightBorderAtJ ? 0 : OverlayPic::skipRightBorder );
    Picture *pic = &Picture::load(resourceGroup, tileBorders + tileId);
    engine.draw( *pic, tile->mapPos() + offset );
  }
}

void Layer::drawColumn( Engine& engine, const Point& pos, const int percent)
{
  __D_IMPL(_d,Layer)
  engine.draw( _d->footColumn, pos + Point( 10, -21 ) );

  int roundPercent = ( percent / 10 ) * 10;

  for( int offsetY=10; offsetY < roundPercent; offsetY += 10 )
  {
    engine.draw( _d->bodyColumn, pos - Point( -18, 8 + offsetY ) );
  }

  if( percent >= 10 )
  {
    engine.draw( _d->headerColumn, pos - Point( -6, 25 + roundPercent ) );
  }
}

void Layer::init( Point cursor )
{
  __D_IMPL(_d,Layer)
  _d->lastCursorPos = cursor;
  _d->startCursorPos = cursor;
  _d->nextLayer = getType();
}

void Layer::afterRender( Engine& engine)
{
  __D_IMPL(_d,Layer)
  if( !_d->tooltipText.empty() )
  {
    engine.draw( *_d->tooltipPic, _d->lastCursorPos );
  }

  if( _d->drawGrid )
  {
    Tilemap& tmap = _d->city->tilemap();
    Point offset = _d->camera->getOffset();
    int size = tmap.size();
    SdlEngine* painter = static_cast< SdlEngine* >( &engine );
    Picture& screen = painter->getScreen();
    for( int k=0; k < size; k++ )
    {
      const Tile& tile = tmap.at( 0, k );
      const Tile& etile = tmap.at( size - 1, k );
      PictureDecorator::drawLine( screen, tile.mapPos() + offset, etile.mapPos() + offset, 0xff0000ff);

      const Tile& rtile = tmap.at( k, 0 );
      const Tile& ertile = tmap.at( k, size - 1 );
      PictureDecorator::drawLine( screen, rtile.mapPos() + offset, ertile.mapPos() + offset, 0xff0000ff );
    }

    std::string text;
    Font font = Font::create( FONT_0 );
    font.setColor( 0xffffffff );
    switch( _d->posMode )
    {
    case 1:
      for( int i=0; i < size; i++ )
      {
        for( int j=0; j < size; j++ )
        {
          const Tile& rtile = tmap.at( i, j );
          text = StringHelper::format( 0xff, "(%d,%d)", i, j );
          //PictureDecorator::basicText( screen, rtile.mapPos() + offset + Point( 0, 0),text.c_str(), 0xffffffff );
          font.draw( screen, text, rtile.mapPos() + offset + Point( 7, -7 ), false );
        }
      }
    break;
    }
  }
}

Layer::Layer( Camera* camera, PlayerCityPtr city )
  : __INIT_IMPL(Layer)
{
  __D_IMPL(_d,Layer)
  _d->camera = camera;
  _d->city = city;
  _d->drawGrid = false;
  _d->posMode = 0;
  _d->tooltipPic.reset( Picture::create( Size( 240, 80 ) ) );
}

void Layer::_loadColumnPicture(int picId)
{
  __D_IMPL(_d,Layer)
  _d->footColumn = Picture::load( ResourceGroup::sprites, picId + 2 );
  _d->bodyColumn = Picture::load( ResourceGroup::sprites, picId + 1 );
  _d->headerColumn = Picture::load( ResourceGroup::sprites, picId );
}

int Layer::getNextLayer() const{ return _dfunc()->nextLayer; }
Camera* Layer::_camera(){ return _dfunc()->camera;}
PlayerCityPtr Layer::_city(){ return _dfunc()->city;}
void Layer::_setNextLayer(int layer) { _dfunc()->nextLayer = layer;}
Layer::~Layer(){}
void Layer::_setLastCursorPos(Point pos){ _dfunc()->lastCursorPos = pos; }
void Layer::_setStartCursorPos(Point pos){ _dfunc()->startCursorPos = pos; }
Point Layer::_startCursorPos() const{ return _dfunc()->startCursorPos; }
Point Layer::_lastCursorPos() const { return _dfunc()->lastCursorPos; }

}
