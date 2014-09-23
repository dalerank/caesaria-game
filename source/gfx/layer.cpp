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
#include "core/stringhelper.hpp"
#include "walker_debuginfo.hpp"
#include "core/timer.hpp"
#include "core/logger.hpp"

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
  Tile* currentTile;
  PlayerCityPtr city;
  PictureRef outline;
  bool needUpdateOutline;
  PictureRef tooltipPic;
  int nextLayer;
  std::string tooltipText;
  RenderQueue renderQueue;
  Layer::VisibleWalkers vwalkers;

  int posMode;

  Picture footColumn;
  Picture bodyColumn;
  Picture headerColumn;
public:
  void updateOutlineTexture( Tile* tile );
};

void Layer::registerTileForRendering(Tile& tile)
{
  __D_IMPL(_d,Layer)
  if( tile.rov() != 0 )
  {
    Renderer::PassQueue passQueue = tile.rov()->passQueue();
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
  Point offset = _d->camera->offset();
  foreach( tile, tiles )
  {
    drawPass( engine, *(*tile), offset, pass );
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

      Tile* selectedTile = _d->camera->at( _d->lastCursorPos, true );
      _d->needUpdateOutline = (selectedTile != _d->currentTile);
      _d->currentTile = selectedTile;
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
        events::GameEventPtr e = events::ShowTileInfo::create( tile->epos() );
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
    int moveValue = _d->camera->scrollSpeed() * ( event.keyboard.shift ? 4 : 1 ) * (pressed ? 1 : 0);

    switch( event.keyboard.key )
    {
    case KEY_UP:    case KEY_KEY_W: _d->camera->moveUp   ( moveValue ); break;
    case KEY_DOWN:  case KEY_KEY_S: _d->camera->moveDown ( moveValue ); break;
    case KEY_RIGHT: case KEY_KEY_D: _d->camera->moveRight( moveValue ); break;
    case KEY_LEFT:  case KEY_KEY_A: _d->camera->moveLeft ( moveValue ); break;
    case KEY_ESCAPE: _setNextLayer( citylayer::simple ); break;    
    default: break;
    }

    if( event.keyboard.control && event.keyboard.shift && event.keyboard.pressed )
    {
      LayerDrawOptions& opts = LayerDrawOptions::instance();
      switch( event.keyboard.key )
      {
      case KEY_KEY_1: opts.toggle( LayerDrawOptions::drawGrid ); break;
      case KEY_KEY_2: _d->posMode = (++_d->posMode) % 3; break;
      case KEY_KEY_3: opts.toggle( LayerDrawOptions::renderOverlay ); break;
      case KEY_KEY_4: opts.toggle( LayerDrawOptions::showPath ); break;
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

  outStartPos = TilePos( std::min<int>( startPosTmp.i(), stopPosTmp.i() ), std::min<int>( startPosTmp.j(), stopPosTmp.j() ) );
  outStopPos  = TilePos( std::max<int>( startPosTmp.i(), stopPosTmp.i() ), std::max<int>( startPosTmp.j(), stopPosTmp.j() ) );

  return _city()->tilemap().getArea( outStartPos, outStopPos );
}

void Layer::drawPass( Engine& engine, Tile& tile, const Point& offset, Renderer::Pass pass)
{
  Point screenPos = tile.mappos() + offset;
  switch( pass )
  {
  case Renderer::ground: engine.draw( tile.picture(), screenPos ); break;

  case Renderer::groundAnimation:
  {
    if( tile.animation().isValid() )
    {
      engine.draw( tile.animation().currentFrame(), screenPos );
    }
  }
  break;

  case Renderer::overlay:
  {
    if( tile.rov().isNull() )
      return;

    engine.draw( tile.rov()->picture(), screenPos );
  }
  break;

  default:
  {
    if( tile.rov().isNull() )
      return;

    engine.draw( tile.rov()->pictures( pass ), screenPos );
  }
  break;
  }
}

WalkerList Layer::_getVisibleWalkerList(const VisibleWalkers& aw, const TilePos& pos)
{
  Layer::VisibleWalkers vWalkers = visibleWalkers();

  WalkerList walkerList;
  foreach( wtAct, vWalkers )
  {
    WalkerList foundWalkers = _city()->walkers( (walker::Type)*wtAct, pos );
    walkerList.insert( walkerList.end(), foundWalkers.begin(), foundWalkers.end() );
  }

  return walkerList;
}

void Layer::drawWalkers( Engine& engine, const Tile& tile, const Point& camOffset )
{
  Pictures pics;
  WalkerList walkers = _getVisibleWalkerList( visibleWalkers(), tile.pos() );

  foreach( w, walkers )
  {
    pics.clear();
    (*w)->getPictures( pics );
    engine.draw( pics, (*w)->mappos() + camOffset );
  }
}

void Layer::_setTooltipText(const std::string& text)
{
  __D_IMPL(_d,Layer)
  if( !_d->tooltipPic.isNull() && (_d->tooltipText != text))
  {
    Font font = Font::create( FONT_2 );
    _d->tooltipText = text;
    Size size = font.getTextSize( text );

    if( _d->tooltipPic->size() != size )
    {
      _d->tooltipPic.reset( Picture::create( size, 0, true ) );
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
  const TilesArray& visibleTiles = _d->camera->tiles();
  const TilesArray& flatTiles = _d->camera->flatTiles();
  Point camOffset = _d->camera->offset();

  _camera()->startFrame();

  // FIRST PART: draw all flat land (walkable/boatable)
  Tile* tile;
  foreach( it, flatTiles )
  {
    drawTile( engine, **it, camOffset );
  }

  LayerDrawOptions& opts = LayerDrawOptions::instance();
  if( !opts.isFlag( LayerDrawOptions::renderOverlay ) )
  {
    engine.setColorMask( 0x00ff0000, 0x0000ff00, 0x000000ff, 0xc0000000 );
  }
  // SECOND PART: draw all sprites, impassable land and buildings
  //int r0=0, r1=0, r2=0;
  foreach( it, visibleTiles )
  {
    tile = *it;
    int z = tile->epos().z();

    drawTileR( engine, *tile, camOffset, z, false );
    drawWalkers( engine, *tile, camOffset );
    drawTileW( engine, *tile, camOffset, z );
  }

  engine.resetColorMask();

  if( opts.isFlag( LayerDrawOptions::showPath ) )
  {
    const WalkerList& walkers = _city()->walkers( walker::all );
    foreach( it, walkers )
    {
      if( (*it)->getFlag( Walker::showDebugInfo ) )
        WalkerDebugInfo::showPath( *it, engine, _d->camera );
    }
  }
}

void Layer::drawTileW( Engine& engine, Tile& tile, const Point& offset, const int depth)
{
  Tile* master = tile.masterTile();

  // multi-tile: draw the master tile.
  // single-tile: draw current tile
  // and it is time to draw the master tile
  drawPass( engine, 0 == master ? tile : *master, offset, Renderer::overWalker );
}

const Layer::VisibleWalkers& Layer::visibleWalkers() const
{
  return _dfunc()->vwalkers;
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
  if( master->epos().z() == depth && !master->rwd() )
  {
    drawTile( engine, *master, offset );
  }
}

void Layer::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  if( !tile.rwd() )
  {
    tile.setWasDrawn();
    drawPass( engine, tile, offset, Renderer::ground );
    drawPass( engine, tile, offset, Renderer::groundAnimation );

    if( tile.rov().isValid() )
    {
      registerTileForRendering( tile );
      drawPass( engine, tile, offset, Renderer::overlay );
      drawPass( engine, tile, offset, Renderer::overlayAnimation );
    }
  }
}

void Layer::drawArea(Engine& engine, const TilesArray& area, const Point &offset, const std::string &resourceGroup, int tileId)
{
  if( area.empty() )
    return;

  Tile* baseTile = area.front();
  TileOverlayPtr overlay = baseTile->overlay();
  int leftBorderAtI = baseTile->i();
  int rightBorderAtJ = overlay->size().height() - 1 + baseTile->j();
  foreach( it, area )
  {
    Tile* tile = *it;
    int tileBorders = ( tile->i() == leftBorderAtI ? 0 : OverlayPic::skipLeftBorder )
                      + ( tile->j() == rightBorderAtJ ? 0 : OverlayPic::skipRightBorder );
    Picture *pic = &Picture::load(resourceGroup, tileBorders + tileId);
    engine.draw( *pic, tile->mappos() + offset );
  }
}

void Layer::drawColumn( Engine& engine, const Point& pos, const int percent)
{
  __D_IMPL(_d,Layer)
  // Column made of tree base parts and contains maximum 10 parts.
  // Header (10)
  // Body (10, max 8 pieces)
  // Foot (10)
  //
  // In original game fire colomn may be in one of 12 (?) states: none, f, f+h, f+b+h, f+2b+h, ... f+8b+h


  int clamped = math::clamp(percent, 0, 100);
  int rounded = (clamped / 10) * 10;
  // [0,  9] -> 0
  // [10,19] -> 10
  // ...
  // [80,89] -> 80
  // [90,99] -> 90
  // [100] -> 100
  // rounded == 0 -> nothing
  // rounded == 10 -> header + footer
  // rounded == 20 -> header + body + footer

  if (percent == 0)
  {
    // Nothing to draw.
    return;
  }

  engine.draw( _d->footColumn, pos + Point( 10, -21 ) );

  if(rounded > 10)
  {
    for( int offsetY=7; offsetY < rounded; offsetY += 10 )
    {
      engine.draw( _d->bodyColumn, pos - Point( -18, 8 + offsetY ) );
    }

    engine.draw(_d->headerColumn, pos - Point(-6, 25 + rounded));
  }
}

void Layer::init( Point cursor )
{
  __D_IMPL(_d,Layer)
  _d->lastCursorPos = cursor;
  _d->startCursorPos = cursor;
  _d->nextLayer = type();
}

 void Layer::beforeRender(Engine&){}

void Layer::afterRender( Engine& engine)
{
  __D_IMPL(_d,Layer)
  Point cursorPos = engine.cursorPos();
  Size screenSize = engine.screenSize();
  Point offset = _d->camera->offset();
  Point moveValue;

  //on edge cursor moving
  if( cursorPos.x() >= 0 && cursorPos.x() < 2 ) moveValue.rx() -= 1;
  else if( cursorPos.x() > screenSize.width() - 2 && cursorPos.x() <= screenSize.width() ) moveValue.rx() += 1;
  if( cursorPos.y() >= 0 && cursorPos.y() < 2 ) moveValue.ry() += 1;
  else if( cursorPos.y() > screenSize.height() - 2 && cursorPos.y() <= screenSize.height() ) moveValue.ry() -= 1;

  if( moveValue.x() != 0 || moveValue.y() != 0 )
  {
    _d->camera->move( moveValue.toPointF() );
  }

  if( !_d->tooltipText.empty() )
  {
    engine.draw( *_d->tooltipPic, _d->lastCursorPos );
  }  

  LayerDrawOptions& opts = LayerDrawOptions::instance();
  if( opts.isFlag( LayerDrawOptions::drawGrid ) )
  {
    Tilemap& tmap = _d->city->tilemap();    
    int size = tmap.size();
    Picture& screen = engine.screen();
    for( int k=0; k < size; k++ )
    {
      const Tile& tile = tmap.at( 0, k );
      const Tile& etile = tmap.at( size - 1, k );
      engine.drawLine( 0x80ff0000, tile.mappos() + offset, etile.mappos() + offset );

      const Tile& rtile = tmap.at( k, 0 );
      const Tile& ertile = tmap.at( k, size - 1 );
      engine.drawLine( 0x80ff0000, rtile.mappos() + offset, ertile.mappos() + offset );
    }

    std::string text;
    Font font = Font::create( FONT_0 );
    font.setColor( 0x80ffffff );
    switch( _d->posMode )
    {
    case 1:
      for( int i=0; i < size; i+=2 )
      {
        for( int j=0; j < size; j+=2 )
        {
          const Tile& rtile = tmap.at( i, j );
          text = StringHelper::format( 0xff, "(%d,%d)", i, j );
          //PictureDecorator::basicText( screen, rtile.mapPos() + offset + Point( 0, 0),text.c_str(), 0xffffffff );
          font.draw( screen, text, rtile.mappos() + offset + Point( 7, -7 ), false );
        }
      }
    break;
    }
  }

  if( _d->currentTile )
  {
    Point pos = _d->currentTile->mappos();
    int size = (_d->currentTile->picture().width() + 2) / 60;

    TileOverlayPtr ov = _d->currentTile->overlay();
    if( ov.isValid() )
    {
      size = ov->size().width();
      pos = ov->tile().mappos();
    }
    else if( _d->currentTile->masterTile() != 0 )
    {
      pos = _d->currentTile->masterTile()->mappos();
    }

    pos += offset;
    engine.drawLine( DefaultColors::red, pos, pos + Point( 29, 15 ) * size );
    engine.drawLine( DefaultColors::red, pos + Point( 29, 15 ) * size, pos + Point( 58, 0) * size );
    engine.drawLine( DefaultColors::red, pos + Point( 58, 0) * size, pos + Point( 29, -15 ) * size );
    engine.drawLine( DefaultColors::red, pos + Point( 29, -15 ) * size, pos );
  }
}

Layer::Layer( Camera* camera, PlayerCityPtr city )
  : __INIT_IMPL(Layer)
{
  __D_IMPL(_d,Layer)
  _d->camera = camera;
  _d->city = city;
  _d->currentTile = 0;

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

void Layer::_addWalkerType(walker::Type wtype)
{
  _dfunc()->vwalkers.insert( wtype );
}

int Layer::nextLayer() const{ return _dfunc()->nextLayer; }
Camera* Layer::_camera(){ return _dfunc()->camera;}
PlayerCityPtr Layer::_city(){ return _dfunc()->city;}
void Layer::_setNextLayer(int layer) { _dfunc()->nextLayer = layer;}
Layer::~Layer(){}
void Layer::_setLastCursorPos(Point pos){ _dfunc()->lastCursorPos = pos; }
void Layer::_setStartCursorPos(Point pos){ _dfunc()->startCursorPos = pos; }
Point Layer::_startCursorPos() const{ return _dfunc()->startCursorPos; }
Tile *Layer::_currentTile() const{ return _dfunc()->currentTile; }
Point Layer::_lastCursorPos() const { return _dfunc()->lastCursorPos; }

void Layer::Impl::updateOutlineTexture( Tile* tile )
{
  if( !needUpdateOutline )
    return;

 /* if( tile && tile->overlay().isValid() )
  {
    const Picture& pic = tile->overlay()->picture();

    outline.reset( Picture::create( pic.size(), 0, true ) );

  } */
}

LayerDrawOptions &LayerDrawOptions::instance()
{
  static LayerDrawOptions inst;
  return inst;
}

}
