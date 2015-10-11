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

#include "constructor.hpp"
#include "objects/aqueduct.hpp"
#include "constants.hpp"
#include "walker/constants.hpp"
#include "game/roadbuild_helper.hpp"
#include "core/logger.hpp"
#include "events/build.hpp"
#include "core/foreach.hpp"
#include "city/city.hpp"
#include "core/event.hpp"
#include "gfx/sdl_engine.hpp"
#include "core/font.hpp"
#include "objects/fortification.hpp"
#include "core/utils.hpp"
#include "gfx/camera.hpp"
#include "gui/dialogbox.hpp"
#include "gfx/renderermode.hpp"
#include "events/warningmessage.hpp"
#include "game/funds.hpp"
#include "game/settings.hpp"
#include "walker/walker.hpp"
#include "gfx/tilearea.hpp"
#include "gfx/city_renderer.hpp"
#include "gfx/helper.hpp"
#include "gfx/tilemap.hpp"
#include "city/statistic.hpp"
#include "core/osystem.hpp"

using namespace gui;
using namespace gfx;
using namespace events;

namespace citylayer
{
static const int frameCountLimiter=25;

class Constructor::Impl
{
public:
  typedef std::map<unsigned int, Tile*> CachedTiles;
  bool multiBuilding;
  TilePos lastTilePos;
  TilePos startTilePos;
  bool kbShift, kbCtrl;
  bool lmbPressed;
  bool overdrawBuilding;
  bool borderBuilding;
  bool roadAssignment;
  bool needUpdateTiles;
  int drawLayerIndex;
  int frameCount;
  int money4Construction;
  Renderer* renderer;
  LayerPtr lastLayer;
  std::string resForbiden;
  Picture grnPicture;
  Picture redPicture;
  bool readyForExit;

  struct {
    Font font;
    Picture image;
  } text;

  TilesArray buildTiles;  // these tiles have draw over "normal" tilemap tiles!
  CachedTiles cachedTiles;

