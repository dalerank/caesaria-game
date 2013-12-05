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

#include "layerdestroy.hpp"
#include "layerconstants.hpp"
#include "events/event.hpp"
#include "walker/constants.hpp"
#include "walker/walker.hpp"
#include "core/foreach.hpp"
#include "tilemap_camera.hpp"
#include "game/city.hpp"
#include "core/event.hpp"

using namespace constants;

void LayerDestroy::_clearAll()
{
  TilesArray tiles4clear = _getSelectedArea();
  foreach( Tile* tile, tiles4clear )
  {
    events::GameEventPtr event = events::ClearLandEvent::create( tile->getIJ() );
    event->dispatch();
  }
}

void LayerDestroy::_drawTileInSelArea( GfxEngine& engine, Tile& tile, Tile* master, const Point& offset )
{
  if( master==NULL )
  {
    // single-tile
    drawTile( engine, tile, offset );
    engine.drawPicture( _clearPic, tile.getXY() + offset );
  }
  else
  {
    engine.setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );

    // multi-tile: draw the master tile.
    if( !master->getFlag( Tile::wasDrawn ) )
      drawTile( engine, *master, offset );

    engine.resetTileDrawMask();
  }
}


void LayerDestroy::render( GfxEngine& engine )
{
  // center the map on the screen
  Point cameraOffset = _getCamera()->getOffset();

  int lastZ = -1000;  // dummy value

  _getCamera()->startFrame();

  TilesArray visibleTiles = _getCamera()->getTiles();

  Tilemap& tmap = _getCity()->getTilemap();

  std::set<int> hashDestroyArea;
  TilesArray destroyArea = _getSelectedArea();

  //create list of destroy tiles add full area building if some of it tile constain in destroy area
  foreach( Tile* tile, destroyArea)
  {
    hashDestroyArea.insert( tile->getJ() * 1000 + tile->getI() );

    TileOverlayPtr overlay = tile->getOverlay();
    if( overlay.isValid() )
    {
      TilesArray overlayArea = tmap.getArea( overlay->getTilePos(), overlay->getSize() );
      foreach( Tile* ovelayTile, overlayArea )
      {
        hashDestroyArea.insert( ovelayTile->getJ() * 1000 + ovelayTile->getI() );
      }
    }
  }
  //Rect destroyArea = Rect( startPos.getI(), startPos.getJ(), stopPos.getI(), stopPos.getJ() );

  // FIRST PART: draw all flat land (walkable/boatable)
  foreach( Tile* tile, visibleTiles )
  {
    Tile* master = tile->getMasterTile();

    if( !tile->isFlat() )
      continue;

    int tilePosHash = tile->getJ() * 1000 + tile->getI();
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      _drawTileInSelArea( engine, *tile, master, cameraOffset );
    }
    else
    {
      if( master==NULL )
      {
        // single-tile
        drawTile( engine, *tile, cameraOffset );
      }
      else if( !master->getFlag( Tile::wasDrawn ) )
      {
        // multi-tile: draw the master tile.
        drawTile( engine, *master, cameraOffset );
      }
    }
  }

  // SECOND PART: draw all sprites, impassable land and buildings
  WalkerList walkerList = _getVisibleWalkerList();
  foreach( Tile* tile, visibleTiles )
  {
    int z = tile->getIJ().getZ();

    if (z != lastZ)
    {
      // TODO: pre-sort all animations
      lastZ = z;
      _drawWalkersBetweenZ( engine, walkerList, z, z+1, cameraOffset );
    }

    int tilePosHash = tile->getJ() * 1000 + tile->getI();
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      engine.setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );
    }

    drawTileR( engine, *tile, cameraOffset, z, false );
    engine.resetTileDrawMask();
  }
}

void LayerDestroy::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      _setLastCursorPos( event.mouse.getPosition() );
      if( !event.mouse.isLeftPressed() || _getStartCursorPos().getX() < 0 )
      {
        _setStartCursorPos( _getLastCursorPos() );
      }
    }
    break;

    case mouseLbtnPressed:
    {
      _setStartCursorPos( _getLastCursorPos() );
    }
    break;

    case mouseLbtnRelease:            // left button
    {
      Tile* tile = _getCamera()->at( event.mouse.getPosition(), false );  // tile under the cursor (or NULL)
      if( tile == 0 )
      {
        break;
      }

      _clearAll();
      _setStartCursorPos( _getLastCursorPos() );
    }
    break;

    case mouseRbtnRelease:
    {
      _setNextLayer( citylayer::simple );
    }
    break;

    default:
    break;
    }
  }

  if( event.EventType == sEventKeyboard )
  {
    bool pressed = event.keyboard.pressed;
    int moveValue = _getCamera()->getScrollSpeed() * ( event.keyboard.shift ? 4 : 1 ) * (pressed ? 1 : 0);

    TilemapCamera* cam = _getCamera();
    switch( event.keyboard.key )
    {
    case KEY_UP:    cam->moveUp   ( moveValue ); break;
    case KEY_DOWN:  cam->moveDown ( moveValue ); break;
    case KEY_RIGHT: cam->moveRight( moveValue ); break;
    case KEY_LEFT:  cam->moveLeft ( moveValue ); break;
    default: break;
    }
  }
}

int LayerDestroy::getType() const
{
  return citylayer::destroy;
}

std::set<int> LayerDestroy::getVisibleWalkers() const
{
  std::set<int> ret;
  ret.insert( walker::all );

  return ret;
}

void LayerDestroy::drawTile( GfxEngine& engine, Tile& tile, Point offset )
{
  Point screenPos = tile.getXY() + offset;

  TileOverlayPtr overlay = tile.getOverlay();

  if( overlay.isValid() )
  {
    registerTileForRendering( tile );
  }

  if( !tile.getFlag( Tile::wasDrawn ) )
  {
    tile.setWasDrawn();
    engine.drawPicture( tile.getPicture(), screenPos );

    if( tile.getAnimation().isValid() )
    {
      engine.drawPicture( tile.getAnimation().getFrame(), screenPos );
    }

    drawTilePass( engine, tile, offset, Renderer::foreground );
  }
}

LayerPtr LayerDestroy::create(TilemapCamera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerDestroy( camera, city ) );
  ret->drop();

  return ret;
}

LayerDestroy::LayerDestroy(TilemapCamera& camera, PlayerCityPtr city)
  : Layer( camera, city )
{
  _clearPic = Picture::load( "oc3_land", 2 );
}
