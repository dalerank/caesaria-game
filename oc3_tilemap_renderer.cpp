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

#include "oc3_tilemap_renderer.hpp"

#include <list>
#include <vector>

#include "oc3_tile.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_screen_game.hpp"
#include "oc3_positioni.hpp"
#include "oc3_pictureconverter.hpp"
#include "oc3_event.hpp"
#include "oc3_roadpropagator.hpp"
#include "oc3_tilemapchangecommand.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_building_house.hpp"
#include "oc3_house_level.hpp"
#include "oc3_building_watersupply.hpp"
#include "oc3_foreach.hpp"

class TilemapRenderer::Impl
{
public:
  typedef std::vector< Picture > Pictures;
  
  Picture clearPic;
  PtrTilesList postTiles;  // these tiles have draw over "normal" tilemap tiles!
  Point lastCursorPos;
  Point startCursorPos;
  bool  lmbPressed;
  // current map offset, for private use
  Point mapOffset;
  CityPtr city;     // city to display
  Tilemap* tilemap;
  TilemapArea* mapArea;  // visible map area
  ScreenGame* screenGame;
  GfxEngine* engine;

  TilePos lastTilePos;
  TilemapChangeCommandPtr changeCommand;

  typedef Delegate1< Tile& > DrawTileSignature;
  DrawTileSignature drawTileFunction;

  Impl()
  {
    visibleWalkers.push_back(WT_ALL);
  }

  void getSelectedArea( TilePos& outStartPos, TilePos& outStopPos );
  // returns the tile at the cursor position.

  void buildAll();
  void clearAll();

  void drawTileBase( Tile& tile );
  void drawTileWater( Tile& tile );
  void drawTileDesirability( Tile& tile );
  void drawTileFire( Tile& tile );
  void drawTileDamage( Tile& tile );
  void drawTileReligion( Tile& tile );
  void drawTileInSelArea( Tile& tile, Tile* master );
  void drawTileFood( Tile& tile );
  void drawAnimations( LandOverlayPtr overlay, const Point& screenPos );
  void drawColumn( const Point& pos, const int startPicId, const int percent );

  void drawTilemapWithRemoveTools();
  void simpleDrawTilemap();

  void drawTile( Tile& tile );
  void drawTileEx( Tile& tile, const int depth );

  Tile* getTile( const Point& pos, bool overborder);

  template< class X, class Y >
  void setDrawFunction( Y* obj, void (X::*func)( Tile& ) )
  {
    drawTileFunction = makeDelegate( obj, func );
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

private:
  std::vector<WalkerType> visibleWalkers;

  WalkerList getVisibleWalkerList();
  void drawWalkersBetweenZ( WalkerList walkerList, int minZ, int maxZ );
  void drawBuildingAreaTiles( Tile& baseTile, LandOverlayPtr overlay, std::string resourceGroup, int tileId );

oc3_signals public:
  Signal1< const Tile& > onShowTileInfoSignal;
  Signal1< std::string > onWarningMessageSignal;
};

TilemapRenderer::TilemapRenderer() : _d( new Impl )
{
  _d->city = NULL;
  _d->mapArea = NULL;
}

TilemapRenderer::~TilemapRenderer() {}

void TilemapRenderer::init( CityPtr city, TilemapArea &mapArea, ScreenGame *screen)
{
  _d->city = city;
  _d->tilemap = &city->getTilemap();
  _d->mapArea = &mapArea;
  _d->screenGame = screen;
  _d->engine = &GfxEngine::instance();
  _d->clearPic = Picture::load( "oc3_land", 2 );
  _d->setDrawFunction( _d.data(), &Impl::drawTileBase );
}

void TilemapRenderer::Impl::drawTileEx( Tile& tile, const int depth )
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
  if( master->getIJ().getZ() == depth && !master->wasDrawn() )
  {
    drawTile( *master );
  }
}

void TilemapRenderer::Impl::drawTile( Tile& tile )
{
  drawTileFunction( tile );
}