  Signal3<object::Type,TilePos,int> onBuildSignal;
public:
  void sortBuildTiles();
};

void Constructor::_discardPreview()
{
  __D_IMPL(d,Constructor)
  for( auto tile : d->buildTiles )
  {
    if( tile->overlay().isValid() )
      tile->overlay()->deleteLater();

    delete tile;
  }

  d->buildTiles.clear();
  d->cachedTiles.clear();
}

void Constructor::_checkPreviewBuild(TilePos pos)
{
  __D_IMPL(d,Constructor);
  BuildModePtr bldCommand =  d->renderer->mode().as<BuildMode>();

  if (bldCommand.isNull())
    return;

  // TODO: do only when needed, when (i, j, _buildInstance) has changed
  ConstructionPtr overlay = bldCommand->contruction();

  if( !overlay.isValid() )
  {
    return;
  }

  Size size = overlay->size();
  int cost = overlay->info().cost();

  bool walkersOnTile = false;
  if( bldCommand->flag( LayerMode::checkWalkers ) )
  {
    TilesArray tiles = _city()->tilemap().area( pos, size );
    for( auto tile : tiles )
    {
      auto walkers = _city()->walkers( tile->epos() );

      if( !walkers.empty() )
      {
        walkersOnTile = true;
        break;
      }
    }
  }

  city::AreaInfo areaInfo( _city(), pos, &d->buildTiles );
  if( !walkersOnTile && overlay->canBuild( areaInfo ) )
  {
    Tilemap& tmap = _city()->tilemap();
    Tile *masterTile=0;
    d->money4Construction += cost;
    for (int dj = 0; dj < size.height(); ++dj)
    {
      for (int di = 0; di < size.width(); ++di)
      {
        Tile& basicTile =  tmap.at( pos + TilePos( di, dj ) );
        Tile* tile = new Tile( basicTile.pos() );  // make a copy of tile
        tile->setEPos( basicTile.epos() );

        if (di==0 && dj==0)
        {
          // this is the masterTile
          masterTile = tile;
        }
        tile->setPicture( tmap.at( pos + TilePos( di, dj ) ).picture() );
        tile->setMaster( masterTile );
        tile->setOverlay( overlay.as<Overlay>() );
        d->buildTiles.push_back( tile );
      }
    }
  }
  else
  {
    Tilemap& tmap = _city()->tilemap();
    for (int dj = 0; dj < size.height(); ++dj)
    {
      for (int di = 0; di < size.width(); ++di)
      {
        TilePos rPos = pos + TilePos( di, dj );
        if( !tmap.isInside( rPos ) )
          continue;

        const Tile& basicTile = tmap.at( rPos );
        const bool isConstructible = basicTile.getFlag( Tile::isConstructible );
        Tile* tile = new Tile( basicTile.pos() );  // make a copy of tile
        tile->setEPos( basicTile.epos() );

        walkersOnTile = false;
        if( bldCommand->flag( LayerMode::checkWalkers ) )
        {
          walkersOnTile = !_city()->walkers( rPos ).empty();
        }

        tile->setPicture( (!walkersOnTile && isConstructible) ? d->grnPicture : d->redPicture );
        tile->setMaster( 0 );
        tile->setFlag( Tile::clearAll, true );
        tile->setOverlay( 0 );
        d->buildTiles.push_back( tile );
      }
    }
  }
}

void Constructor::_checkBuildArea()
{
  __D_IMPL(_d,Constructor);
  if( !_d->lmbPressed || _startCursorPos().x() < 0 )
  {
    _setStartCursorPos( _lastCursorPos() );

    Tile* tile = _camera()->at( _lastCursorPos(), true );
    _d->startTilePos = tile ? tile->epos() : tilemap::invalidLocation();
  }
}

static bool compare_tile(const Tile* one, const Tile* two)
{
  return one->pos().z() > two->pos().z();
}

void Constructor::_updatePreviewTiles( bool force )
{
  __D_IMPL(d,Constructor);
  Tile* curTile = _camera()->at( _lastCursorPos(), true );

  if( !curTile )
    return;

  if( !force && d->lastTilePos == curTile->epos() )
    return;

  if( !d->multiBuilding )
  {
    _setStartCursorPos( _lastCursorPos() );
    d->startTilePos = curTile->pos();
  }

  d->lastTilePos = curTile->epos();

  _discardPreview();
  d->money4Construction = 0;

  if( d->borderBuilding )
  {
    Tile* startTile = _camera()->at( d->startTilePos );  // tile under the cursor (or NULL)
    Tile* stopTile  = _camera()->at( _lastCursorPos(),  true );

    TilesArray pathTiles = RoadPropagator::createPath( _city()->tilemap(),
                                                     startTile->epos(), stopTile->epos(),
                                                     d->roadAssignment, d->kbShift );
    Tilemap& tmap = _city()->tilemap();
    TilePos leftUpCorner = pathTiles.leftUpCorner();
    TilePos rigthDownCorner = pathTiles.rightDownCorner();
    TilePos leftDownCorner( leftUpCorner.i(), rigthDownCorner.j() );
    TilesArray ret;

    int mmapSize = std::max<int>( leftUpCorner.j() - rigthDownCorner.j() + 1,
                                  rigthDownCorner.i() - leftUpCorner.i() + 1 );
    for( int y=0; y < mmapSize; y++ )
    {
      for( int t=0; t <= y; t++ )
      {
        TilePos tpos = leftDownCorner + TilePos( t, mmapSize - 1 - ( y - t ) );
        if( pathTiles.contain( tpos ) )
          ret.push_back( &tmap.at( tpos ) );
      }
    }

    for( int x=1; x < mmapSize; x++ )
    {
      for( int t=0; t < mmapSize-x; t++ )
      {
        TilePos tpos = leftDownCorner + TilePos( x + t, t );
        if( pathTiles.contain( tpos ) )
          ret.push_back( &tmap.at( tpos ) );
      }
    }

    pathTiles = ret;
    for( auto tile : pathTiles )
      _checkPreviewBuild( tile->epos() );
  }
  else
  {
    TilesArray tiles = _getSelectedArea( d->startTilePos );

    for( auto tile : tiles )
      _checkPreviewBuild( tile->epos() );
  }  

  d->sortBuildTiles();

  d->text.image.fill( 0x0, Rect() );
  d->text.font.setColor( 0xffff0000 );
  d->text.font.draw( d->text.image, utils::i2str( d->money4Construction ) + " Dn", Point() );
}

void Constructor::_buildAll()
{
  __D_IMPL(d,Constructor);
  BuildModePtr bldCommand = d->renderer->mode().as<BuildMode>();
  if( bldCommand.isNull() )
    return;

  ConstructionPtr cnstr = bldCommand->contruction();

  if( !cnstr.isValid() )
  {
    Logger::warning( "LayerBuild: No construction for build" );
    return;
  }

  if( !_city()->treasury().haveMoneyForAction( 1 ) )
  {
    auto event = WarningMessage::create( "##out_of_credit##", 2 );
    event->dispatch();
    return;
  }

  bool buildOk = false;  
  bool tileBusyBuilding = false;
  city::AreaInfo areaInfo( _city(), TilePos() );
  for( auto tile : d->buildTiles )
  {
    areaInfo.pos = tile->epos();
    tileBusyBuilding |= tile->overlay().is<Building>();

    if( cnstr->canBuild( areaInfo ) && tile->isMaster())
    {
      auto event = BuildAny::create( tile->epos(), cnstr->type() );
      event->dispatch();
      buildOk = true;

      emit d->onBuildSignal( cnstr->type(), tile->epos(), cnstr->info().cost() );
    }
  }

  d->startTilePos = d->lastTilePos;

  if( !buildOk )
  {
    std::string errorStr = cnstr->errorDesc();
    std::string busyText = tileBusyBuilding
                              ? "##need_build_on_free_area##"
                              : "##need_build_on_cleared_area##";
    auto event = WarningMessage::create( errorStr.empty() ? busyText : errorStr,
                                         WarningMessage::neitral );
    event->dispatch();
  }

  d->needUpdateTiles = true;
}

void Constructor::_exitBuildMode()
{
  __D_IMPL(_d,Constructor);
  DrawOptions::instance().setFlag( DrawOptions::mayChangeLayer, true );
  _setNextLayer( _d->lastLayer.isValid() ? _d->lastLayer->type() : citylayer::simple );
  _setStartCursorPos( Point(-1, -1) );
  _discardPreview();
}

void Constructor::handleEvent(NEvent& event)
{
  __D_IMPL(_d,Constructor);
  if( event.EventType == sEventMouse )
  {
    _d->kbShift = event.mouse.shift;
    _d->kbCtrl = event.mouse.control;
    _d->readyForExit = false;

    if( !OSystem::isAndroid() )
      _d->lmbPressed = event.mouse.isLeftPressed();

    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      _setLastCursorPos( event.mouse.pos() );
      _checkBuildArea();
      _updatePreviewTiles( false );
    }
    break;

    case mouseLbtnPressed:
    {
      _updatePreviewTiles( false );
      _d->lmbPressed = true;
    }
    break;

    case mouseLbtnRelease:            // left button
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      if( tile == 0 )
      {
        break;
      }

      if( !OSystem::isAndroid() )
        _finishBuild();
    }
    break;

