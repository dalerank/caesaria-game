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
#include "objects/tree.hpp"
#include "game/settings.hpp"
#include "walker/walker.hpp"
#include "objects/objects_factory.hpp"
#include "gfx/tilearea.hpp"
#include "gfx/city_renderer.hpp"
#include "gfx/helper.hpp"
#include "gfx/tilemap.hpp"
#include "objects/coast.hpp"
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
  bool splineEnabled;
  int drawRadius;  
  bool kbShift, kbCtrl;
  bool lmbPressed;
  bool overdrawBuilding;
  bool borderBuilding;
  bool roadAssignment;
  bool needUpdateTiles;
  int drawLayerIndex;
  int frameCount;
  Renderer* renderer;
  //LayerPtr lastLayer;
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
  bool canBuildOn( OverlayPtr overlay, const city::AreaInfo& areaInfo) const;
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

void Constructor::_checkPreviewBuild(const TilePos& pos)
{
  __D_REF(d,Constructor);
  auto command =  d.renderer->mode().as<EditorMode>();

  if (command.isNull())
    return;

  // TODO: do only when needed, when (i, j, _buildInstance) has changed
  OverlayPtr overlay = command->overlay();

  if( !overlay.isValid() )
  {
    return;
  }

  Size size = overlay->size();

  city::AreaInfo areaInfo( _city(), pos, &d.buildTiles );
  if( d.canBuildOn( overlay, areaInfo ) )
  {
    Tilemap& tmap = _city()->tilemap();
    Tile *masterTile=0;
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
        auto clone = Overlay::create( overlay->type() );
        tile->setPicture( tmap.at( pos + TilePos( di, dj ) ).picture() );
        tile->setMaster( masterTile );
        tile->setOverlay( clone );
        bool added = d.buildTiles.appendOnce( tile );
        if( !added )
        {
          delete tile;
        }
      }
    }
  }
  /*else
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
  }*/
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
    {
      if( !tile )
      {
        Logger::warning( "WARNING !!!: ConstructorArea: tile is null" );
        continue;
      }
      Size size;
      Tile* master = tile->master() ? tile->master() : tile;
      if( tile->overlay().isValid() )
      {
         size = tile->overlay()->size();
      }
      else
      {
         size = Size( gfx::tilemap::picWidth2CellSize( tile->picture().width() ) );
      }

      if( size.area() > 1 )
      {
        TilesArray apTiles = _city()->tilemap().area( master->epos(), size );
        for( auto aptile : apTiles )
          tiles.appendOnce( aptile );
      }
    }

    for( auto tile : tiles )
      _checkPreviewBuild( tile->epos() );
  }

  d->sortBuildTiles();

  /*d->text.image.fill( 0x0, Rect() );
  d->text.font.setColor( 0xffff0000 );
  d->text.font.draw( d->text.image, utils::i2str( d->money4Construction ) + " Dn", Point() );*/
}

void Constructor::_buildAll()
{
  __D_IMPL(d,Constructor);
  auto command = d->renderer->mode().as<EditorMode>();
  if( command.isNull() )
    return;

  if( d->buildTiles.empty() )
  {
    Logger::warning( "LayerEditor: No tiles for build" );
    return;
  }  

  city::AreaInfo areaInfo( _city(), TilePos() );
  for( auto tile : d->buildTiles )
  {    
    areaInfo.pos = tile->epos();
    tile->overlay()->build( areaInfo );
  }

  d->startTilePos = d->lastTilePos;
  d->needUpdateTiles = true;
}