void TilemapRenderer::Impl::drawAnimations( LandOverlayPtr overlay, const Point& screenPos )
{
  // building foregrounds and animations
  Impl::Pictures& fgPictures = overlay->getForegroundPictures();
  foreach( Picture& picRef, fgPictures )
  {
    // skip void picture
    if( picRef.isValid() )
    {
      engine->drawPicture( picRef, screenPos);
    }
  }
}

void TilemapRenderer::Impl::drawTileDesirability( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  const TerrainTile& terrain = tile.getTerrain();
  if( terrain.getOverlay().isNull() )
  {
    //draw background
    if( terrain.isConstructible() && terrain.getDesirability() != 0 )
    {
      int picOffset = math::clamp( terrain.getDesirability() / 16, -5, 6 );
      Picture& pic = Picture::load( ResourceGroup::land2a, 37 + picOffset );
      engine->drawPicture( pic, screenPos );
    }
    else
    {
      engine->drawPicture( tile.getPicture(), screenPos );
    }    
  }
  else
  {   
    LandOverlayPtr overlay = terrain.getOverlay();
    switch( overlay->getType() )
    {
    //roads
    case B_ROAD:
    case B_PLAZA:
      engine->drawPicture( tile.getPicture(), screenPos );
      drawAnimations( overlay, screenPos );
    break;  

    //other buildings
    default:      
      {
        int picOffset = math::clamp( terrain.getDesirability() / 16, -5, 6 );
        Picture& pic = Picture::load( ResourceGroup::land2a, 37 + picOffset );
        PtrTilesList tiles4clear = tilemap->getFilledRectangle( tile.getIJ(), overlay->getSize() );
        foreach( Tile* tile, tiles4clear )
        {
          engine->drawPicture( pic, tile->getXY() + mapOffset );
        }
      }
    break;
    } 
  }
}

void TilemapRenderer::Impl::drawTileFire( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  const TerrainTile& terrain = tile.getTerrain();
  if( terrain.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {   
    LandOverlayPtr overlay = terrain.getOverlay();
    Picture pic;
    int fireLevel = 0;
    switch( overlay->getType() )
    {
    //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
    case B_BURNING_RUINS:
    case B_BURNED_RUINS:
    case B_COLLAPSED_RUINS:
    case B_PREFECTURE:
      pic = tile.getPicture();
      needDrawAnimations = true;
    break;  

      //houses
    case B_HOUSE:
      {
        HousePtr house = overlay.as< House >();
        pic = Picture::load( ResourceGroup::waterOverlay, ( overlay->getSize().getWidth() - 1 )*2 + 11 );
        fireLevel = (int)house->getFireLevel();
        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getNbHabitants() ==0);
      }
    break;

      //other buildings
    default:
      {
        pic = Picture::load( ResourceGroup::waterOverlay, (overlay->getSize().getWidth() - 1)*2 + 1 );
        BuildingPtr building = overlay.as< Building >();
        if( building.isValid() )
        {
          fireLevel = (int)building->getFireLevel();
        }
      }
    break;
    }  

    engine->drawPicture( pic, screenPos );

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( fireLevel >= 0)
    {
      drawColumn( screenPos, 18, fireLevel );
    }
  }
}

void TilemapRenderer::Impl::drawTileDamage( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  const TerrainTile& terrain = tile.getTerrain();
  if( terrain.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {   
    LandOverlayPtr overlay = terrain.getOverlay();
    Picture pic;
    int damageLevel = 0;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
    case B_COLLAPSED_RUINS:
    case B_ENGINEER_POST:
      pic = tile.getPicture();
      needDrawAnimations = true;
      break;  

      //houses
    case B_HOUSE:
      {
        HousePtr house = overlay.as< House >();
        pic = Picture::load( ResourceGroup::waterOverlay, ( overlay->getSize().getWidth() - 1 )*2 + 11 );
        damageLevel = (int)house->getDamageLevel();
        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getNbHabitants() ==0);
      }
      break;

      //other buildings
    default:
      {
        pic = Picture::load( ResourceGroup::waterOverlay, (overlay->getSize().getWidth() - 1)*2 + 1 );
        BuildingPtr building = overlay.as< Building >();
        if( building.isValid() )
        {
          damageLevel = (int)building->getDamageLevel();
        }
      }
      break;
    }  

    engine->drawPicture( pic, screenPos );

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( damageLevel >= 0 )
    {
      drawColumn( screenPos, 15, damageLevel );
    }
  }
}