    case mouseRbtnRelease: { _exitBuildMode(); } break;
    default:    break;
    }
  }

  if( event.EventType == sEventKeyboard )
  {
    bool handled = _moveCamera( event );
    _d->kbShift = event.keyboard.shift;
    _d->kbCtrl = event.keyboard.control;

    if( !handled && !event.keyboard.pressed )
    {
      switch( event.keyboard.key )
      {
      case KEY_ESCAPE:
      {
        if( OSystem::isAndroid() )
        {
          if( !_d->readyForExit )
          {
            _setStartCursorPos( Point(-1, -1) );
            _d->startTilePos = tilemap::invalidLocation();
            _d->lastTilePos = tilemap::invalidLocation();
            _d->needUpdateTiles = true;
            _d->lmbPressed = false;
            _d->readyForExit = true;
            break;
          }
        }

        _exitBuildMode();
      }
      break;

      case KEY_RETURN:
      {
        if( !event.keyboard.pressed )  //button was left up
        {
          _finishBuild();
        }
      }
      break;

      default: break;
      }
    }
    else
    {
      _d->needUpdateTiles = true;
    }
  }
}

void Constructor::_finishBuild()
{
  _buildAll();
  _setStartCursorPos( _lastCursorPos() );
  _updatePreviewTiles( true );
  _dfunc()->lmbPressed = false;
}

int Constructor::type() const {  return citylayer::constructor; }

