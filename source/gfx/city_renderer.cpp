// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "city_renderer.hpp"

#include <list>
#include <vector>

#include "tile.hpp"
#include "engine.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "pictureconverter.hpp"
#include "core/event.hpp"
#include "game/roadbuild_helper.hpp"
#include "game/tilemapchangecommand.hpp"
#include "game/tilemap.hpp"
#include "core/stringhelper.hpp"
#include "building/house.hpp"
#include "game/house_level.hpp"
#include "building/watersupply.hpp"
#include "core/foreach.hpp"
#include "events/event.hpp"
#include "core/font.hpp"
#include "gfx/sdl_engine.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "building/constants.hpp"
#include "layerSimple.hpp"
#include "layerwater.hpp"
#include "layerfire.hpp"
#include "layerfood.hpp"
#include "layerhealth.hpp"
#include "layerconstants.hpp"
#include "layerreligion.hpp"
#include "layerdamage.hpp"
#include "layerdesirability.hpp"
#include "layerentertainment.hpp"

using namespace constants;

class CityRenderer::Impl
{
public: 
  typedef std::vector<Tile*> TileQueue;
  typedef std::map<Renderer::Pass, TileQueue> RenderQueue;
  typedef std::vector<LayerPtr> Layers;

  Picture clearPic;
  TilemapTiles postTiles;  // these tiles have draw over "normal" tilemap tiles!
  Point lastCursorPos;
  Point startCursorPos;
  bool  lmbPressed;
  // current map offset, for private use
  Point mapOffset;
  CityPtr city;     // city to display
  Tilemap* tilemap;
  GfxEngine* engine;
  TilemapCamera camera;  // visible map area
  RenderQueue renderQueue;
  Layers layers;

  int scrollSpeed;

  TilePos lastTilePos;
  TilemapChangeCommandPtr changeCommand;

  Layer::VisibleWalkers visibleWalkers;
  LayerPtr currentLayer;


  void getSelectedArea( TilePos& outStartPos, TilePos& outStopPos );
  // returns the tile at the cursor position.

  void buildAll();
  void clearAll();
  void drawTileInSelArea( Tile& tile, Tile* master );
  void drawAnimations();
  void renderTilesRTools();
  void renderTilesBTools();
  void renderTiles();

  void drawTile( Tile& tile );
  void drawTileEx( Tile& tile, const int depth );

  Tile* getTile( const Point& pos, bool overborder);

  WalkerList getVisibleWalkerList();
  void drawWalkersBetweenZ( WalkerList walkerList, int minZ, int maxZ );

  void resetWasDrawn( TilemapArea tiles )
  {
    foreach( Tile* tile, tiles )
      tile->resetWasDrawn();
  }

  void setVisibleWalkers(const WalkerType walkersTypes[])
  {
    visibleWalkers.clear();
    int i=0;
    while (walkersTypes[i] != WT_NONE)
    {
      visibleWalkers.push_back(walkersTypes[i++]);
    }
  }

oc3_signals public:
  Signal1< const Tile& > onShowTileInfoSignal;
  Signal1< std::string > onWarningMessageSignal;
};

CityRenderer::CityRenderer() : _d( new Impl )
{
}

CityRenderer::~CityRenderer() {}

void CityRenderer::initialize( CityPtr city, GfxEngine* engine )
{
  _d->visibleWalkers.push_back(WT_ALL);
  _d->scrollSpeed = 4;
  _d->city = city;
  _d->tilemap = &city->getTilemap();
  _d->camera.init( *_d->tilemap );
  _d->engine = engine;
  _d->clearPic = Picture::load( "oc3_land", 2 );
  _d->currentLayer = _d->layers.front();

  addLayer( LayerSimple::create( this, city ) );
  addLayer( LayerWater::create( this, city ) );
  addLayer( LayerFire::create( this, city ) );
  addLayer( LayerFood::create( this, city ) );
  addLayer( LayerHealth::create( this, city, citylayer::health ));
  addLayer( LayerHealth::create( this, city, citylayer::doctor ));
  addLayer( LayerHealth::create( this, city, citylayer::hospital ));
  addLayer( LayerHealth::create( this, city, citylayer::barber ));
  addLayer( LayerHealth::create( this, city, citylayer::baths ));
  addLayer( LayerReligion::create( this, city ) );
  addLayer( LayerDamage::create( this, city ) );
  addLayer( LayerDesirability::create( this, city ) );
  addLayer( LayerEntertainment::create( this, city, citylayer::entertainment ) );
  addLayer( LayerEntertainment::create( this, city, citylayer::theater ) );
  addLayer( LayerEntertainment::create( this, city, citylayer::amphitheater ) );
  addLayer( LayerEntertainment::create( this, city, citylayer::colloseum ) );
  addLayer( LayerEntertainment::create( this, city, citylayer::hippodrome ) );
}

