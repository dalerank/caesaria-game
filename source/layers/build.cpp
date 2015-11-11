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

#include "build.hpp"
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
#include "walker/corpse.hpp"
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

class Build::Impl
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
  bool readyForExit;

  struct {
    Picture green;
    Picture red;
  } btile;

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

void Build::_discardPreview()
{
  __D_IMPL(d,Build)
  for( auto tile : d->buildTiles )
  {
    if( tile->overlay().isValid() )
      tile->overlay()->deleteLater();

    delete tile;
  }

  d->buildTiles.clear();
  d->cachedTiles.clear();
}

void Build::_checkPreviewBuild(TilePos pos)
{
  __D_IMPL(d,Build);
  auto command =  d->renderer->mode().as<BuildMode>();

  if (command.isNull())
    return;

  // TODO: do only when needed, when (i, j, _buildInstance) has changed
  ConstructionPtr construction = command->contruction();

  if( !construction.isValid() )
  {
    return;
  }

  Size size = construction->size();
  int cost = construction->info().cost();

  bool walkersOnTile = false;
  if( command->flag( LayerMode::checkWalkers ) )
  {
    TilesArray tiles = _city()->tilemap().area( pos, size );
    for( auto tile : tiles )
    {
      auto walkers = _city()->walkers( tile->epos() )
                              .exclude<Corpse>();

      if( !walkers.empty() )
      {
        walkersOnTile = true;
        break;
      }
    }
  }

  city::AreaInfo areaInfo( _city(), pos, &d->buildTiles );
  if( !walkersOnTile && construction->canBuild( areaInfo ) )
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
        tile->setOverlay( construction.as<Overlay>() );
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
        if( command->flag( LayerMode::checkWalkers ) )
        {
          walkersOnTile = !_city()->walkers( rPos ).empty();
        }

        tile->setPicture( (!walkersOnTile && isConstructible) ? d->btile.green : d->btile.red );
        tile->setMaster( 0 );
        tile->setFlag( Tile::clearAll, true );
        tile->setOverlay( 0 );
        d->buildTiles.push_back( tile );
      }
    }
  }
}

void Build::_checkBuildArea()
{
  __D_REF(d,Build);
  if( !d.lmbPressed || _startCursorPos().x() < 0 )
  {
    _setStartCursorPos( _lastCursorPos() );

    Tile* tile = _camera()->at( _lastCursorPos(), true );
    d.startTilePos = tile ? tile->epos() : tilemap::invalidLocation();
  }
}

static bool compare_tile(const Tile* one, const Tile* two)
{
  return one->pos().z() > two->pos().z();
}

void Build::_updatePreviewTiles( bool force )
{
  __D_REF(d,Build);
  Tile* curTile = _camera()->at( _lastCursorPos(), true );

  if( !curTile )
    return;

  if( !force && d.lastTilePos == curTile->epos() )
    return;

  if( !d.multiBuilding )
  {
    _setStartCursorPos( _lastCursorPos() );
    d.startTilePos = curTile->pos();
  }

  d.lastTilePos = curTile->epos();

  _discardPreview();
  d.money4Construction = 0;

  if( d.borderBuilding )
  {
    Tile* startTile = _camera()->at( d.startTilePos );  // tile under the cursor (or NULL)
    Tile* stopTile  = _camera()->at( _lastCursorPos(),  true );

    TilesArray pathTiles = RoadPropagator::createPath( _city()->tilemap(),
                                                       startTile->epos(), stopTile->epos(),
                                                       d.roadAssignment, d.kbShift );
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
    TilesArray tiles = _getSelectedArea( d.startTilePos );

    for( auto tile : tiles )
      _checkPreviewBuild( tile->epos() );
  }  

  d.sortBuildTiles();

  d.text.image.fill( 0x0, Rect() );
  d.text.font.setColor( 0xffff0000 );
  d.text.font.draw( d.text.image, utils::i2str( d.money4Construction ) + " Dn", Point() );
}

void Build::_buildAll()
{
  __D_REF(d,Build);
  auto command = d.renderer->mode().as<BuildMode>();
  if( command.isNull() )
    return;

  ConstructionPtr construction = command->contruction();

  if( !construction.isValid() )
  {
    Logger::warning( "LayerBuild: No construction for build" );
    return;
  }

  if( !_city()->treasury().haveMoneyForAction( 1 ) )
  {
    events::dispatch<WarningMessage>( "##out_of_credit##", 2 );
    return;
  }

  bool buildOk = false;  
  bool tileBusyBuilding = false;
  city::AreaInfo areaInfo( _city(), TilePos() );
  for( auto tile : d.buildTiles )
  {
    areaInfo.pos = tile->epos();
    tileBusyBuilding |= tile->overlay().is<Building>();

    if( construction->canBuild( areaInfo ) && tile->isMaster())
    {
      events::dispatch<BuildAny>( tile->epos(), construction->type() );
      buildOk = true;

      emit d.onBuildSignal( construction->type(), tile->epos(), construction->info().cost() );
    }
  }

  d.startTilePos = d.lastTilePos;

  if( !buildOk )
  {
    std::string errorStr = construction->errorDesc();
    std::string busyText = tileBusyBuilding
                              ? "##need_build_on_free_area##"
                              : "##need_build_on_cleared_area##";
    events::dispatch<WarningMessage>( errorStr.empty() ? busyText : errorStr,
                                         WarningMessage::neitral );
  }

  d.needUpdateTiles = true;
}