void Constructor::_drawBuildTile( Engine& engine, Tile* tile, const Point& offset )
{
  if( tile == nullptr )
    return;

  __D_IMPL(_d,Constructor);
  city::AreaInfo areaInfo( _city(), TilePos(), &_d->buildTiles );

  Tile* postTile = tile;
  postTile->resetRendered();

  if( postTile->master() )
    postTile = postTile->master();

  ConstructionPtr construction = postTile->overlay<Construction>();
  engine.resetColorMask();

  areaInfo.pos = postTile->epos();
  bool maskSet = false;
  Size size(1,1);

  if( construction.isValid() && construction->canBuild( areaInfo ) )
  {
    engine.setColorMask( 0x00000000, 0x0000ff00, 0, 0xff000000 );
    maskSet = true;

    size = construction->size();
  }

  if( postTile == tile && maskSet )
  {
    TilesArray area = _city()->tilemap().area( areaInfo.pos, size );
    for( auto gtile : area )
    {
      drawPass( engine, *gtile, offset, Renderer::ground );
      drawPass( engine, *gtile, offset, Renderer::groundAnimation );
    }
  }

  drawPass( engine, *postTile, offset, Renderer::ground );
  drawPass( engine, *postTile, offset, Renderer::groundAnimation );

  if( maskSet )
  {
    const Picture& picOver = construction->picture( areaInfo );
    engine.draw( picOver, postTile->mappos() + offset );
    drawPass( engine, *postTile, offset, Renderer::overlayAnimation );
    engine.resetColorMask();
  }
}

void Constructor::_tryDrawBuildTile( Engine& engine, Tile &tile, const Point& camOffset)
{
  Impl::CachedTiles& cache = _dfunc()->cachedTiles;
  auto it = cache.find( tile::hash( tile.epos() ) );
  if( it != cache.end() )
  {
    _drawBuildTile( engine, it->second, camOffset );
  }
}

void Constructor::_drawBuildTiles( Engine& engine )
{
  __D_IMPL(_d,Constructor);

  Point offset = _camera()->offset();

  for( auto tile : _d->buildTiles )
    _drawBuildTile( engine, tile, offset );  
}

void Constructor::drawTile( Engine& engine, Tile& tile, const Point& offset )
{
  __D_IMPL(_d,Constructor);
  Point screenPos = tile.mappos() + offset;

  ConstructionPtr cntr = tile.overlay<Construction>();
  city::AreaInfo info( _city(), tile.epos(), &_d->buildTiles );

  const Picture* picBasic = 0;
  const Picture* picOver = 0;
  if( cntr.isValid() && info.tiles().size() > 0 )
  {
    picBasic = &cntr->picture();
    picOver = &cntr->picture( info );
  }

  if( picOver && picBasic != picOver )
  {
    drawPass( engine, tile, offset, Renderer::ground );
    engine.draw( *picOver, screenPos );
    drawPass( engine, tile, offset, Renderer::overlayAnimation );
  }
  else if( _d->lastLayer.isValid() )
  {
    _d->lastLayer->drawTile( engine, tile, offset );
  }
  else
  {
    Layer::drawTile( engine, tile, offset );
  }

  if( !_d->overdrawBuilding )
    _tryDrawBuildTile( engine, tile, offset );
}

void Constructor::drawProminentTile( Engine& engine, Tile& tile, const Point& offset, const int depth, bool force)
{
  Layer::drawProminentTile( engine, tile, offset, depth, force );

  _tryDrawBuildTile( engine, tile, offset );
}

void Constructor::render( Engine& engine)
{
  __D_IMPL(d,Constructor);
  Layer::render( engine );

  if( ++d->frameCount >= frameCountLimiter)
  {
    _updatePreviewTiles( true );
  }

  d->frameCount %= frameCountLimiter;

  if( d->overdrawBuilding )
    _drawBuildTiles( engine );
}

void Constructor::_initBuildMode()
{
  __D_IMPL(_d,Constructor);
  BuildModePtr command = ptr_cast<BuildMode>( _d->renderer->mode() );
  Logger::warningIf( !command.isValid(), "LayerBuild: init unknown command" );

  _d->multiBuilding = command.isValid() ? command->flag( LayerMode::multibuild ) : false;
  _d->roadAssignment = command.isValid() ? command->flag( LayerMode::assign2road ) : false;
  _d->borderBuilding = command.isValid() ? command->flag( LayerMode::border ) : false;
}