void CityRenderer::Impl::drawTileEx( Tile& tile, const int depth )
{
  if( tile.isFlat() )
  {
    return;  // tile has already been drawn!
  }

  Tile* master = tile.getMasterTile();

  if( 0 == master )    // single-tile
  {
    drawTile( tile );
    return;
  }

  // multi-tile: draw the master tile.
  // and it is time to draw the master tile
  if( master->getIJ().getZ() == depth && !master->getFlag( Tile::wasDrawn ) )
  {
    drawTile( *master );
  }
}

void CityRenderer::Impl::drawTile( Tile& tile )
{
  currentLayer->drawTile( *engine, tile, mapOffset );
}

void CityRenderer::Impl::drawAnimations()
{
  // building foregrounds and animations
  TileQueue& tiles = renderQueue[ Renderer::animations ];
  foreach( Tile* tile, tiles )
  {
    currentLayer->drawTilePass( *engine, *tile, mapOffset, Renderer::animations );
  }

  tiles.clear();
}

void CityRenderer::Impl::drawTileInSelArea( Tile& tile, Tile* master )
{
  if( master==NULL )
  {
    // single-tile
    currentLayer->drawTile( *engine, tile, mapOffset );
    engine->drawPicture( clearPic, tile.getXY() + mapOffset );
  }
  else
  {
    engine->setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );

    // multi-tile: draw the master tile.
    if( !master->getFlag( Tile::wasDrawn ) )
      currentLayer->drawTile( *engine, *master, mapOffset );

    engine->resetTileDrawMask();
  }
}

void CityRenderer::Impl::renderTilesRTools()
{
  // center the map on the screen
  mapOffset = Point( engine->getScreenWidth() / 2 - 30 * (camera.getCenterX() + 1) + 1,
                     engine->getScreenHeight() / 2 + 15 * (camera.getCenterZ()-tilemap->getSize() + 1) - 30 );

  int lastZ = -1000;  // dummy value

  TilemapArea visibleTiles = camera.getTiles();
  resetWasDrawn( visibleTiles );

  TilePos startPos, stopPos;
  getSelectedArea( startPos, stopPos );
 
  std::set<int> hashDestroyArea;
  TilemapArea destroyArea = tilemap->getArea( startPos, stopPos );
  
  //create list of destroy tiles add full area building if some of it tile constain in destroy area
  foreach( Tile* tile, destroyArea)
  {
    hashDestroyArea.insert( tile->getJ() * 1000 + tile->getI() );

    TileOverlayPtr overlay = tile->getOverlay();
    if( overlay.isValid() )
    {
      TilemapArea overlayArea = tilemap->getArea( overlay->getTilePos(), overlay->getSize() );
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
      drawTileInSelArea( *tile, master );
    }
    else
    {
      if( master==NULL )
      {
        // single-tile
        drawTile( *tile );
      }
      else if( !master->getFlag( Tile::wasDrawn ) )
      {
        // multi-tile: draw the master tile.
        drawTile( *master );
      }    
    }
  }  

  // SECOND PART: draw all sprites, impassable land and buildings
  WalkerList walkerList = getVisibleWalkerList();
  foreach( Tile* tile, visibleTiles )
  {
    int z = tile->getIJ().getZ();

    if (z != lastZ)
    {
      // TODO: pre-sort all animations
      lastZ = z;
      this->drawWalkersBetweenZ(walkerList, z, z+1);
    }   

    int tilePosHash = tile->getJ() * 1000 + tile->getI();
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      engine->setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );      
    }

    drawTileEx( *tile, z );
    engine->resetTileDrawMask();
  }
}