void TilemapRenderer::Impl::drawTileReligion( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  const TerrainTile& terrain = tile.getTerrain();
  if( terrain.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {   
    LandOverlayPtr overlay = terrain.getOverlay();
    Picture pic;
    int religionLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
    case B_TEMPLE_CERES: case B_TEMPLE_MARS: case B_TEMPLE_MERCURE: case B_TEMPLE_NEPTUNE: case B_TEMPLE_VENUS:
    case B_TEMPLE_ORACLE:
    case B_BIG_TEMPLE_CERES: case B_BIG_TEMPLE_MARS: case B_BIG_TEMPLE_MERCURE: case B_BIG_TEMPLE_NEPTUNE: case B_BIG_TEMPLE_VENUS:
      pic = tile.getPicture();
      needDrawAnimations = true;
    break;  

      //houses
    case B_HOUSE:
      {
        HousePtr house = overlay.as< House >();
        pic = Picture::load( ResourceGroup::waterOverlay, ( overlay->getSize().getWidth() - 1 )*2 + 11 );
        religionLevel = house->getServiceAccess(Service::S_TEMPLE_MERCURE);
        religionLevel += house->getServiceAccess(Service::S_TEMPLE_VENUS);
        religionLevel += house->getServiceAccess(Service::S_TEMPLE_MARS);
        religionLevel += house->getServiceAccess(Service::S_TEMPLE_NEPTUNE);
        religionLevel += house->getServiceAccess(Service::S_TEMPLE_CERES);
        religionLevel = math::clamp( religionLevel / (house->getLevelSpec().getMinReligionLevel()+1), 0, 100 );
        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getNbHabitants() ==0);
      }
    break;

      //other buildings
    default:
      {
        pic = Picture::load( ResourceGroup::waterOverlay, (overlay->getSize().getWidth() - 1)*2 + 1 );
      }
    break;
    }  

    engine->drawPicture( pic, screenPos );

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( religionLevel > 0 )
    {
      drawColumn( screenPos, 9, religionLevel );
    }
  }
}

void TilemapRenderer::Impl::drawTileFood( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  const TerrainTile& terrain = tile.getTerrain();
  if( terrain.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {   
    LandOverlayPtr overlay = terrain.getOverlay();
    Picture pic;
    int foodLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
    case B_MARKET:
    case B_GRANARY:
      pic = tile.getPicture();
      needDrawAnimations = true;
    break;  

      //houses
    case B_HOUSE:
      {
        drawBuildingAreaTiles(tile, overlay, ResourceGroup::foodOverlay, WaterOverlay::base);
        HousePtr house = overlay.as< House >();
        foodLevel = house->getFoodLevel();
        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getNbHabitants() == 0);
      }
      break;

      //other buildings
    default:
      {
        drawBuildingAreaTiles(tile, overlay, ResourceGroup::foodOverlay, WaterOverlay::base);
      }
      break;
    }  

    if ( pic.isValid())
    {
      engine->drawPicture( pic, screenPos );
    }

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( foodLevel >= 0 )
    {
      drawColumn( screenPos, 18, math::clamp( 100 - foodLevel, 0, 100 ) );
    }
  }
}

