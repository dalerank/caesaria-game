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

#include "layerbuild.hpp"
#include "objects/aqueduct.hpp"
#include "layerconstants.hpp"
#include "walker/constants.hpp"
#include "game/roadbuild_helper.hpp"
#include "core/logger.hpp"
#include "events/event.hpp"
#include "core/gettext.hpp"
#include "core/foreach.hpp"
#include "city/city.hpp"
#include "tilemap_camera.hpp"
#include "core/event.hpp"
#include "sdl_engine.hpp"
#include "core/font.hpp"
#include "objects/fortification.hpp"
#include "core/stringhelper.hpp"

using namespace constants;

class LayerBuild::Impl
{
public:
  bool multiBuilding;
  TilePos lastTilePos;
  bool borderBuilding;
  bool roadAssignment;
  CityRenderer* renderer;
  TilesArray buildTiles;  // these tiles have draw over "normal" tilemap tiles!
};

void LayerBuild::_discardPreview()
{
  foreach( Tile* tile, _d->buildTiles )
  {
    if( tile->getOverlay().isValid() )
    {
      tile->getOverlay()->deleteLater();
    }

    delete tile;
  }

  _d->buildTiles.clear();
}

void LayerBuild::_checkPreviewBuild(TilePos pos)
{
  BuildModePtr bldCommand = _d->renderer->getMode().as<BuildMode>();

  if (bldCommand.isNull())
    return;

  // TODO: do only when needed, when (i, j, _buildInstance) has changed
  ConstructionPtr overlay = bldCommand->getContruction();

  if (!overlay.isValid())
  {
    return;
  }

  int size = overlay->getSize().getWidth();

  if( overlay->canBuild( _getCity(), pos, _d->buildTiles ) )
  {
    //bldCommand->setCanBuild(true);
    Tilemap& tmap = _getCity()->getTilemap();
    Tile *masterTile=0;
    for (int dj = 0; dj < size; ++dj)
    {
      for (int di = 0; di < size; ++di)
      {
        Tile* tile = new Tile( tmap.at( pos + TilePos( di, dj ) ));  // make a copy of tile

        if (di==0 && dj==0)
        {
          // this is the masterTile
          masterTile = tile;
        }
        tile->setPicture( &overlay->getPicture() );
        tile->setMasterTile( masterTile );
        tile->setOverlay( overlay.as<TileOverlay>() );
        //tile->setFlag( Tile::tlRock, true );  //dirty hack that drawing this tile
        _d->buildTiles.push_back( tile );
      }
    }
  }
  else
  {
    //bldCommand->setCanBuild(false);

    Picture& grnPicture = Picture::load("oc3_land", 1);
    Picture& redPicture = Picture::load("oc3_land", 2);

    //TilemapArea area = til
    Tilemap& tmap = _getCity()->getTilemap();
    for (int dj = 0; dj < size; ++dj)
    {
      for (int di = 0; di < size; ++di)
      {
        TilePos rPos = pos + TilePos( di, dj );
        if( !tmap.isInside( rPos ) )
          continue;

        Tile* tile = new Tile( tmap.at( rPos ) );  // make a copy of tile

        bool isConstructible = tile->getFlag( Tile::isConstructible );
        tile->setPicture( isConstructible ? &grnPicture : &redPicture );
        tile->setMasterTile( 0 );
        tile->setFlag( Tile::clearAll, true );
        //tile->setFlag( Tile::tlRock, true );  //dirty hack that drawing this tile
        tile->setOverlay( 0 );
        _d->buildTiles.push_back( tile );
      }
    }
  }
}

void LayerBuild::_updatePreviewTiles( bool force )
{
  if( !_d->multiBuilding )
    _setStartCursorPos( _getLastCursorPos() );

  Tile* curTile = _getCamera()->at( _getLastCursorPos(), true );

  if( !curTile )
    return;

  if( curTile && !force && _d->lastTilePos == curTile->getIJ() )
    return;

  _d->lastTilePos = curTile->getIJ();

  _discardPreview();

  if( _d->borderBuilding )
  {
    Tile* startTile = _getCamera()->at( _getStartCursorPos(), true );  // tile under the cursor (or NULL)
    Tile* stopTile  = _getCamera()->at( _getLastCursorPos(),  true );

    TilesArray pathWay = RoadPropagator::createPath( _getCity()->getTilemap(),
                                                     startTile->getIJ(), stopTile->getIJ(),
                                                     _d->roadAssignment );
    for( TilesArray::iterator it=pathWay.begin(); it != pathWay.end(); it++ )
    {
      _checkPreviewBuild( (*it)->getIJ() );
    }
  }
  else
  {
    TilesArray tiles = _getSelectedArea();

    foreach( Tile* tile, tiles )
    {
      _checkPreviewBuild( tile->getIJ() );
    }
  }
}