void CityRenderer::Impl::renderTiles()
{
  // center the map on the screen
  mapOffset = Point( engine->getScreenWidth() / 2 - 30 * (camera.getCenterX() + 1) + 1,
                     engine->getScreenHeight() / 2 + 15 * (camera.getCenterZ() - tilemap->getSize() + 1) - 30 );

  int lastZ = -1000;  // dummy value

  TilemapArea visibleTiles = camera.getTiles();

  foreach( Tile* tile, visibleTiles )
  {
    tile->resetWasDrawn();
  }

  // FIRST PART: draw all flat land (walkable/boatable)
  foreach( Tile* tile, visibleTiles )
  {
    Tile* master = tile->getMasterTile();

    if( !tile->isFlat() )
      continue;

    if( master==NULL )
    {
      // single-tile
      drawTile( *tile );
    }
    else
    {
      // multi-tile: draw the master tile.
      if( !master->getFlag( Tile::wasDrawn ) )
        drawTile( *master );
    }    
  }  

  // SECOND PART: draw all sprites, impassable land and buildings
  WalkerList walkerList = getVisibleWalkerList();

  foreach( Tile* tile, visibleTiles )
  {
    int z = tile->getIJ().getZ();

    if (z != lastZ)
    {
      // TODO: pre-sort all animations
      lastZ = z;
      drawWalkersBetweenZ( walkerList, z, z+1);
    }   

    drawTileEx( *tile, z );
  }
}

void CityRenderer::registerTileForRendering(Tile& tile)
{
  if( tile.getOverlay() != 0 )
  {
    Renderer::PassQueue passQueue = tile.getOverlay()->getPassQueue();
    foreach( Renderer::Pass pass, passQueue )
    {
      _d->renderQueue[ pass ].push_back( &tile );
    }
  }
}

const TilemapTiles&CityRenderer::getPostTiles() const
{
  return _d->postTiles;
}

void CityRenderer::render()
{
  //First part: drawing city
  if( _d->changeCommand.isValid() && _d->changeCommand.is<TilemapRemoveCommand>() )
  {
    _d->renderTilesRTools();
  }
  else
  {
    _d->renderTiles();
  }

  //Second part: drawing build tools
  _d->renderTilesBTools();

  _d->drawAnimations();
}

Tile* CityRenderer::getTile( const Point& pos, bool overborder )
{
  return _d->getTile( pos, overborder );
}

void CityRenderer::Impl::renderTilesBTools()
{
  if( changeCommand.isValid() && changeCommand.is<TilemapBuildCommand>() )
  {
    foreach( Tile* postTile, postTiles )
    {
      postTile->resetWasDrawn();

      ConstructionPtr ptr_construction = postTile->getOverlay().as<Construction>();
      engine->resetTileDrawMask();

      if (ptr_construction != NULL)
      {
        if (ptr_construction->canBuild( city, postTile->getIJ()))
        {
          engine->setTileDrawMask( 0x00000000, 0x0000ff00, 0, 0xff000000 );

          // aqueducts must be shown in correct form
          AqueductPtr aqueduct = ptr_construction.as<Aqueduct>();
          if (aqueduct != NULL)
            aqueduct->setPicture(aqueduct->computePicture( city, &postTiles, postTile->getIJ()));
        }
      }

      drawTileEx( *postTile, postTile->getIJ().getZ() );
    }

    engine->resetTileDrawMask();
  }
}

Tile* CityRenderer::Impl::getTile( const Point& pos, bool overborder)
{
  Point mOffset = pos - mapOffset;  // x relative to the left most pixel of the tilemap
  int i = (mOffset.getX() + 2 * mOffset.getY()) / 60;
  int j = (mOffset.getX() - 2 * mOffset.getY()) / 60;
  
  if( overborder )
  {
      i = math::clamp( i, 0, tilemap->getSize() - 1 );
      j = math::clamp( j, 0, tilemap->getSize() - 1 );
  }
  // std::cout << "ij ("<<i<<","<<j<<")"<<std::endl;

  if (i>=0 && j>=0 && i < tilemap->getSize() && j < tilemap->getSize())
  {
     // valid coordinate
     return &tilemap->at( TilePos( i, j ) );
  }
  else
  {
     // the pixel is outside the tilemap => no tile here
     return NULL;
  }
}