void TilemapRenderer::Impl::drawTileWater( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  Size areaSize(1);
  const TerrainTile& terrain = tile.getTerrain();
  if( terrain.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {
    LandOverlayPtr overlay = terrain.getOverlay();
    Picture pic;
    switch( overlay->getType() )
    {
      //water buildings
    case B_ROAD:
    case B_PLAZA:
    case B_RESERVOIR:
    case B_FOUNTAIN:
    case B_WELL:
    case B_AQUEDUCT:
      pic = tile.getPicture();
      needDrawAnimations = true;
      areaSize = overlay->getSize();
    break;

    default:
    {
      int tileNumber = 0;
      bool haveWater = terrain.getWaterService( WTR_FONTAIN ) > 0 || terrain.getWaterService( WTR_WELL ) > 0;
      if ( overlay->getType() == B_HOUSE )
      {
        HousePtr h = overlay.as<House>();
        tileNumber = WaterOverlay::inHouse;
        haveWater = haveWater || h->hasServiceAccess(Service::S_FOUNTAIN) || h->hasServiceAccess(Service::S_WELL);
      }
      tileNumber += (haveWater ? WaterOverlay::haveWater : 0);
      tileNumber += terrain.getWaterService( WTR_RESERVOIR ) > 0 ? WaterOverlay::reservoirRange : 0;

      drawBuildingAreaTiles( tile, overlay, ResourceGroup::waterOverlay, WaterOverlay::base + tileNumber );

      pic = Picture::getInvalid();
      areaSize = 0;
      needDrawAnimations = false;
    }
    break;
    }

    if ( pic.isValid() )
    {
      engine->drawPicture( pic, screenPos );
    }

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
  }

  if( !needDrawAnimations && (terrain.isWalkable(true) || terrain.isBuilding()) )
  {
    PtrTilesArea area;
    if( areaSize.getWidth() == 1 )
    {
      area.push_back( &tile );
    }
    else
    {
      area = tilemap->getFilledRectangle( tile.getIJ(), areaSize );
    }

    foreach( Tile* tile, area )
    {
      TerrainTile& curTera = tile->getTerrain();
      int reservoirWater = curTera.getWaterService( WTR_RESERVOIR );
      int fontainWater = curTera.getWaterService( WTR_FONTAIN );

      if( (reservoirWater + fontainWater > 0) && !curTera.isWater() && curTera.getOverlay().isNull() )
      {
        int picIndex = reservoirWater ? WaterOverlay::reservoirRange : 0;
        picIndex |= fontainWater > 0 ? WaterOverlay::haveWater : 0;
        picIndex |= WaterOverlay::skipLeftBorder | WaterOverlay::skipRightBorder;
        engine->drawPicture( Picture::load( ResourceGroup::waterOverlay, picIndex + WaterOverlay::base ), tile->getXY() + mapOffset );
      }
    }
  }
}


void TilemapRenderer::Impl::drawTileBase( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  LandOverlayPtr overlay = tile.getTerrain().getOverlay();

  if (!overlay.isNull())
    if (overlay.is<Aqueduct>() && postTiles.size() > 0) {
      // check, do we have any aqueducts there... there can be empty items
      bool isAqueducts = false;
      for (std::list<Tile*>::iterator it = postTiles.begin(); it != postTiles.end(); ++it)
        if ((*it)->getTerrain().getOverlay().is<Aqueduct>()) {
          isAqueducts = true;
          break;
        }

      if (isAqueducts) {
        tile.setWasDrawn();
        Picture& pic = overlay.as<Aqueduct>()->computePicture(&postTiles, tile.getIJ());
        engine->drawPicture( pic, screenPos );
      }
    }

  if (!tile.wasDrawn())
  {
    tile.setWasDrawn();
    engine->drawPicture( tile.getPicture(), screenPos );
  }

  if (overlay.isNull())
    return;

  drawAnimations( overlay, screenPos );
}

void TilemapRenderer::Impl::drawTileInSelArea( Tile& tile, Tile* master )
{
  if( master==NULL )
  {
    // single-tile
    drawTileFunction( tile );
    engine->drawPicture( clearPic, tile.getXY() + mapOffset );
  }
  else
  {
    engine->setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );

    // multi-tile: draw the master tile.
    if( !master->wasDrawn() )
      drawTileFunction( *master );

    engine->resetTileDrawMask();
  }
}

