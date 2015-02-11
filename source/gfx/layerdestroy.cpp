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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "layerdestroy.hpp"
#include "layerconstants.hpp"
#include "events/event.hpp"
#include "walker/constants.hpp"
#include "walker/walker.hpp"
#include "core/foreach.hpp"
#include "tilemap_camera.hpp"
#include "city/city.hpp"
#include "gfx/helper.hpp"
#include "core/event.hpp"
#include "events/clearland.hpp"
#include "core/utils.hpp"
#include "objects/metadata.hpp"
#include "events/fundissue.hpp"
#include "city/funds.hpp"
#include "core/font.hpp"
#include "game/settings.hpp"

using namespace constants;

namespace gfx
{

namespace layer
{

class Destroy::Impl
{
public:
  Picture shovelPic;
  Picture clearPic;
  PictureRef textPic;
  unsigned int savesum, money4destroy;
  TilePos startTilePos;
  Font textFont;
};

void Destroy::_clearAll()
{
  TilesArray tiles4clear = _getSelectedArea( _d->startTilePos );
  foreach( tile, tiles4clear )
  {
    events::GameEventPtr event = events::ClearTile::create( (*tile)->epos() );
    event->dispatch();
  }
}

unsigned int Destroy::_checkMoney4destroy(const Tile& tile)
{
  TileOverlayPtr overlay = tile.overlay();
  if( overlay.isValid() )
  {
    const MetaData& mdata = MetaDataHolder::getData( overlay->type() );
    return mdata.getOption( MetaDataOptions::cost ).toInt() / 2;
  }

  if( tile.getFlag( Tile::tlTree ) ) return 6;
  if( tile.getFlag( Tile::tlRoad) ) return 4;

  return 0;
}

void Destroy::render( Engine& engine )
{
  // center the map on the screen
  Point cameraOffset = _camera()->offset();

  const TilesArray& visibleTiles = _camera()->tiles();
  const TilesArray& flatTiles = _camera()->flatTiles();

  _camera()->startFrame();

  Tilemap& tmap = _city()->tilemap();

  std::set<int> hashDestroyArea;
  TilesArray destroyArea = _getSelectedArea( _d->startTilePos );

  //create list of destroy tiles add full area building if some of it tile constain in destroy area
  _d->savesum = _d->money4destroy;
  _d->money4destroy = 0;
  foreach( it, destroyArea)
  {
    Tile* tile = *it;
    hashDestroyArea.insert( tile::hash(tile->epos()));

    TileOverlayPtr overlay = tile->overlay();
    if( overlay.isValid() )
    {
      TilesArray overlayArea = tmap.getArea( overlay->tile().epos(), overlay->size() );
      foreach( ovelayTile, overlayArea )
      {
        hashDestroyArea.insert( tile::hash((*ovelayTile)->epos()));
      }
    }

    _d->money4destroy += _checkMoney4destroy( *tile );
  }

  // FIRST PART: draw all flat land (walkable/boatable)  
  foreach( it, flatTiles )
  {
    Tile* tile = *it;
    Tile* master = tile->masterTile();

    tile = master == 0 ? tile : master;

    if( !tile->rwd() )
    {
      int tilePosHash = tile::hash(tile->epos());
      if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() &&
          tile->getFlag( Tile::isDestructible ) )
      {
        engine.setColorMask( 0x00ff0000, 0, 0, 0xff000000 );
      }

      drawPass( engine, *tile, cameraOffset, Renderer::ground );
      drawPass( engine, *tile, cameraOffset, Renderer::groundAnimation );

      drawTile( engine, *tile, cameraOffset );

      engine.resetColorMask();
    }
  }