TilemapCamera& CityRenderer::getCamera()
{
  return _d->camera;
}

void CityRenderer::updatePreviewTiles( bool force )
{
  TilemapBuildCommandPtr bldCommand = _d->changeCommand.as<TilemapBuildCommand>();

  if( bldCommand.isNull() )
    return;

  if( !bldCommand->isMultiBuilding() )
    _d->startCursorPos = _d->lastCursorPos;

  Tile* curTile = getTile( _d->lastCursorPos, true );

  if( !curTile )
    return;

  if( curTile && !force && _d->lastTilePos == curTile->getIJ() )
    return;

  _d->lastTilePos = curTile->getIJ();

  discardPreview();

  if( bldCommand->isBorderBuilding() )
  {
    Tile* startTile = getTile( _d->startCursorPos, true );  // tile under the cursor (or NULL)
    Tile* stopTile  = getTile( _d->lastCursorPos,  true );

    ConstTilemapWay pathWay = RoadPropagator::createPath( *_d->tilemap, *startTile, *stopTile );
    for( ConstTilemapWay::iterator it=pathWay.begin(); it != pathWay.end(); it++ )
    {
      checkPreviewBuild( (*it)->getIJ() );
    }
  }
  else
  {
    TilePos startPos, stopPos;
    _d->getSelectedArea( startPos, stopPos );

    std::cout << "start is" << startPos << " and stop is " << stopPos << std::endl;

    for( int i = startPos.getI(); i <= stopPos.getI(); i++ )
    {
      for( int j = startPos.getJ(); j <=stopPos.getJ(); j++ )
      {
        checkPreviewBuild( TilePos( i, j ) );
      }
    }
  }
}

void CityRenderer::Impl::getSelectedArea( TilePos& outStartPos, TilePos& outStopPos )
{
  Tile* startTile = getTile( startCursorPos, true );  // tile under the cursor (or NULL)
  Tile* stopTile  = getTile( lastCursorPos, true );

  TilePos startPosTmp = startTile->getIJ();
  TilePos stopPosTmp  = stopTile->getIJ();

//  std::cout << "TilemapRenderer::_getSelectedArea" << " ";
//  std::cout << "(" << startPosTmp.getI() << " " << startPosTmp.getJ() << ") (" << stopPosTmp.getI() << " " << stopPosTmp.getJ() << ")" << std::endl;
    
  outStartPos = TilePos( std::min<int>( startPosTmp.getI(), stopPosTmp.getI() ), std::min<int>( startPosTmp.getJ(), stopPosTmp.getJ() ) );
  outStopPos  = TilePos( std::max<int>( startPosTmp.getI(), stopPosTmp.getI() ), std::max<int>( startPosTmp.getJ(), stopPosTmp.getJ() ) );
}

void CityRenderer::Impl::clearAll()
{
  TilePos startPos, stopPos;
  getSelectedArea( startPos, stopPos );

  TilemapTiles tiles4clear = tilemap->getArea( startPos, stopPos );
  foreach( Tile* tile, tiles4clear )
  {
    events::GameEventPtr event = events::ClearLandEvent::create( tile->getIJ() );
    event->dispatch();
  }
}

void CityRenderer::Impl::buildAll()
{
  TilemapBuildCommandPtr bldCommand = changeCommand.as<TilemapBuildCommand>();
  if( bldCommand.isNull() )
    return;

  ConstructionPtr cnstr = bldCommand->getContruction();

  if( !cnstr.isValid() )
  {
    Logger::warning( "No construction for build" );
    return;
  }

  bool buildOk = false;
  foreach( Tile* tile, postTiles )
  {   
    if( cnstr->canBuild( city, tile->getIJ() ) && tile->isMasterTile())
    {
      events::GameEventPtr event = events::BuildEvent::create( tile->getIJ(), cnstr->getType() );
      event->dispatch();
      buildOk = true;
    }   
  }

  if( !buildOk )
  {
    std::string errorStr = cnstr->getError();

    onWarningMessageSignal.emit( errorStr.empty()
                                 ? _("##need_build_on_cleared_area##")
                                 : errorStr );
  }
}