void Constructor::_exitBuildMode()
{
  DrawOptions::instance().setFlag( DrawOptions::mayChangeLayer, true );
  _setNextLayer( citylayer::simple );
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

void Constructor::_drawBuildTile( const RenderInfo& renderInfo, Tile* tile )
{
  if( tile == nullptr )
    return;

  __D_IMPL(_d,Constructor);
  city::AreaInfo areaInfo( _city(), TilePos(), &_d->buildTiles );

  Tile* postTile = tile;
  postTile->resetRendered();

  if( postTile->master() )
    postTile = postTile->master();

  auto overlay = postTile->overlay();
  renderInfo.engine.resetColorMask();

  areaInfo.pos = postTile->epos();
  bool maskSet = false;
  Size size(1,1);

  if( overlay.isValid() )
  {
    renderInfo.engine.setColorMask( 0x00000000, 0x0000ff00, 0, 0xff000000 );
    maskSet = true;

    size = overlay->size();
  }

  if( postTile == tile && maskSet )
  {
    TilesArray area = _city()->tilemap().area( areaInfo.pos, size );
    /*for( auto gtile : area )
    {
      drawPass( engine, *gtile, offset, Renderer::ground );
      drawPass( engine, *gtile, offset, Renderer::groundAnimation );
    }*/
  }

  //drawPass( engine, *postTile, offset, Renderer::ground );
  //drawPass( engine, *postTile, offset, Renderer::groundAnimation );

  if( maskSet )
  {
    const Picture& picOver = overlay->picture( areaInfo );
    renderInfo.engine.draw( picOver, postTile->mappos() + renderInfo.offset );
    drawPass( renderInfo, *postTile, Renderer::overlayAnimation );
    renderInfo.engine.resetColorMask();
  }
}

void Constructor::_tryDrawBuildTile( const RenderInfo& renderInfo, Tile &tile)
{
  Impl::CachedTiles& cache = _dfunc()->cachedTiles;
  auto it = cache.find( tile::hash( tile.epos() ) );
  if( it != cache.end() )
  {
    _drawBuildTile( renderInfo, it->second );
  }
}

void Constructor::_drawBuildTiles(const RenderInfo& renderInfo )
{
  for( auto tile : _dfunc()->buildTiles )
    _drawBuildTile( renderInfo, tile );
}

void Constructor::drawTile( const RenderInfo& rinfo, Tile& tile )
{
  __D_REF(d,Constructor);

  auto overlay = tile.overlay();
  city::AreaInfo info( _city(), tile.epos(), &d.buildTiles );

  const Picture* picBasic = 0;
  const Picture* picOver = 0;
  if( overlay.isValid() && info.tiles().size() > 0 )
  {
    picBasic = &overlay->picture();
    picOver = &overlay->picture( info );
  }

  if( picOver && picBasic != picOver )
  {
    Point screenPos = tile.mappos() + rinfo.offset;
    drawPass( rinfo, tile, Renderer::ground );
    rinfo.engine.draw( *picOver, screenPos );
    drawPass( rinfo, tile, Renderer::overlayAnimation );
  }
  /*else if( d.lastLayer.isValid() )
  {
    if( d.cachedTiles.count( tile::hash( tile.epos() ) ) == 0 )
      d.lastLayer->drawTile( rinfo, tile );
  }*/
  else
  {
    Layer::drawTile( rinfo, tile );
  }

  if( !d.overdrawBuilding )
    _tryDrawBuildTile( rinfo, tile );
}

void Constructor::drawProminentTile( const RenderInfo& renderInfo, Tile& tile, const int depth, bool force)
{
  if( _dfunc()->cachedTiles.count( tile::hash( tile.epos() ) ) == 0 )
    Layer::drawProminentTile( renderInfo, tile, depth, force );

  _tryDrawBuildTile( renderInfo, tile );
}

void Constructor::render( Engine& engine)
{
  __D_IMPL(d,Constructor);
  Layer::render( engine );

  /*if( ++d->frameCount >= frameCountLimiter)
  {
    _updatePreviewTiles( true );
  }*/

  d->frameCount %= frameCountLimiter;

  if( d->overdrawBuilding )
    _drawBuildTiles( { engine, _camera()->offset() } );
}

void Constructor::_initBuildMode()
{
  __D_IMPL(_d,Constructor);
  auto command = _d->renderer->mode().as<EditorMode>();
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
    events::dispatch<WarningMessage>( "Press red cross for break/exit, stamp for build", WarningMessage::neitral );
  }
}

void Constructor::beforeRender(Engine& engine)
{
  __D_IMPL(_d,Constructor);
  _d->overdrawBuilding = DrawOptions::instance().isFlag( DrawOptions::overdrawOnBuild );
  Layer::beforeRender( engine );
}

void Constructor::drawPass(const RenderInfo& rinfo, Tile& tile, Renderer::Pass pass)
{
  Layer::drawPass( rinfo, tile, pass );
}

void Constructor::afterRender(Engine& engine)
{
  __D_REF(d,Constructor);
  if( d.needUpdateTiles )
  {
    if( !OSystem::isAndroid() )
    {
      _setLastCursorPos( engine.cursorPos() );
    }

    _checkBuildArea();
    _updatePreviewTiles( false );
  }

#ifdef DEBUG
  const TilesArray& visibleTiles = _camera()->tiles();
  for( auto tile : visibleTiles )
  {
    if( tile->terrain().coast )
    {
      CoastPtr coast = tile->overlay<Coast>();
      if( coast != nullptr )
      {
        _addText( tile->mappos() + Point( 20, -15 ), utils::format( 0xff, "+%x", coast->_rindex ) );
      }
    }
  }
#endif

  Layer::afterRender( engine );
}

const Layer::WalkerTypes& Constructor::visibleTypes() const
{
  return Layer::visibleTypes();
}

void Constructor::renderUi(Engine &engine)
{
  Layer::renderUi( engine );
  engine.draw( _dfunc()->text.image, engine.cursorPos() + Point( 10, 10 ));
}

void Constructor::changeLayer(int layer)
{
  if( layer != type() )
  {
    if( layer == citylayer::destroyd )
    {
       _exitBuildMode();
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
  d->drawRadius = 1;
  d->splineEnabled = false;
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

bool Constructor::Impl::canBuildOn(OverlayPtr overlay, const city::AreaInfo& areaInfo) const
{
  if( overlay->type() == object::terrain
      || overlay->type() == object::water )
  {
    return true;
  }
  else if( overlay->type() == object::tree )
  {
    bool walkable = areaInfo.tile().isWalkable( true );
    bool isTree = is_kind_of<Tree>(areaInfo.tile().overlay());
    return (walkable || isTree );
  }
  else if( overlay->type() == object::rock )
  {
    bool walkable = areaInfo.tile().isWalkable( true );
    bool isRock = areaInfo.tile().terrain().rock;
    return (walkable || isRock );
  }

  return false;
}

}//end namespace citylayer