void TilemapRenderer::Impl::drawTilemapWithRemoveTools()
{
  // center the map on the screen
  mapOffset = Point( engine->getScreenWidth() / 2 - 30 * (mapArea->getCenterX() + 1) + 1,
                     engine->getScreenHeight() / 2 + 15 * (mapArea->getCenterZ()-tilemap->getSize() + 1) - 30 );

  int lastZ = -1000;  // dummy value

  std::vector< TilePos > tiles = mapArea->getTiles();
  mapArea->resetWasDrawn();

  TilePos startPos, stopPos;
  getSelectedArea( startPos, stopPos );
 
  std::set<int> hashDestroyArea;
  PtrTilesArea destroyArea = tilemap->getFilledRectangle( startPos, stopPos );
  
  //create list of destroy tiles add full area building if some of it tile constain in destroy area
  foreach( Tile* tile, destroyArea)
  {
    hashDestroyArea.insert( tile->getJ() * 1000 + tile->getI() );

    LandOverlayPtr overlay = tile->getTerrain().getOverlay();
    if( overlay.isValid() )
    {
      PtrTilesArea overlayArea = tilemap->getFilledRectangle( overlay->getTilePos(), overlay->getSize() );
      foreach( Tile* ovelayTile, overlayArea )
      {
        hashDestroyArea.insert( ovelayTile->getJ() * 1000 + ovelayTile->getI() );
      }
    }
  }
  //Rect destroyArea = Rect( startPos.getI(), startPos.getJ(), stopPos.getI(), stopPos.getJ() );

  // FIRST PART: draw all flat land (walkable/boatable)
  foreach( TilePos tilePos, tiles )
  {
    Tile& tile = tilemap->at( tilePos );
    Tile* master = tile.getMasterTile();

    if( !tile.isFlat() )
      continue;

    int tilePosHash = tilePos.getJ() * 1000 + tilePos.getI();
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      drawTileInSelArea( tile, master );  
    }
    else
    {
      if( master==NULL )
      {
        // single-tile
        drawTile( tile );
      }
      else if( !master->wasDrawn() )
      {
        // multi-tile: draw the master tile.
        drawTile( *master );
      }    
    }
  }  

  // SECOND PART: draw all sprites, impassable land and buildings
  WalkerList walkerList = getVisibleWalkerList();
  foreach( TilePos tilePos, tiles )
  {
    int z = tilePos.getZ();

    if (z != lastZ)
    {
      // TODO: pre-sort all animations
      lastZ = z;
      this->drawWalkersBetweenZ(walkerList, z, z+1);
    }   

    int tilePosHash = tilePos.getJ() * 1000 + tilePos.getI();
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      engine->setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );      
    }

    drawTileEx( tilemap->at( tilePos ), z );
    engine->resetTileDrawMask();
  }
}

void TilemapRenderer::Impl::simpleDrawTilemap()
{
  // center the map on the screen
  mapOffset = Point( engine->getScreenWidth() / 2 - 30 * (mapArea->getCenterX() + 1) + 1,
                     engine->getScreenHeight() / 2 + 15 * (mapArea->getCenterZ()-tilemap->getSize() + 1) - 30 );

  int lastZ = -1000;  // dummy value

  std::vector< TilePos >& tiles = const_cast< std::vector< TilePos >& >( mapArea->getTiles() );

  foreach( TilePos tilePos, tiles )
  {
    tilemap->at( tilePos ).resetWasDrawn();
  }

  // FIRST PART: draw all flat land (walkable/boatable)
  foreach( TilePos tilePos, tiles )
  {
    Tile& tile = tilemap->at( tilePos );
    Tile* master = tile.getMasterTile();

    if( !tile.isFlat() )
      continue;

    if( master==NULL )
    {
      // single-tile
      drawTile( tile );
    }
    else
    {
      // multi-tile: draw the master tile.
      if( !master->wasDrawn() )
        drawTile( *master );
    }    
  }  

  // SECOND PART: draw all sprites, impassable land and buildings
  WalkerList walkerList = getVisibleWalkerList();

  foreach( TilePos tilePos, tiles )
  {
    int z = tilePos.getZ();

    if (z != lastZ)
    {
      // TODO: pre-sort all animations
      lastZ = z;
      drawWalkersBetweenZ( walkerList, z, z+1);
    }   

    drawTileEx( tilemap->at( tilePos ), z );
  }
}