void CityRenderer::Impl::drawWalkersBetweenZ(WalkerList walkerList, int minZ, int maxZ)
{
  PicturesArray pictureList;

  foreach( WalkerPtr walker, walkerList )
  {
    // TODO: calculate once && sort
    int zAnim = walker->getIJ().getZ();// getJ() - walker.getI();
    if( zAnim > minZ && zAnim <= maxZ )
    {
      pictureList.clear();
      walker->getPictureList( pictureList );
      foreach( Picture& picRef, pictureList )
      {
        if( picRef.isValid() )
        {
          engine->drawPicture( picRef, walker->getPosition() + mapOffset );
        }
      }
    }
  }
}

WalkerList CityRenderer::Impl::getVisibleWalkerList()
{
  WalkerList walkerList;
  foreach( WalkerType wtAct, visibleWalkers )
  {
    WalkerList foundWalkers = city->getWalkerList( wtAct );
    walkerList.insert(walkerList.end(), foundWalkers.begin(), foundWalkers.end());
  }

  return walkerList;
}

void CityRenderer::handleEvent( NEvent& event )
{
    if( event.EventType == sEventMouse )
    {
        switch( event.MouseEvent.Event  )
        {
        case mouseMoved:
        {
            _d->lastCursorPos = event.MouseEvent.getPosition();  
            if( !_d->lmbPressed || _d->startCursorPos.getX() < 0 )
            {
                _d->startCursorPos = _d->lastCursorPos;
            }
           
            updatePreviewTiles();
        }
        break;        

        case mouseLbtnPressed:
        {
            _d->startCursorPos = _d->lastCursorPos;
            _d->lmbPressed = true;
            updatePreviewTiles();
        }
        break;

        case mouseLbtnRelease:            // left button
        {
            Tile* tile = _d->getTile( event.MouseEvent.getPosition(), false );  // tile under the cursor (or NULL)
            if( tile == 0 )
            {
              _d->lmbPressed = false;
              break;
            }

            if( _d->changeCommand.isValid() )
            {                
                if( _d->changeCommand.is<TilemapRemoveCommand>() )
                {
                    _d->clearAll();                      
                }
                else if( _d->changeCommand.is<TilemapBuildCommand>() )
                {
                    _d->buildAll();               
                }
                _d->startCursorPos = _d->lastCursorPos;
                updatePreviewTiles( true );
            }
            else
            {
                getCamera().setCenter( tile->getIJ() );
                _d->city->setCameraPos( tile->getIJ() );
            }

            _d->lmbPressed = false;
            _d->startCursorPos = _d->lastCursorPos;
        }
        break;

        case mouseRbtnRelease:
        {
            Tile* tile = _d->getTile( event.MouseEvent.getPosition(), false );  // tile under the cursor (or NULL)
            if( _d->changeCommand.isValid() )
            { 
                _d->changeCommand = TilemapChangeCommandPtr();
                discardPreview();
            }
            else
            {
              if( tile )
              {
                _d->onShowTileInfoSignal.emit( *tile );
              }
            }         
        }
        break;

        default:
        break;
        }
    }  

    if( event.EventType == sEventKeyboard )
    {
      int moveValue = _d->scrollSpeed * ( event.KeyboardEvent.Shift ? 2 : 0 ) ;
      switch( event.KeyboardEvent.Key )
      {
      case KEY_UP: getCamera().moveUp( moveValue  ); break;
      case KEY_DOWN: getCamera().moveDown( moveValue ); break;
      case KEY_RIGHT: getCamera().moveRight( moveValue ); break;
      case KEY_LEFT: getCamera().moveLeft( moveValue ); break;
      default: break;
      }
    }
}

void CityRenderer::discardPreview()
{
  foreach( Tile* tile, _d->postTiles )
  {
    if( tile->getOverlay().isValid() )
    {
      tile->getOverlay()->deleteLater();
    }

    delete tile;
  }

  _d->postTiles.clear();
}

