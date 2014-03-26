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

using namespace constants;

class Layer::Impl
{
public:
  typedef std::vector<Tile*> TileQueue;
  typedef std::map<Renderer::Pass, TileQueue> RenderQueue;

  Point lastCursorPos;
  Point startCursorPos;
  TilemapCamera* camera;
  PlayerCityPtr city;
  PictureRef tooltipPic;
  int nextLayer;
  std::string tooltipText;
  RenderQueue renderQueue;

  Picture footColumn;
  Picture bodyColumn;
  Picture headerColumn;
};

void Layer::registerTileForRendering(Tile& tile)
{
  if( tile.overlay() != 0 )
  {
    Renderer::PassQueue passQueue = tile.overlay()->getPassQueue();
    foreach( pass, passQueue )
    {
      _d->renderQueue[ *pass ].push_back( &tile );
    }
  }
}

void Layer::renderPass( GfxEngine& engine, Renderer::Pass pass )
{
  // building foregrounds and animations
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
    case KEY_UP:    _d->camera->moveUp   ( moveValue ); break;
    case KEY_DOWN:  _d->camera->moveDown ( moveValue ); break;
    case KEY_RIGHT: _d->camera->moveRight( moveValue ); break;
    case KEY_LEFT:  _d->camera->moveLeft ( moveValue ); break;
    default: break;
    }
  }
}

TilesArray Layer::_getSelectedArea()
{
  TilePos outStartPos, outStopPos;

  Tile* startTile = _d->camera->at( _d->startCursorPos, true );  // tile under the cursor (or NULL)
  Tile* stopTile  = _d->camera->at( _d->lastCursorPos, true );

  TilePos startPosTmp = startTile->pos();
  TilePos stopPosTmp  = stopTile->pos();

//  std::cout << "TilemapRenderer::_getSelectedArea" << " ";
//  std::cout << "(" << startPosTmp.getI() << " " << startPosTmp.getJ() << ") (" << stopPosTmp.getI() << " " << stopPosTmp.getJ() << ")" << std::endl;

  outStartPos = TilePos( std::min<int>( startPosTmp.i(), stopPosTmp.i() ), std::min<int>( startPosTmp.j(), stopPosTmp.j() ) );
  outStopPos  = TilePos( std::max<int>( startPosTmp.i(), stopPosTmp.i() ), std::max<int>( startPosTmp.j(), stopPosTmp.j() ) );

  return _getCity()->tilemap().getArea( outStartPos, outStopPos );
}

void Layer::drawTilePass( GfxEngine& engine, Tile& tile, Point offset, Renderer::Pass pass)
{
  Point screenPos = tile.mapPos() + offset;
  switch( pass )
  {
  case Renderer::ground: engine.drawPicture( tile.picture(), screenPos ); break;

  case Renderer::groundAnimation:
    if( tile.animation().isValid() )
    {
      engine.drawPicture( tile.animation().currentFrame(), screenPos );
    }
  break;

  default:
    if( tile.overlay().isNull() )
      return;

    const PicturesArray& pictures = tile.overlay()->getPictures( pass );

    for( PicturesArray::const_iterator it=pictures.begin(); it != pictures.end(); it++ )
    {
      engine.drawPicture( *it, screenPos );
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
    WalkerList foundWalkers = _getCity()->getWalkers( (walker::Type)*wtAct, pos );
    walkerList.insert( walkerList.end(), foundWalkers.begin(), foundWalkers.end() );
  }

  return walkerList;
}

void Layer::_drawWalkers( GfxEngine& engine, const Tile& tile, const Point& camOffset )
{
  PicturesArray pictureList;
  WalkerList walkers = _getVisibleWalkerList( getVisibleWalkers(), tile.pos() );

  foreach( w, walkers )
  {
    pictureList.clear();
    (*w)->getPictureList( pictureList );
    foreach( picRef, pictureList )
    {
      if( (*picRef).isValid() )
      {
        engine.drawPicture( *picRef, (*w)->getMappos() + camOffset );
      }
    }
  }
}

void Layer::_setTooltipText(std::string text)
{
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

void Layer::render( GfxEngine& engine)
{
  // center the map on the screen
  const TilesArray& visibleTiles = _d->camera->getTiles();
  Point camOffset = _d->camera->getOffset();

  _getCamera()->startFrame();

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

void Layer::drawTileW( GfxEngine& engine, Tile& tile, const Point& offset, const int depth)
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

void Layer::drawTileR( GfxEngine& engine, Tile& tile, const Point& offset, const int depth, bool force)
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

void Layer::drawArea(GfxEngine& engine, const TilesArray& area, Point offset, std::string resourceGroup, int tileId)
{
  if( area.empty() )
    return;

  Tile* baseTile = area.front();
  TileOverlayPtr overlay = baseTile->overlay();
  Picture *pic = NULL;
  int leftBorderAtI = baseTile->i();
  int rightBorderAtJ = overlay->size().height() - 1 + baseTile->j();
  for( TilesArray::const_iterator it=area.begin(); it != area.end(); it++ )
  {
    Tile* tile = *it;
    int tileBorders = ( tile->i() == leftBorderAtI ? 0 : OverlayPic::skipLeftBorder )
                      + ( tile->j() == rightBorderAtJ ? 0 : OverlayPic::skipRightBorder );
    pic = &Picture::load(resourceGroup, tileBorders + tileId);
    engine.drawPicture( *pic, tile->mapPos() + offset );
  }
}

void Layer::drawColumn(GfxEngine& engine, const Point& pos, const int percent)
{
  engine.drawPicture( _d->footColumn, pos + Point( 10, -21 ) );

  int roundPercent = ( percent / 10 ) * 10;

  for( int offsetY=10; offsetY < roundPercent; offsetY += 10 )
  {
    engine.drawPicture( _d->bodyColumn, pos - Point( -18, 8 + offsetY ) );
  }

  if( percent >= 10 )
  {
    engine.drawPicture( _d->headerColumn, pos - Point( -6, 25 + roundPercent ) );
  }
}

void Layer::init( Point cursor )
{
  _d->lastCursorPos = cursor;
  _d->startCursorPos = cursor;
  _d->nextLayer = getType();
}

void Layer::afterRender(GfxEngine& engine)
{
  if( !_d->tooltipText.empty() )
  {
    engine.drawPicture( *_d->tooltipPic, _d->lastCursorPos );
  }
}

Layer::Layer( TilemapCamera& camera, PlayerCityPtr city )
  : _d( new Impl )
{
  _d->camera = &camera;
  _d->city = city;
  _d->tooltipPic.reset( Picture::create( Size( 240, 80 ) ) );
}

void Layer::_loadColumnPicture(int picId)
{
  _d->footColumn = Picture::load( ResourceGroup::sprites, picId + 2 );
  _d->bodyColumn = Picture::load( ResourceGroup::sprites, picId + 1 );
  _d->headerColumn = Picture::load( ResourceGroup::sprites, picId );
}

int Layer::getNextLayer() const{ return _d->nextLayer; }
TilemapCamera* Layer::_getCamera(){  return _d->camera;}
PlayerCityPtr Layer::_getCity(){  return _d->city;}
void Layer::_setNextLayer(int layer) { _d->nextLayer = layer;}
Layer::~Layer(){}
void Layer::_setLastCursorPos(Point pos){   _d->lastCursorPos = pos; }
void Layer::_setStartCursorPos(Point pos){  _d->startCursorPos = pos; }
Point Layer::_getStartCursorPos() const{ return _d->startCursorPos; }
Point Layer::_getLastCursorPos() const {  return _d->lastCursorPos; }
