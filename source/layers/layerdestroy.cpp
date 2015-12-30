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
#include "constants.hpp"
#include "events/event.hpp"
#include "walker/constants.hpp"
#include "walker/walker.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap_camera.hpp"
#include "city/city.hpp"
#include "gfx/tilemap_config.hpp"
#include "core/event.hpp"
#include "events/clearland.hpp"
#include "core/utils.hpp"
#include "objects/metadata.hpp"
#include "events/fundissue.hpp"
#include "game/funds.hpp"
#include "core/font.hpp"
#include "gfx/tilearea.hpp"
#include "core/osystem.hpp"
#include "build.hpp"
#include "events/undo_action.hpp"
#include "objects/tree.hpp"
#include "game/settings.hpp"
#include "core/format.hpp"
#include "gfx/maskstate.hpp"
#include "gfx/tile_config.hpp"

using namespace gfx;
using namespace events;

namespace citylayer
{

class DestroingArea : std::set<unsigned int>
{
public:
  DestroingArea& operator+=(Tile* tile)
  {
    this->insert( tile->epos().hash() );
    return *this;
  }

  DestroingArea& operator+=(const TilesArray& tiles )
  {
    for( auto tile : tiles )
      this->insert( tile->epos().hash() );

    return *this;
  }

  bool inArea( Tile* tile ) const
  {
    return tile->getFlag( Tile::isDestructible ) && count( tile->epos().hash() ) > 0;
  }
};

class Destroy::Impl
{
public:
  Picture shovelPic;
  Picture clearPic;
  Picture textPic;
  unsigned int savesum, money4destroy;
  TilePos startTilePos;
  LayerPtr lastLayer;
  Renderer* renderer;
  Font textFont;
  bool readyForExit;
};

void Destroy::_clearAll()
{
  TilesArray tiles4clear = _getSelectedArea( _d->startTilePos );
  std::set<Tile*> alsoDestroyed;
  for( auto tile : tiles4clear )
  {
    Tile* master = tile::master( tile );
    if( alsoDestroyed.count( master ) == 0 )
    {
      alsoDestroyed.insert( master );

      events::dispatch<ClearTile>( master->epos() );

      if( master->overlay().isValid() )
      {
        auto objectType = master->overlay()->type();
        int money = _checkMoney4destroy( *master );

        events::dispatch<UndoAction>( UndoAction::destroyed, objectType, master->epos(), money );
      }
    }
  }

  events::dispatch<UndoAction>( UndoAction::finished );
}

unsigned int Destroy::_checkMoney4destroy(const Tile& tile)
{
  OverlayPtr overlay = tile.overlay();
  int baseValue = 0;
  if( overlay.isValid() )
  {
    baseValue = overlay->info().cost() / 2;
  }

  baseValue += tile.getFlag( Tile::tlTree ) ? 6 : 0;
  baseValue += tile.getFlag( Tile::tlRoad ) ? 4 : 0;

  return baseValue;
}

void Destroy::render( Engine& engine )
{
  // center the map on the screen
  RenderInfo renderInfo = { engine, _camera()->offset() };

  const TilesArray& visibleTiles = _camera()->tiles();
  const TilesArray& flatTiles = _camera()->flatTiles();
  const TilesArray& groundTiles = _camera()->groundTiles();

  _camera()->startFrame();

  DestroingArea hashDestroyArea;
  TilesArray destroyArea = _getSelectedArea( _d->startTilePos );

  //create list of destroy tiles add full area building if some of it tile constain in destroy area
  _d->savesum = _d->money4destroy;
  _d->money4destroy = 0;
  std::set<Tile*> checked;
  for( auto dtile : destroyArea )
  {
    hashDestroyArea += dtile;

    OverlayPtr overlay = dtile->overlay();
    if( overlay.isValid() )
      hashDestroyArea += overlay->area();

    Tile* master = dtile->master() ? dtile->master() : dtile;
    if( checked.count( master ) == 0 )
    {
      checked.insert( master );
      _d->money4destroy += _checkMoney4destroy( *dtile );
    }
  }

  for( auto tile : groundTiles )
  {
    MaskState mask( engine, hashDestroyArea.inArea( tile )
                              ? ColorList::red
                              : ColorList::clear );

    drawLandTile( renderInfo, *tile );
  }

  // FIRST PART: draw all flat land (walkable/boatable)  
  for( auto ftile : flatTiles )
  {
    ftile = ftile->master() ? ftile->master() : ftile;

    if( !ftile->rendered() )
    {
      MaskState mask( engine, hashDestroyArea.inArea( ftile )
                              ? ColorList::red
                              : ColorList::clear );

      drawTile( renderInfo, *ftile );
    }
  }

  // SECOND PART: draw all sprites, impassable land and buildings
  for( auto vtile : visibleTiles )
  {
    int z = vtile->epos().z();

    MaskState mask( engine, hashDestroyArea.inArea( vtile )
                              ? ColorList::red
                              : ColorList::clear );

    drawProminentTile( renderInfo, *vtile, z, false );
    drawWalkers( renderInfo, *vtile );
  }
}

void Destroy::renderUi(Engine &engine)
{
  if( _d->savesum != _d->money4destroy )
  {
    _d->textPic.fill( 0x0, Rect() );
    _d->textFont.setColor( 0xffff0000 );
    _d->textFont.draw( _d->textPic, fmt::format( "{} Dn", _d->money4destroy ), Point() );
  }

  engine.draw( _d->shovelPic, engine.cursorPos() - Point( 5, _d->shovelPic.height() ) );
  engine.draw( _d->textPic, engine.cursorPos() + Point( 10, 10 ));
}

void Destroy::changeLayer(int layer)
{
  if( layer != type() )
  {
    if( layer == citylayer::build )
    {
      _exitDestroyTool();
    }
    else
    {
      _d->lastLayer = _d->renderer
                          ? _d->renderer->getLayer( layer )
                          : LayerPtr();
    }
  }
}

void Destroy::beforeRender(Engine &engine)
{
  if( _d->lastLayer.isValid() )
    _d->lastLayer->beforeRender( engine );
  else
    Layer::beforeRender( engine );
}

void Destroy::afterRender(Engine &engine)
{
  if( _d->lastLayer.isValid() )
    _d->lastLayer->afterRender( engine );
  else
    Layer::beforeRender( engine );
}

void Destroy::init(Point cursor)
{
  Layer::init( cursor );

  _setLastCursorPos( cursor );
  _setStartCursorPos( cursor );

  _d->startTilePos = TilePos::invalid();

  LayerPtr layer = _d->renderer->currentLayer();
  if( layer.isValid() )
  {
    auto buildLayer = layer.as<Build>();
    if( buildLayer.isValid() )
    {
      _d->lastLayer = buildLayer->drawLayer();
    }
    else if( layer->type() != type() )
    {
      _d->lastLayer = layer;
    }
  }

  DrawOptions::instance().setFlag( DrawOptions::mayChangeLayer, false );
}

void Destroy::_exitDestroyTool()
{
  _setNextLayer( _d->lastLayer.isValid() ? _d->lastLayer->type() : citylayer::simple );
  _city()->setOption( PlayerCity::updateTiles, 1 );
  DrawOptions::instance().setFlag( DrawOptions::mayChangeLayer, true );
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
        _d->startTilePos = tile ? tile->epos() : TilePos::invalid();
      }
    }
    break;

    case mouseLbtnRelease:            // left button
    {
      if( !OSystem::isAndroid() )
        _executeClear();
    }
    break;

    case mouseLbtnPressed: { _setStartCursorPos( _lastCursorPos() ); } break;
    case mouseRbtnRelease: { _exitDestroyTool(); } break;

    default:
    break;
    }
  }

  if( event.EventType == sEventKeyboard )
  {
    bool handled = _moveCamera( event );

    if( !handled )
    {
      switch( event.keyboard.key )
      {
      case KEY_ESCAPE:
      {
        if( OSystem::isAndroid() )
        {
          if( !_d->readyForExit )
          {
            _setLastCursorPos( _startCursorPos() );
            _d->readyForExit = true;
            break;
          }
        }

        _exitDestroyTool();
      }
      break;

      case KEY_RETURN:
      {
        _executeClear();
      }
      break;

      default: break;
      }
    }
  }
}