void CityRenderer::checkPreviewBuild(const TilePos & pos)
{
  TilemapBuildCommandPtr bldCommand = _d->changeCommand.as<TilemapBuildCommand>();

  if (bldCommand.isNull())
    return;

  // TODO: do only when needed, when (i, j, _buildInstance) has changed
  ConstructionPtr overlay = bldCommand->getContruction();

  if (!overlay.isValid()) {
    return;
  }

  int size = overlay->getSize().getWidth();

  if( overlay->canBuild( _d->city, pos ) )
  {
    bldCommand->setCanBuild(true);
    Tile *masterTile=0;
    for (int dj = 0; dj < size; ++dj)
    {
      for (int di = 0; di < size; ++di)
      {
        Tile* tile = new Tile(_d->tilemap->at( pos + TilePos( di, dj ) ));  // make a copy of tile

        if (di==0 && dj==0)
        {
          // this is the masterTile
          masterTile = tile;
        }
        tile->setPicture( &overlay->getPicture() );
        tile->setMasterTile( masterTile );
        tile->setFlag( Tile::tlBuilding, true );
        tile->setOverlay( overlay.as<TileOverlay>() );
        _d->postTiles.push_back( tile );
        //_priorityTiles.push_back( tile );
      }
    }
  }
  else
  {
    bldCommand->setCanBuild(false);

    Picture& grnPicture = Picture::load("oc3_land", 1);
    Picture& redPicture = Picture::load("oc3_land", 2);

    for (int dj = 0; dj < size; ++dj)
    {
      for (int di = 0; di < size; ++di)
      {
        TilePos rPos = pos + TilePos( di, dj );
        if( !_d->tilemap->isInside( rPos ) )
          continue;

        Tile* tile = new Tile( _d->tilemap->at( rPos ) );  // make a copy of tile

        bool isConstructible = tile->getFlag( Tile::isConstructible );
        tile->setPicture( isConstructible ? &grnPicture : &redPicture );
        tile->setMasterTile( 0 );
        tile->setFlag( Tile::clearAll, true );
        tile->setFlag( Tile::tlBuilding, true );
        tile->setOverlay( 0 );
        _d->postTiles.push_back( tile );
      }
    }
  }
}

Tile* CityRenderer::getTile(const TilePos& pos )
{
  return &_d->tilemap->at( pos );
}

Signal1< const Tile& >& CityRenderer::onShowTileInfo()
{
  return _d->onShowTileInfoSignal;
}

void CityRenderer::setMode( const TilemapChangeCommandPtr command )
{
  _d->changeCommand = command;
  _d->startCursorPos = _d->lastCursorPos;
  _d->lmbPressed = false;

  updatePreviewTiles();

  if( _d->changeCommand.is<TilemapOverlayCommand>() )
  {
    TilemapOverlayCommandPtr ovCmd = _d->changeCommand.as<TilemapOverlayCommand>();
    _d->currentLayer = 0;
    int layerName = ovCmd->getType();
    foreach( LayerPtr layer, _d->layers )
    {
      if( layer->getType() == layerName )
      {
        _d->currentLayer = layer;
        break;
      }
    }

    if( _d->currentLayer.isNull() )
    {
      _d->currentLayer = _d->layers.front();
    }

    _d->visibleWalkers = _d->currentLayer->getVisibleWalkers();
    _d->changeCommand = TilemapChangeCommandPtr();
  }
}

void CityRenderer::animate(unsigned int time)
{
  TilemapArea visibleTiles = _d->camera.getTiles();

  foreach( Tile* tile, visibleTiles )
  {
    tile->animate( time );
  }
}

void CityRenderer::setScrollSpeed(int value)
{
  _d->scrollSpeed = value;
}

void CityRenderer::addLayer(LayerPtr layer)
{
  _d->layers.push_back( layer );
}

Point CityRenderer::getOffset() const
{
  return _d->mapOffset;
}

Signal1< std::string >& CityRenderer::onWarningMessage()
{
  return _d->onWarningMessageSignal;
}

Tilemap& CityRenderer::getTilemap()
{ 
  return *_d->tilemap;
}
