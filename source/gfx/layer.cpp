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
#include "gfx/helper.hpp"
#include "core/variant_map.hpp"
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
#include "core/utils.hpp"
#include "walker_debuginfo.hpp"
#include "core/timer.hpp"
#include "core/logger.hpp"
#include "animation_bank.hpp"

using namespace constants;

namespace gfx
{

namespace layer
{

class Layer::Impl
{
public:
  typedef std::set<int> AlwaysDrawObjects;

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
  Picture terraintPic;
  Layer::WalkerTypes vwalkers;
  PictureRef tilePosText;
  Font debugFont;
  AlwaysDrawObjects drObjects;

  int posMode;
public:
  void updateOutlineTexture( Tile* tile );
};

void Layer::registerTileForRendering(Tile& tile)
{
}

void Layer::renderUi(Engine& engine)
{
  __D_IMPL(_d,Layer)
  if( !_d->tooltipText.empty() )
  {
    engine.draw( *_d->tooltipPic, _d->lastCursorPos );
  }
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
  else if( event.EventType == sAppEvent )
  {
    switch( event.app.type )
    {
    case appWindowFocusEnter:
    case appWindowFocusLeave:
      DrawOptions::instance().setFlag( DrawOptions::windowActive, event.app.type == appWindowFocusEnter );
    break;

    default: break;
    }
  }
  else if( event.EventType == sEventKeyboard )
  {
    bool pressed = event.keyboard.pressed;
    int moveValue = math::clamp<int>( _d->camera->scrollSpeed()/10, 1, 99 );

    moveValue *= ( event.keyboard.shift ? 4 : 1 ) * (pressed ? 1 : 0);

    switch( event.keyboard.key )
    {
    case KEY_UP:    /*case KEY_KEY_W:*/ _d->camera->moveUp   ( moveValue ); break;
    case KEY_DOWN:  /*case KEY_KEY_S:*/ _d->camera->moveDown ( moveValue ); break;
    case KEY_RIGHT: /*case KEY_KEY_D:*/ _d->camera->moveRight( moveValue ); break;
    case KEY_LEFT:  /*case KEY_KEY_A:*/ _d->camera->moveLeft ( moveValue ); break;
    case KEY_ESCAPE: _setNextLayer( citylayer::simple ); break;    
    default: break;
    }
  }
}

TilesArray Layer::_getSelectedArea( TilePos startPos )
{
  __D_IMPL(_d,Layer)
  TilePos outStartPos, outStopPos;

  Tile* startTile = startPos.i() < 0
                      ? _d->camera->at( _d->startCursorPos, true ) // tile under the cursor (or NULL)
                      : _d->camera->at( startPos );
  Tile* stopTile  = _d->camera->at( _d->lastCursorPos, true );

  TilePos startPosTmp = startTile->epos();
  TilePos stopPosTmp  = stopTile->epos();

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

void Layer::drawWalkers( Engine& engine, const Tile& tile, const Point& camOffset )
{
  Pictures pics;
  const WalkerList& walkers = _city()->walkers( tile.pos() );
  const Layer::WalkerTypes& vWalkers = visibleTypes();

  bool viewAll = vWalkers.count( walker::all );
  foreach( w, walkers )
  {
    if( viewAll || vWalkers.count( (*w)->type() ) > 0 )
    {
      pics.clear();
      (*w)->getPictures( pics );
      engine.draw( pics, (*w)->mappos() + camOffset );
    }
  }
}

void Layer::_setTooltipText(const std::string& text)
{
  __D_IMPL(_d,Layer)
  if( _d->tooltipText != text )
  {
    Font font = Font::create( FONT_2 );
    _d->tooltipText = text;
    Size size = font.getTextSize( text );

    if( _d->tooltipPic.isNull() || (_d->tooltipPic->size() != size) )
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
  Point camOffset = _d->camera->offset();

  _camera()->startFrame();
  //FIRST PART: draw lands
  drawLands( engine, _d->camera );

  DrawOptions& opts = DrawOptions::instance();
  if( opts.isFlag( DrawOptions::shadowOverlay ) )
  {
    engine.setColorMask( 0x00ff0000, 0x0000ff00, 0x000000ff, 0xc0000000 );
  }
  // SECOND PART: draw all sprites, impassable land and buildings
  //int r0=0, r1=0, r2=0;
  foreach( it, visibleTiles )
  {
    Tile* tile = *it;
    int z = tile->epos().z();

    drawProminentTile( engine, *tile, camOffset, z, false );
    drawWalkers( engine, *tile, camOffset );
    drawWalkerOverlap( engine, *tile, camOffset, z );
  }

  engine.resetColorMask();

  if( opts.isFlag( DrawOptions::showPath ) )
  {
    WalkerList overDrawWalkers;

    const WalkerList& walkers = _city()->walkers( walker::all );
    foreach( it, walkers )
    {
      if( (*it)->getFlag( Walker::showPath ) )
      {
        overDrawWalkers << *it;
      }
      else
      {
        if( (*it)->getFlag( Walker::showDebugInfo ) )
          WalkerDebugInfo::showPath( *it, engine, _d->camera );
      }
    }

    foreach ( it, overDrawWalkers )
    {
      WalkerDebugInfo::showPath( *it, engine, _d->camera, DefaultColors::yellow );
    }
  }
}

void Layer::drawWalkerOverlap( Engine& engine, Tile& tile, const Point& offset, const int depth)
{
  Tile* master = tile.masterTile();

  // multi-tile: draw the master tile.
  // single-tile: draw current tile
  // and it is time to draw the master tile
  drawPass( engine, 0 == master ? tile : *master, offset, Renderer::overWalker );
}

const Layer::WalkerTypes& Layer::visibleTypes() const
{
  return _dfunc()->vwalkers;
}

void Layer::drawProminentTile( Engine& engine, Tile& tile, const Point& offset, const int depth, bool force)
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
  if( !master->rwd() && master == &tile )
  {
    drawTile( engine, *master, offset );
  }
}

void Layer::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  if( !tile.rwd() )
  {
    tile.setWasDrawn();
    if( DrawOptions::instance().isFlag( DrawOptions::oldGraphics ) )
    {
      drawPass( engine, tile, offset, Renderer::ground );
      drawPass( engine, tile, offset, Renderer::groundAnimation );
    }

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

void Layer::drawLands( Engine& engine, Camera* camera )
{
  const TilesArray& flatTiles = camera->flatTiles();
  const TilesArray& visibleTiles = camera->tiles();
  const Picture& terrainPic = _dfunc()->terraintPic;
  Point camOffset = camera->offset();

  // FIRST PART: draw all flat land (walkable/boatable)
  foreach( it, flatTiles )
  {
    drawPass( engine, **it, camOffset, Renderer::ground );
    drawPass( engine, **it, camOffset, Renderer::groundAnimation );

    drawTile( engine, **it, camOffset );
  }

  if( DrawOptions::instance().isFlag( DrawOptions::oldGraphics ) )
    return;

  foreach( it, visibleTiles )
  {
    Tile& t = **it;
    if( !t.isFlat() )
    {
      Tile* master = t.masterTile();
      master = (master == 0 ? &t : master);

      if( t.rov().isNull() )
      {
        // multi-tile: draw the master tile.
        // and it is time to draw the master tile
        if( !master->rwd() && master == &t )
        {            
          drawPass( engine, *master, camOffset, Renderer::ground );
          drawPass( engine, *master, camOffset, Renderer::groundAnimation );          
        }
      }
      else
      {
        Size size = t.rov()->size();
        if( !master->rwd() && master == &t )
        {
          if( size.width() > 1 )
          {
            for( int i=0; i < size.width(); i++ )
              for( int j=0; j < size.height(); j++ )
              {
                TilePos tpos = t.epos() + TilePos( i, j );
                Point mappos = Point( tilemap::cellSize().width() * ( tpos.i() + tpos.j() ),
                                      tilemap::cellSize().height() * ( tpos.i() - tpos.j() ) - 0 * tilemap::cellSize().height() );

                engine.draw( terrainPic, mappos + camOffset );
              }
          }
          else
          {
            drawPass( engine, *master, camOffset, Renderer::ground );
            drawPass( engine, *master, camOffset, Renderer::groundAnimation );
          }
        }
      }
    }
  }
}

void Layer::init( Point cursor )
{
  __D_IMPL(_d,Layer)
  _d->lastCursorPos = cursor;
  _d->startCursorPos = cursor;
  _d->nextLayer = type();
}

void Layer::beforeRender(Engine&) {}

void Layer::afterRender( Engine& engine)
{
  __D_IMPL(_d,Layer)
  Point cursorPos = engine.cursorPos();
  Size screenSize = engine.virtualSize();
  Point offset = _d->camera->offset();
  Point moveValue;

  //on edge cursor moving
  DrawOptions& opts = DrawOptions::instance();
  if( opts.isFlag( DrawOptions::windowActive | DrawOptions::borderMoving ) )
  {
    if( cursorPos.x() >= 0 && cursorPos.x() < 2 ) moveValue.rx() -= 1;
    else
      if( cursorPos.x() > screenSize.width() - 2 )
        moveValue.rx() += 1;

    if( cursorPos.y() >= 0 && cursorPos.y() < 2 ) moveValue.ry() += 1;
    else
      if( cursorPos.y() > screenSize.height() - 2 )
        moveValue.ry() -= 1;

    if( moveValue.x() != 0 || moveValue.y() != 0 )
    {
      _d->camera->move( moveValue.toPointF() );
    }
  }

  if( opts.isFlag( DrawOptions::drawGrid ) )
  {
    Tilemap& tmap = _d->city->tilemap();    
    int size = tmap.size();
    //Picture& screen = engine.screen();
    for( int k=0; k < size; k++ )
    {
      const Tile& tile = tmap.at( 0, k );
      const Tile& etile = tmap.at( size - 1, k );
      engine.drawLine( 0x80ff0000, tile.mappos() + offset, etile.mappos() + offset );

      const Tile& rtile = tmap.at( k, 0 );
      const Tile& ertile = tmap.at( k, size - 1 );
      engine.drawLine( 0x80ff0000, rtile.mappos() + offset, ertile.mappos() + offset );
    }

    /*std::string text;
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
          text = utils::format( 0xff, "(%d,%d)", i, j );
          //PictureDecorator::basicText( screen, rtile.mapPos() + offset + Point( 0, 0),text.c_str(), 0xffffffff );
          font.draw( screen, text, rtile.mappos() + offset + Point( 7, -7 ), false );
        }
      }
    break;
    }*/
  }

  if( opts.isFlag( DrawOptions::showRoads ) )
  {
    const Picture& grnPicture = Picture::load( "oc3_land", 1);

    TilesArray tiles = _d->city->tilemap().allTiles();
    foreach( it, tiles )
    {
      if( (*it)->getFlag( Tile::tlRoad ) )
        engine.draw( grnPicture , offset + (*it)->mappos() );
    }
  }

  if( opts.isFlag( DrawOptions::showWalkableTiles ) )
  {
    const Picture& grnPicture = Picture::load( "oc3_land", 1);

    TilesArray tiles = _d->city->tilemap().allTiles();
    foreach( it, tiles )
    {
      if( (*it)->isWalkable( true ) )
        engine.draw( grnPicture , offset + (*it)->mappos() );
    }
  }

  if( opts.isFlag( DrawOptions::showFlatTiles ) )
  {
    const Picture& grnPicture = Picture::load( "oc3_land", 1);

    TilesArray tiles = _d->city->tilemap().allTiles();
    foreach( it, tiles )
    {
      if( (*it)->isFlat() )
        engine.draw( grnPicture , offset + (*it)->mappos() );
    }
  }

  if( opts.isFlag( DrawOptions::showLockedTiles ) )
  {
    const Picture& grnPicture = Picture::load( "oc3_land", 2);

    TilesArray tiles = _d->city->tilemap().allTiles();
    foreach( it, tiles )
    {
      if( !(*it)->isWalkable( true ) )
        engine.draw( grnPicture , offset + (*it)->mappos() );
    }
  }

  if( _d->currentTile && opts.isFlag( DrawOptions::showObjectArea ) )
  {
    Tile* tile = _d->currentTile;
    Point pos = tile->mappos();
    int rwidth = tilemap::cellPicSize().width();
    int halfRWidth = rwidth / 2;
    Size size( math::clamp<int>( (tile->picture().width() + 2) / rwidth, 1, 10 ) );

    if( _d->tilePosText->isValid() )
    {
      _d->tilePosText->fill( 0x0 );
      _d->debugFont.draw( *_d->tilePosText, utils::format( 0xff, "%d,%d", tile->i(), tile->j() ), false, true );
    }

    TileOverlayPtr ov = tile->overlay();
    if( ov.isValid() )
    {
      size = ov->size();
      pos = ov->tile().mappos();


    }
    else if( tile->masterTile() != 0 )
    {
      pos = tile->masterTile()->mappos();
      size = (tile->masterTile()->picture().width() + 2) / rwidth;
    }

    pos += offset;
    engine.drawLine( DefaultColors::red, pos, pos + Point( halfRWidth, halfRWidth/2 ) * size.height() );
    engine.drawLine( DefaultColors::red, pos + Point( halfRWidth, halfRWidth/2 ) * size.width(), pos + Point( rwidth, 0) * size.height() );
    engine.drawLine( DefaultColors::red, pos + Point( rwidth, 0) * size.width(), pos + Point( halfRWidth, -halfRWidth/2 ) * size.height() );
    engine.drawLine( DefaultColors::red, pos + Point( halfRWidth, -halfRWidth/2 ) * size.width(), pos );
    engine.draw( *_d->tilePosText, pos );
  }
}

Layer::Layer( Camera* camera, PlayerCityPtr city )
  : __INIT_IMPL(Layer)
{
  __D_IMPL(_d,Layer)
  _d->camera = camera;
  _d->city = city;
  _d->debugFont = Font::create( FONT_1_WHITE );
  _d->currentTile = 0;

  _d->posMode = 0;
  _d->terraintPic = MetaDataHolder::randomPicture( objects::terrain, 1 );
  _d->tilePosText.init( Size( 240, 80 ) );
}

void Layer::_addWalkerType(walker::Type wtype)
{
  _dfunc()->vwalkers.insert( wtype );
}

void Layer::_fillVisibleObjects(int ltype)
{
  const VariantMap& vm = citylayer::Helper::getConfig( (citylayer::Type)ltype );
  VariantList vl = vm.get( "visibleObjects" ).toList();
  foreach( it, vl )
  {
    int ovType = MetaDataHolder::findType( it->toString() );
    if( ovType != objects::unknown )
      _dfunc()->drObjects.insert( ovType );
    }
}

Layer::WalkerTypes& Layer::_visibleWalkers() { return _dfunc()->vwalkers; }
bool Layer::_isVisibleObject(int ovType) { return _dfunc()->drObjects.count( ovType ) > 0; }
int Layer::nextLayer() const{ return _dfunc()->nextLayer; }
Camera* Layer::_camera(){ return _dfunc()->camera;}
PlayerCityPtr Layer::_city(){ return _dfunc()->city;}
void Layer::changeLayer(int type) {}
void Layer::_setNextLayer(int layer) { _dfunc()->nextLayer = layer;}
Layer::~Layer(){}
void Layer::_setLastCursorPos(Point pos){ _dfunc()->lastCursorPos = pos; }
void Layer::_setStartCursorPos(Point pos){ _dfunc()->startCursorPos = pos; }
Point Layer::_startCursorPos() const{ return _dfunc()->startCursorPos; }
Tile* Layer::_currentTile() const{ return _dfunc()->currentTile; }
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

DrawOptions& DrawOptions::instance()
{
  static DrawOptions inst;
  return inst;
}

}

}