void LayerBuild::_buildAll()
{
  BuildModePtr bldCommand = _d->renderer->getMode().as<BuildMode>();
  if( bldCommand.isNull() )
    return;

  ConstructionPtr cnstr = bldCommand->getContruction();

  if( !cnstr.isValid() )
  {
    Logger::warning( "No construction for build" );
    return;
  }

  bool buildOk = false;
  foreach( Tile* tile, _d->buildTiles )
  {
    if( cnstr->canBuild( _getCity(), tile->getIJ(), TilesArray() ) && tile->isMasterTile())
    {
      events::GameEventPtr event = events::BuildEvent::create( tile->getIJ(), cnstr->getType() );
      event->dispatch();
      buildOk = true;
    }
  }

  if( !buildOk )
  {
    std::string errorStr = cnstr->getError();

    events::GameEventPtr event = events::WarningMessageEvent::create( errorStr.empty()
                                                                      ? _("##need_build_on_cleared_area##")
                                                                      : errorStr );
    event->dispatch();
  }
}

void LayerBuild::handleEvent(NEvent& event)
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

      _updatePreviewTiles(false);
    }
    break;

    case mouseLbtnPressed:
    {
      _setStartCursorPos( _getLastCursorPos() );
      _updatePreviewTiles( false );
    }
    break;

    case mouseLbtnRelease:            // left button
    {
      Tile* tile = _getCamera()->at( event.mouse.getPosition(), false );  // tile under the cursor (or NULL)
      if( tile == 0 )
      {
        break;
      }

      _buildAll();
      _setStartCursorPos( _getLastCursorPos() );
      _updatePreviewTiles( true );
    }
    break;

    case mouseRbtnRelease:
    {
      _setNextLayer( citylayer::simple );
      _discardPreview();
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

int LayerBuild::getType() const
{
  return citylayer::build;
}

std::set<int> LayerBuild::getVisibleWalkers() const
{
  std::set<int> ret;
  ret.insert( walker::all );

  return ret;
}

void LayerBuild::_drawBuildTiles(GfxEngine& engine)
{
  Point offset = _getCamera()->getOffset();
  foreach( Tile* postTile, _d->buildTiles )
  {
    postTile->resetWasDrawn();

    ConstructionPtr ptr_construction = postTile->getOverlay().as<Construction>();
    engine.resetTileDrawMask();

    if( ptr_construction != NULL
        && ptr_construction->canBuild( _getCity(), postTile->getIJ(), _d->buildTiles ) )
    {
      engine.setTileDrawMask( 0x00000000, 0x0000ff00, 0, 0xff000000 );
    }

    drawTileR( engine, *postTile, offset, postTile->getIJ().getZ(), true );
  }

  engine.resetTileDrawMask();
}

void LayerBuild::drawTile( GfxEngine& engine, Tile& tile, Point offset )
{
  Point screenPos = tile.getXY() + offset;

  TileOverlayPtr overlay = tile.getOverlay();
  const TilesArray& postTiles = _d->buildTiles;

  if( overlay.isValid() )
  {
    ConstructionPtr cntr = overlay.as<Construction>();
    if( cntr.isValid() && postTiles.size() > 0 )
    {
      tile.setWasDrawn();
      const Picture& pic = cntr->getPicture( _getCity(), tile.getIJ(), postTiles );
      engine.drawPicture( pic, screenPos );

      drawTilePass( engine, tile, offset, Renderer::foreground );
    }

    registerTileForRendering( tile );

    if( tile.getOverlay().is<Fortification>() )
    {
      GfxSdlEngine* e = static_cast< GfxSdlEngine* >( &GfxEngine::instance());
      Font f = Font::create( FONT_2 );
      f.setColor( 0xffff0000 );
      int df = tile.getOverlay().as<Fortification>()->getDirection();
      f.draw( e->getScreen(), StringHelper::format( 0xff, "%x", df), screenPos + Point( 20, -80 ), false );
    }
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

void LayerBuild::render(GfxEngine& engine)
{
  Layer::render( engine );

  _drawBuildTiles( engine );
}

void LayerBuild::init(Point cursor)
{
  Layer::init( cursor );

  BuildModePtr command = _d->renderer->getMode().as<BuildMode>();
  _d->multiBuilding = command.isValid() ? command->isMultiBuilding() : false;
  _d->roadAssignment = command.isValid() ? command->isRoadAssignment() : false;
  _d->borderBuilding = command.isValid() ? command->isBorderBuilding() : false;
}

LayerPtr LayerBuild::create(CityRenderer* renderer, PlayerCityPtr city)
{
  LayerPtr ret( new LayerBuild( renderer, city ) );
  ret->drop();

  return ret;
}

LayerBuild::~LayerBuild()
{
}

LayerBuild::LayerBuild(CityRenderer* renderer, PlayerCityPtr city)
  : Layer( renderer->getCamera(), city ), _d( new Impl )
{
  _d->renderer = renderer;  
}