void Constructor::init(Point cursor)
{
  __D_IMPL(_d,Constructor);
  Layer::init( cursor );

  _d->lastTilePos = tilemap::invalidLocation();
  _d->startTilePos = tilemap::invalidLocation();
  _d->readyForExit = false;
  _d->kbShift = false;
  _d->kbCtrl = false;

  changeLayer( _d->renderer->currentLayer()->type() );

  DrawOptions::instance().setFlag( DrawOptions::mayChangeLayer, false );

  if( OSystem::isAndroid() )
  {
    auto message = WarningMessage::create( "Press red cross for break/exit, stamp for build", WarningMessage::neitral );
    message->dispatch();
  }
}

void Constructor::beforeRender(Engine& engine)
{
  __D_IMPL(_d,Constructor);
  _d->overdrawBuilding = DrawOptions::instance().isFlag( DrawOptions::overdrawOnBuild );
  if( _d->lastLayer.isValid() )
    _d->lastLayer->beforeRender( engine );
  else
    Layer::beforeRender( engine );
}

void Constructor::drawPass(Engine& engine, Tile& tile, const Point& offset, Renderer::Pass pass)
{
  __D_IMPL(_d,Constructor);
  if( _d->lastLayer.isValid() )
    _d->lastLayer->drawPass( engine, tile, offset, pass );
  else
    Layer::drawPass( engine, tile, offset, pass );
}

void Constructor::afterRender(Engine& engine)
{
  __D_IMPL(_d,Constructor);
  if( _d->needUpdateTiles )
  {
   if( !OSystem::isAndroid() )
   {
      _setLastCursorPos( engine.cursorPos() );
   }

    _checkBuildArea();
    _updatePreviewTiles( false );
  }

  if( _d->lastLayer.isValid() )
    _d->lastLayer->afterRender( engine );
  else
    Layer::afterRender( engine );
}

const Layer::WalkerTypes& Constructor::visibleTypes() const
{
  __D_IMPL_CONST(_d,Constructor);
  if( _d->lastLayer.isValid() )
    return _d->lastLayer->visibleTypes();

  return Layer::visibleTypes();
}

void Constructor::renderUi(Engine &engine)
{
  Layer::renderUi( engine );
  engine.draw( _dfunc()->text.image, engine.cursorPos() + Point( 10, 10 ));
}

void Constructor::changeLayer(int layer)
{
  __D_IMPL(_d,Constructor);
  if( layer != type() )
  {
    if( layer == citylayer::destroyd )
    {
       _exitBuildMode();
    }
    else
    {
      _d->lastLayer = _d->renderer
                          ? _d->renderer->getLayer( layer )
                          : LayerPtr();
    }
  }

  _initBuildMode();
}

LayerPtr Constructor::create(Renderer& renderer, PlayerCityPtr city)
{
  LayerPtr ret( new Constructor( renderer, city ) );
  ret->drop();

  return ret;
}

LayerPtr Constructor::drawLayer() const { return _dfunc()->lastLayer; }
Constructor::~Constructor() {}

Constructor::Constructor( Renderer& renderer, PlayerCityPtr city)
  : Layer( renderer.camera(), city ),
    __INIT_IMPL(Constructor)
{
  __D_IMPL(d,Constructor);
  d->renderer = &renderer;
  d->frameCount = 0;
  d->needUpdateTiles = false;
  d->resForbiden = SETTINGS_STR( forbidenTile );
  d->startTilePos = gfx::tilemap::invalidLocation();
  d->text.font = Font::create( FONT_5 );
  d->readyForExit = false;
  d->text.image = Picture( Size( 100, 30 ), 0, true );
  _addWalkerType( walker::all );

  d->grnPicture.load( d->resForbiden, 1 );
  d->redPicture.load( d->resForbiden, 2 );
}

Signal3<object::Type,TilePos,int>& Constructor::onBuild()
{
  return _dfunc()->onBuildSignal;
}

void Constructor::Impl::sortBuildTiles()
{
  std::sort( buildTiles.begin(), buildTiles.end(), compare_tile );

  cachedTiles.clear();
  for( auto t : buildTiles )
    cachedTiles[ tile::hash( t->epos() ) ] = t;
}

}//end namespace citylayer