void Build::_exitBuildMode()
{
  __D_IMPL(_d,Build);
  DrawOptions::instance().setFlag( DrawOptions::mayChangeLayer, true );
  _setNextLayer( _d->lastLayer.isValid() ? _d->lastLayer->type() : citylayer::simple );
  _setStartCursorPos( Point(-1, -1) );
  _discardPreview();
}

void Build::handleEvent(NEvent& event)
{
  __D_REF(d,Build);

  if( event.EventType == sEventMouse )
  {
    d.kbShift = event.mouse.shift;
    d.kbCtrl = event.mouse.control;
    d.readyForExit = false;

    Point cursorPos = event.mouse.pos();

    if( !OSystem::isAndroid() )
      d.lmbPressed = event.mouse.isLeftPressed();

    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      _setLastCursorPos( cursorPos );
      _checkBuildArea();
      _updatePreviewTiles( false );
    }
    break;

    case mouseLbtnPressed:
    {
      _updatePreviewTiles( false );
      d.lmbPressed = true;
    }
    break;

    case mouseLbtnRelease:            // left button
    {
      Tile* tile = _camera()->at( cursorPos, false );  // tile under the cursor (or NULL)
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
    d.kbShift = event.keyboard.shift;
    d.kbCtrl = event.keyboard.control;

    if( !handled && !event.keyboard.pressed )
    {
      switch( event.keyboard.key )
      {
      case KEY_ESCAPE:
      {
        if( OSystem::isAndroid() )
        {
          if( !d.readyForExit )
          {
            _setStartCursorPos( Point( -1, -1 ) );
            d.startTilePos = tilemap::invalidLocation();
            d.lastTilePos = tilemap::invalidLocation();
            d.needUpdateTiles = true;
            d.lmbPressed = false;
            d.readyForExit = true;
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
      d.needUpdateTiles = true;
    }
  }
}

void Build::_finishBuild()
{
  _buildAll();
  _setStartCursorPos( _lastCursorPos() );
  _updatePreviewTiles( true );
  _dfunc()->lmbPressed = false;
}

int Build::type() const {  return citylayer::build; }

void Build::_drawBuildTile( const RenderInfo& rinfo, Tile* tile)
{
  if( tile == nullptr )
    return;

  __D_IMPL(_d,Build);
  city::AreaInfo areaInfo( _city(), TilePos(), &_d->buildTiles );

  Tile* postTile = tile;
  postTile->resetRendered();

  if( postTile->master() )
    postTile = postTile->master();

  ConstructionPtr construction = postTile->overlay<Construction>();
  rinfo.engine.resetColorMask();

  areaInfo.pos = postTile->epos();
  bool maskSet = false;
  Size size(1,1);

  if( construction.isValid() && construction->canBuild( areaInfo ) )
  {
    rinfo.engine.setColorMask( 0x00000000, 0x0000ff00, 0, 0xff000000 );
    maskSet = true;

    size = construction->size();
  }

  if( postTile == tile && maskSet )
  {
    TilesArray area = _city()->tilemap().area( areaInfo.pos, size );
    for( auto gtile : area )
      drawLandTile( rinfo, *gtile );
  }

  drawLandTile( rinfo, *postTile );

  if( maskSet )
  {
    const Picture& picOver = construction->picture( areaInfo );
    rinfo.engine.draw( picOver, postTile->mappos() + rinfo.offset );
    drawPass( rinfo, *postTile, Renderer::overlayAnimation );
    rinfo.engine.resetColorMask();
  }
}

bool Build::_tryDrawBuildTile(const RenderInfo& rinfo, Tile &tile)
{
  Impl::CachedTiles& cache = _dfunc()->cachedTiles;
  auto it = cache.find( tile::hash( tile.epos() ) );
  if( it != cache.end() )
  {
    _drawBuildTile( rinfo, it->second );
    return true;
  }

  return false;
}

void Build::_drawBuildTiles( const RenderInfo& rinfo )
{
  for( auto tile : _dfunc()->buildTiles )
    _drawBuildTile( rinfo, tile );
}

void Build::drawTile( const RenderInfo& rinfo, Tile& tile )
{
  __D_IMPL(_d,Build);

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
    Point screenPos = tile.mappos() + rinfo.offset;
    drawPass( rinfo, tile, Renderer::ground );
    rinfo.engine.draw( *picOver, screenPos );
    drawPass( rinfo, tile, Renderer::overlayAnimation );
  }
  else if( _d->lastLayer.isValid() )
  {
    _d->lastLayer->drawTile( rinfo, tile );
  }
  else
  {
    Layer::drawTile( rinfo, tile );
  }

  if( !_d->overdrawBuilding )
    _tryDrawBuildTile( rinfo, tile );
}

void Build::drawProminentTile( const RenderInfo& renderInfo, Tile& tile, const int depth, bool force)
{
  Layer::drawProminentTile( renderInfo, tile, depth, force );

  _tryDrawBuildTile( renderInfo, tile );
}

void Build::render( Engine& engine)
{
  __D_IMPL(d,Build);
  Layer::render( engine );

  if( ++d->frameCount >= frameCountLimiter)
  {
    _updatePreviewTiles( true );
  }

  d->frameCount %= frameCountLimiter;

  RenderInfo rinfo = { engine, _camera()->offset() };
  if( d->overdrawBuilding )
    _drawBuildTiles( rinfo );
}

void Build::_initBuildMode()
{
  __D_IMPL(_d,Build);
  auto command = _d->renderer->mode().as<BuildMode>();
  Logger::warningIf( !command.isValid(), "LayerBuild: init unknown command" );

  _d->multiBuilding = command.isValid() ? command->flag( LayerMode::multibuild ) : false;
  _d->roadAssignment = command.isValid() ? command->flag( LayerMode::assign2road ) : false;
  _d->borderBuilding = command.isValid() ? command->flag( LayerMode::border ) : false;
}

void Build::init(Point cursor)
{
  __D_IMPL(_d,Build);
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
    events::dispatch<WarningMessage>( "Press red cross for break/exit, stamp for build", WarningMessage::neitral );
  }
}

void Build::beforeRender(Engine& engine)
{
  __D_IMPL(_d,Build);
  _d->overdrawBuilding = DrawOptions::instance().isFlag( DrawOptions::overdrawOnBuild );
  if( _d->lastLayer.isValid() )
    _d->lastLayer->beforeRender( engine );
  else
    Layer::beforeRender( engine );
}

void Build::drawPass(const RenderInfo& rinfo, Tile& tile, Renderer::Pass pass)
{
  __D_IMPL(_d,Build);
  if( _d->lastLayer.isValid() )
    _d->lastLayer->drawPass( rinfo, tile, pass );
  else
    Layer::drawPass( rinfo, tile, pass );
}

void Build::afterRender(Engine& engine)
{
  __D_IMPL(_d,Build);
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

const Layer::WalkerTypes& Build::visibleTypes() const
{
  __D_IMPL_CONST(_d,Build);
  if( _d->lastLayer.isValid() )
    return _d->lastLayer->visibleTypes();

  return Layer::visibleTypes();
}

void Build::renderUi(Engine &engine)
{
  Layer::renderUi( engine );
  engine.draw( _dfunc()->text.image, engine.cursorPos() + Point( 10, 10 ));
}

void Build::changeLayer(int layer)
{
  __D_IMPL(_d,Build);
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

LayerPtr Build::create(Renderer& renderer, PlayerCityPtr city)
{
  LayerPtr ret( new Build( renderer, city ) );
  ret->drop();

  return ret;
}

LayerPtr Build::drawLayer() const { return _dfunc()->lastLayer; }
Build::~Build() {}

Build::Build( Renderer& renderer, PlayerCityPtr city)
  : Layer( renderer.camera(), city ),
    __INIT_IMPL(Build)
{
  __D_REF(d,Build);
  d.renderer = &renderer;
  d.frameCount = 0;
  d.needUpdateTiles = false;
  d.resForbiden = SETTINGS_STR( forbidenTile );
  d.startTilePos = gfx::tilemap::invalidLocation();
  d.text.font = Font::create( FONT_5 );
  d.readyForExit = false;
  d.text.image = Picture( Size( 100, 30 ), 0, true );
  d.btile.green.load( d.resForbiden, 1 );
  d.btile.red.load( d.resForbiden, 2 );

  _addWalkerType( walker::all );
}

Signal3<object::Type,TilePos,int>& Build::onBuild()
{
  return _dfunc()->onBuildSignal;
}

void Build::Impl::sortBuildTiles()
{
  std::sort( buildTiles.begin(), buildTiles.end(), compare_tile );

  cachedTiles.clear();
  for( auto t : buildTiles )
    cachedTiles[ tile::hash( t->epos() ) ] = t;
}

}//end namespace citylayer