void TilemapRenderer::drawTilemap()
{
  //First part: drawing city
  if( _d->changeCommand.isValid() && _d->changeCommand.is<TilemapRemoveCommand>() )
  {
    _d->drawTilemapWithRemoveTools();
  }
  else
  {
    _d->simpleDrawTilemap();
  }

  //Second part: drawing build tools
  if( _d->changeCommand.isValid() && _d->changeCommand.is<TilemapBuildCommand>() )
  {
    foreach( Tile* postTile, _d->postTiles )
    {
      postTile->resetWasDrawn();

      ConstructionPtr ptr_construction = postTile->getTerrain().getOverlay().as<Construction>();
      _d->engine->resetTileDrawMask();

      if (ptr_construction != NULL) {
        if (ptr_construction->canBuild(postTile->getIJ())) {
          _d->engine->setTileDrawMask( 0x00000000, 0x0000ff00, 0, 0xff000000 );

          // aqueducts must be shown in correct form
          AqueductPtr aqueduct = ptr_construction.as<Aqueduct>();
          if (aqueduct != NULL)
            aqueduct->setPicture(aqueduct->computePicture(&_d->postTiles, postTile->getIJ()));
        }
      }

      _d->drawTileEx( *postTile, postTile->getIJ().getZ() );
    }
    _d->engine->resetTileDrawMask();
  }
}

Tile* TilemapRenderer::getTile( const Point& pos, bool overborder )
{
  return _d->getTile( pos, overborder );
}

Tile* TilemapRenderer::Impl::getTile( const Point& pos, bool overborder)
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

TilemapArea& TilemapRenderer::getMapArea()
{
  return *_d->mapArea;
}

void TilemapRenderer::updatePreviewTiles( bool force )
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

    ConstWayOnTiles pathWay = RoadPropagator::createPath( *_d->tilemap, *startTile, *stopTile );
    for( ConstWayOnTiles::iterator it=pathWay.begin(); it != pathWay.end(); it++ )
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

void TilemapRenderer::Impl::getSelectedArea( TilePos& outStartPos, TilePos& outStopPos )
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

void TilemapRenderer::Impl::clearAll()
{
  TilePos startPos, stopPos;
  getSelectedArea( startPos, stopPos );

  PtrTilesList tiles4clear = tilemap->getFilledRectangle( startPos, stopPos );
  foreach( Tile* tile, tiles4clear )
  {
    city->clearLand( tile->getIJ() );
  }
}

void TilemapRenderer::Impl::buildAll()
{
  TilemapBuildCommandPtr bldCommand = changeCommand.as<TilemapBuildCommand>();
  if( bldCommand.isNull() )
    return;

  ConstructionPtr cnstr = bldCommand->getContruction();

  if( !cnstr.isValid() )
  {
    StringHelper::debug( 0xff, "No construction for build" );
    return;
  }

  bool buildOk = false;
  foreach( Tile* tile, postTiles )
  {   
    if( cnstr->canBuild( tile->getIJ() ) && tile->isMasterTile())
    {
      city->build( cnstr->getType(), tile->getIJ() );
      buildOk = true;
    }   
  }

  if( !buildOk )
  {
    onWarningMessageSignal.emit( "##need_build_on_cleared_area##" ); 
  }
}

void TilemapRenderer::Impl::drawColumn( const Point& pos, const int startPicId, const int percent )
{
  engine->drawPicture( Picture::load( ResourceGroup::sprites, startPicId + 2 ), pos + Point( 5, 15 ) );
  
  int roundPercent = ( percent / 10 ) * 10;
  Picture& pic = Picture::load( ResourceGroup::sprites, startPicId + 1 );
  for( int offsetY=10; offsetY < roundPercent; offsetY += 10 )
  {
    engine->drawPicture( pic, pos - Point( -13, -5 + offsetY ) );
  }

  if( percent >= 10 )
  {
    engine->drawPicture( Picture::load( ResourceGroup::sprites, startPicId ), pos - Point( -1, -6 + roundPercent ) );
  }
}