void Destroy::_executeClear()
{
  _clearAll();
  _setStartCursorPos( _lastCursorPos() );

  events::dispatch<Payment>( econ::Issue::buildConstruction, -_d->money4destroy );
}

int Destroy::type() const {  return citylayer::destroyd; }
LayerPtr Destroy::drawLayer() const { return _d->lastLayer; }

void Destroy::drawTile(const RenderInfo& rinfo, Tile& tile)
{
  OverlayPtr overlay = tile.overlay();

  if( overlay.isValid() )
  {
    registerTileForRendering( tile );
  }

  if( _d->lastLayer.isValid() )
  {
    _d->lastLayer->drawTile( rinfo, tile );
  }
  else
  {
    Layer::drawTile( rinfo, tile );
  }
}

LayerPtr Destroy::create(Renderer &renderer, PlayerCityPtr city)
{
  LayerPtr ret( new Destroy( renderer, city ) );
  ret->drop();

  return ret;
}

Destroy::Destroy( Renderer& renderer, PlayerCityPtr city)
  : Layer( renderer.camera(), city ), _d( new Impl )
{
  _d->renderer = &renderer;
  _d->shovelPic.load( "shovel", 1 );
  std::string rcLand = SETTINGS_STR( forbidenTile );
  _d->clearPic.load( rcLand, 2 );
  _d->textFont = Font::create( FONT_5 );
  _d->textPic = Picture( Size( 100, 30 ), 0, true );
  _addWalkerType( walker::all );
}

}//end namespace citylayer