  foreach( it, visibleTiles )
  {
    Tile& t = **it;
    if( !t.isFlat() )
    {
      Tile* master = t.masterTile();
      master = master == 0 ? &t : master;
      int tilePosHash = tile::hash( master->epos());

      if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() &&
          t.getFlag( Tile::isDestructible ) )
      {
        engine.setColorMask( 0x00ff0000, 0, 0, 0xff000000 );
      }

      if( t.rov().isNull() )
      {
        // multi-tile: draw the master tile.
        // and it is time to draw the master tile
        if( !master->rwd() && master == &t )
        {
          drawPass( engine, *master, cameraOffset, Renderer::ground );
          drawPass( engine, *master, cameraOffset, Renderer::groundAnimation );
        }
      }
      else
      {
        Size size = t.rov()->size();
        const Picture& terrainPic = master->picture();

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

                engine.draw( terrainPic, mappos + cameraOffset );
              }
          }
          else
          {
            drawPass( engine, *master, cameraOffset, Renderer::ground );
            drawPass( engine, *master, cameraOffset, Renderer::groundAnimation );
          }
        }
      }

      engine.resetColorMask();
    }
  }

  // SECOND PART: draw all sprites, impassable land and buildings
  foreach( it, visibleTiles )
  {
    Tile* tile = *it;
    int z = tile->epos().z();

    int tilePosHash = tile::hash(tile->epos());
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      if( tile->getFlag( Tile::isDestructible ) )
      {
        engine.setColorMask( 0x00ff0000, 0, 0, 0xff000000 );
      }
    }

    drawProminentTile( engine, *tile, cameraOffset, z, false );

    drawWalkers( engine, *tile, cameraOffset );
    engine.resetColorMask();
  }
}

void Destroy::renderUi(Engine &engine)
{
  if( _d->savesum != _d->money4destroy )
  {
    _d->textPic->fill( 0x0, Rect() );
    _d->textFont.setColor( 0xffff0000 );
    _d->textFont.draw( *_d->textPic, utils::i2str( _d->money4destroy ) + " Dn", Point() );
  }

  engine.draw( _d->shovelPic, engine.cursorPos() - Point( 5, _d->shovelPic.height() ) );
  engine.draw( *_d->textPic, engine.cursorPos() + Point( 10, 10 ));
}

void Destroy::init(Point cursor)
{
  Layer::init( cursor );
  _setLastCursorPos( cursor );
  _setStartCursorPos( cursor );
  _d->startTilePos = TilePos( -1, -1 );
}

void Destroy::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      _setLastCursorPos( event.mouse.pos() );
      if( !event.mouse.isLeftPressed() || _startCursorPos().x() < 0 )
      {
        _setStartCursorPos( _lastCursorPos() );

       Tile* tile = _camera()->at( _lastCursorPos(), true );
        _d->startTilePos = tile ? tile->epos() : TilePos( -1, -1 );
      }
    }
    break;

    case mouseLbtnPressed:
    {
      _setStartCursorPos( _lastCursorPos() );
    }
    break;

    case mouseLbtnRelease:            // left button
    {
      _clearAll();
      _setStartCursorPos( _lastCursorPos() );
      events::GameEventPtr e = events::FundIssueEvent::create( city::Funds::buildConstruction, -_d->money4destroy );
      e->dispatch();      
    }
    break;

    case mouseRbtnRelease:
    {
      _setNextLayer( citylayer::simple );
      _city()->setOption( PlayerCity::updateTiles, 1 );
    }
    break;

    default:
    break;
    }
  }

  if( event.EventType == sEventKeyboard )
  {
    bool pressed = event.keyboard.pressed;
    int moveValue = _camera()->scrollSpeed() * ( event.keyboard.shift ? 4 : 1 ) * (pressed ? 1 : 0);

    switch( event.keyboard.key )
    {
    case KEY_UP:    _camera()->moveUp   ( moveValue ); break;
    case KEY_DOWN:  _camera()->moveDown ( moveValue ); break;
    case KEY_RIGHT: _camera()->moveRight( moveValue ); break;
    case KEY_LEFT:  _camera()->moveLeft ( moveValue ); break;
    case KEY_ESCAPE: _setNextLayer( citylayer::simple ); break;
    default: break;
    }
  }
}

int Destroy::type() const {  return citylayer::destroyd; }

void Destroy::drawTile(Engine& engine, Tile& tile, const Point& offset )
{
  TileOverlayPtr overlay = tile.overlay();

  if( overlay.isValid() )
  {
    registerTileForRendering( tile );
  }

  Layer::drawTile( engine, tile, offset );
}

LayerPtr Destroy::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Destroy( camera, city ) );
  ret->drop();

  return ret;
}

Destroy::Destroy( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city ), _d( new Impl )
{
  _d->shovelPic = Picture::load( "shovel", 1 );
  std::string rcLand = SETTINGS_VALUE( forbidenTile ).toString();
  _d->clearPic = Picture::load( rcLand, 2 );
  _d->textFont = Font::create( FONT_5 );
  _d->textPic.init( Size( 100, 30 ) );
  _addWalkerType( walker::all );
}

}

}//end namespace gfx