void TilemapRenderer::Impl::drawWalkersBetweenZ(WalkerList walkerList, int minZ, int maxZ)
{
  Impl::Pictures pictureList;

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

void TilemapRenderer::Impl::drawBuildingAreaTiles(Tile& baseTile, LandOverlayPtr overlay, std::string resourceGroup, int tileId)
{
  PtrTilesArea area;
  Size areaSize = overlay->getSize();
  if( areaSize.getWidth() == 1 )
  {
    area.push_back( &baseTile );
  }
  else
  {
    area = tilemap->getFilledRectangle( baseTile.getIJ(), areaSize );
  }

  Picture *pic = NULL;
  int leftBorderAtI = baseTile.getI();
  int rightBorderAtJ = areaSize.getHeight() - 1 + baseTile.getJ();
  foreach( Tile* tile, area )
  {
    int tileBorders = ( tile->getI() == leftBorderAtI ? 0 : WaterOverlay::skipLeftBorder )
                        + ( tile->getJ() == rightBorderAtJ ? 0 : WaterOverlay::skipRightBorder );
    pic = &Picture::load(resourceGroup, tileBorders + tileId);
    engine->drawPicture( *pic, tile->getXY() + mapOffset );
  }
}

WalkerList TilemapRenderer::Impl::getVisibleWalkerList()
{
  WalkerList walkerList;
  foreach( WalkerType wtAct, visibleWalkers )
  {
    WalkerList foundWalkers = city->getWalkerList( wtAct );
    walkerList.insert(walkerList.end(), foundWalkers.begin(), foundWalkers.end());
  }

  return walkerList;
}

void TilemapRenderer::handleEvent( NEvent& event )
{
    if( event.EventType == OC3_MOUSE_EVENT )
    {
        switch( event.MouseEvent.Event  )
        {
        case OC3_MOUSE_MOVED:
        {
            _d->lastCursorPos = event.MouseEvent.getPosition();  
            if( !_d->lmbPressed || _d->startCursorPos.getX() < 0 )
            {
                _d->startCursorPos = _d->lastCursorPos;
            }
           
            updatePreviewTiles();
        }
        break;        

        case OC3_LMOUSE_PRESSED_DOWN:
        {
            _d->startCursorPos = _d->lastCursorPos;
            _d->lmbPressed = true;
            updatePreviewTiles();
        }
        break;

        case OC3_LMOUSE_LEFT_UP:            // left button
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
                getMapArea().setCenterIJ( tile->getIJ() );
                _d->city->setCameraPos( tile->getIJ() );
            }

            _d->lmbPressed = false;
            _d->startCursorPos = _d->lastCursorPos;
        }
        break;

        case OC3_RMOUSE_LEFT_UP:
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

    if( event.EventType == OC3_KEYBOARD_EVENT )
    {
        switch( event.KeyboardEvent.Key )
        {
        case KEY_UP:
            getMapArea().moveUp(1 + ( event.KeyboardEvent.Shift ? 4 : 0 ) );
        break;

        case KEY_DOWN:          
            getMapArea().moveDown(1 + ( event.KeyboardEvent.Shift ? 4 : 0 ) );
        break;

        case KEY_RIGHT:
            getMapArea().moveRight(1 + ( event.KeyboardEvent.Shift ? 4 : 0 ));
        break;

        case KEY_LEFT:
            getMapArea().moveLeft(1 + ( event.KeyboardEvent.Shift ? 4 : 0 ) );
        break;

        default:
        break;
        }
    }
}

void TilemapRenderer::discardPreview()
{
  foreach( Tile* tile, _d->postTiles )
  {
       delete tile;
  }

  _d->postTiles.clear();
}



void
TilemapRenderer::checkPreviewBuild(const TilePos & pos)
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

  if (overlay->canBuild(pos))
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
        tile->getTerrain().setBuilding( true );
        tile->getTerrain().setOverlay( overlay.as<LandOverlay>() );
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

        bool isConstructible = tile->getTerrain().isConstructible();
        tile->setPicture( isConstructible ? &grnPicture : &redPicture );
        tile->setMasterTile( 0 );
        tile->getTerrain().clearFlags();
        tile->getTerrain().setBuilding( true );
        tile->getTerrain().setOverlay( 0 );
        _d->postTiles.push_back( tile );
      }
    }
  }
}



Tile* TilemapRenderer::getTile(const TilePos& pos )
{
  return &_d->tilemap->at( pos );
}

Signal1< const Tile& >& TilemapRenderer::onShowTileInfo()
{
  return _d->onShowTileInfoSignal;
}

void TilemapRenderer::setMode( const TilemapChangeCommandPtr command )
{
  _d->changeCommand = command;
  _d->startCursorPos = _d->lastCursorPos;
  _d->lmbPressed = false;

  //_d->startCursorPos = Point( -1, -1 );
  updatePreviewTiles();

  if( _d->changeCommand.is<TilemapOverlayCommand>() )
  {
    TilemapOverlayCommandPtr ovCmd = _d->changeCommand.as<TilemapOverlayCommand>();
    switch( ovCmd->getType() )
    {
    case OV_WATER: _d->setDrawFunction( _d.data(), &Impl::drawTileWater ); break;
    case OV_RISK_FIRE: _d->setDrawFunction( _d.data(), &Impl::drawTileFire ); break;
    case OV_RISK_DAMAGE: _d->setDrawFunction( _d.data(), &Impl::drawTileDamage ); break;
    case OV_COMMERCE_PRESTIGE: _d->setDrawFunction( _d.data(), &Impl::drawTileDesirability ); break;
    case OV_COMMERCE_FOOD: _d->setDrawFunction( _d.data(), &Impl::drawTileFood ); break;
    case OV_RELIGION: _d->setDrawFunction( _d.data(), &Impl::drawTileReligion ); break;
    default:_d->setDrawFunction( _d.data(), &Impl::drawTileBase ); break;
    }

    static const WalkerType OV_DEFAULT_WALKERS[] = { WT_ALL, WT_NONE };
    static const WalkerType OV_WATER_WALKERS[] = { WT_NONE };
    static const WalkerType OV_RISK_FIRE_WALKERS[] = { WT_PREFECT, WT_NONE };
    static const WalkerType OV_COMMERCE_PRESTIGE_WALKERS[] = { WT_ALL, WT_NONE };
    static const WalkerType OV_COMMERCE_FOOD_WALKERS[] = { WT_CART_PUSHER, WT_MARKETLADY, WT_MARKETLADY_HELPER, WT_NONE };
    static const WalkerType OV_RELIGION_WALKERS[] = { WT_ALL, WT_NONE };

    switch( ovCmd->getType() )
    {
    case OV_WATER: _d->setVisibleWalkers(OV_WATER_WALKERS); break;
    case OV_RISK_FIRE: _d->setVisibleWalkers(OV_RISK_FIRE_WALKERS); break;
    case OV_RISK_DAMAGE: _d->setVisibleWalkers(OV_WATER_WALKERS); break;
    case OV_COMMERCE_PRESTIGE: _d->setVisibleWalkers(OV_COMMERCE_PRESTIGE_WALKERS); break;
    case OV_COMMERCE_FOOD: _d->setVisibleWalkers(OV_COMMERCE_FOOD_WALKERS); break;
    case OV_RELIGION: _d->setVisibleWalkers(OV_RELIGION_WALKERS); break;
    default:
      _d->setVisibleWalkers(OV_DEFAULT_WALKERS);
      break;
    }

    _d->changeCommand = TilemapChangeCommandPtr();
  }
}

Signal1< std::string >& TilemapRenderer::onWarningMessage()
{
  return _d->onWarningMessageSignal;
}

Tilemap& TilemapRenderer::getTilemap()
{ 
  return *_d->tilemap;
}
