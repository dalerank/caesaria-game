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

#include "oc3_gui_tilemap.hpp"

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

class GuiTilemap::Impl
{
public:
  typedef std::vector< Picture* > Pictures;
  
  Picture* clearPic;
  PtrTilesList postTiles;  // these tiles have draw over "normal" tilemap tiles!
  Pictures previewToolPictures;
  Point lastCursorPos;
  Point startCursorPos;
  bool  lmbPressed;
  // current map offset, for private use
  Point mapOffset;
  City* city;     // city to display
  Tilemap* tilemap;
  TilemapArea* mapArea;  // visible map area
  ScreenGame* screenGame;
  GfxEngine* engine;

  TilePos lastTilePos;
  TilemapChangeCommandPtr changeCommand;

  typedef Delegate1< Tile& > DrawTileSignature;
  DrawTileSignature drawTileFunction;

  void getSelectedArea( TilePos& outStartPos, TilePos& outStopPos );
  // returns the tile at the cursor position.
  Tile* getTileXY( const Point& pos, bool overborder=false );
  void buildAll();
  void clearAll();

  void drawTileBase( Tile& tile );
  void drawTileWater( Tile& tile );
  void drawTileInSelArea( Tile& tile, Tile* master );
  void drawAnimations( LandOverlayPtr overlay, const Point& screenPos );

  template< class X, class Y >
  void setDrawFunction( Y* obj, void (X::*func)( Tile& ) )
  {
    drawTileFunction = makeDelegate( obj, func );
  }

oc3_signals public:
  Signal1< const Tile& > onShowTileInfoSignal;
  Signal1< std::string > onWarningMessageSignal;
};

GuiTilemap::GuiTilemap() : _d( new Impl )
{
  _d->city = NULL;
  _d->mapArea = NULL;
}

GuiTilemap::~GuiTilemap() {}

void GuiTilemap::init( City &city, TilemapArea &mapArea, ScreenGame *screen)
{
  _d->city = &city;
  _d->tilemap = &city.getTilemap();
  _d->mapArea = &mapArea;
  _d->screenGame = screen;
  _d->engine = &GfxEngine::instance();
  _d->clearPic = &Picture::load( "oc3_land", 2 );
  _d->setDrawFunction( _d.data(), &Impl::drawTileBase );
}

void GuiTilemap::drawTileEx( Tile& tile, const int depth )
{
  if( tile.is_flat() )
  {
    return;  // tile has already been drawn!
  }

  Tile* master = tile.get_master_tile();

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

void GuiTilemap::drawTile( Tile& tile )
{
  _d->drawTileFunction( tile );
}

void GuiTilemap::Impl::drawAnimations( LandOverlayPtr overlay, const Point& screenPos )
{
  // building foregrounds and animations
  Impl::Pictures& fgPictures = overlay->getForegroundPictures();
  for( Impl::Pictures::iterator itPic = fgPictures.begin(); itPic != fgPictures.end(); ++itPic )
  {
    // for each foreground picture
    if( 0 == *itPic )
    {
      // skip void picture
      continue;
    }

    engine->drawPicture( **itPic, screenPos);
  }
}

void GuiTilemap::Impl::drawTileWater( Tile& tile )
{
  Point screenPos( 30 * (tile.getI() + tile.getJ()), 15 * (tile.getI() - tile.getJ()) );
  screenPos += mapOffset;
  
  tile.setWasDrawn();

  const TerrainTile& terrain = tile.get_terrain();
  if( terrain.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.get_picture(), screenPos );
    return;
  }
  
  bool needDrawAnimations = false;
  LandOverlayPtr overlay = terrain.getOverlay();
  Picture* pic = 0;
  switch( overlay->getType() )
  {
    //water buildings
  case B_ROAD:
  case B_PLAZA:
  case B_RESERVOIR:
  case B_FOUNTAIN:
  case B_WELL:
    pic = &tile.get_picture();
    needDrawAnimations = true;
  break;  

    //houses
  case B_HOUSE:
    pic = &Picture::load( ResourceGroup::waterOverlay, (overlay->getSize() - 1)*2 + 1 + 10 );
  break;
    
    //other buildings
  default:
    pic = &Picture::load( ResourceGroup::waterOverlay, (overlay->getSize() - 1)*2 + 1 );
  break;
  }  

  engine->drawPicture( *pic, screenPos );

  if( needDrawAnimations )
  {
    drawAnimations( overlay, screenPos );
  }
}

void GuiTilemap::Impl::drawTileBase( Tile& tile )
{
  Point screenPos( 30 * (tile.getI() + tile.getJ()), 15 * (tile.getI() - tile.getJ()) );
  screenPos += mapOffset;

  tile.setWasDrawn();

  Picture& pic = tile.get_picture();
  //draw background
  engine->drawPicture( pic, screenPos );

  LandOverlayPtr overlay = tile.get_terrain().getOverlay();
  if( overlay.isNull() )
  {
    return;
  }  

  drawAnimations( overlay, screenPos );
}

void GuiTilemap::Impl::drawTileInSelArea( Tile& tile, Tile* master )
{
  if( master==NULL )
  {
    // single-tile
    drawTileFunction( tile );
    engine->drawPicture( *clearPic, 30 * (tile.getI() + tile.getJ()) + mapOffset.getX(), 
                                    15 * (tile.getI() - tile.getJ()) + mapOffset.getY() );
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

void GuiTilemap::drawTilemap()
{
  Tilemap &tilemap = *_d->tilemap;
  TilemapArea &mapArea = *_d->mapArea;  

  // center the map on the screen
  Point mOffset( _d->engine->getScreenWidth() / 2 - 30 * (mapArea.getCenterX() + 1) + 1,
                 _d->engine->getScreenHeight() / 2 + 15 * (mapArea.getCenterZ()-tilemap.getSize() + 1) - 30 );

  _d->mapOffset = mOffset;  // this is the current offset

  int lastZ = -1000;  // dummy value

  const std::vector< TilePos >& tiles = mapArea.getTiles();
  std::vector< TilePos >::const_iterator itPos;

  for( itPos = tiles.begin(); itPos != tiles.end(); ++itPos )
  {
    getTile( *itPos ).resetWasDrawn();
  }

  Rect selectedArea( -1, -1, -1, -1 );
  if( _d->changeCommand.isValid() && _d->changeCommand.is<TilemapRemoveCommand>() )
  {
    TilePos startPos, stopPos;
    _d->getSelectedArea( startPos, stopPos );
    selectedArea = Rect( startPos.getI(), startPos.getJ(), stopPos.getI(), stopPos.getJ() );
  }

  // FIRST PART: draw all flat land (walkable/boatable)
  for( itPos = tiles.begin(); itPos != tiles.end(); ++itPos )
  {
    Tile& tile = getTile( *itPos );
    Tile* master = tile.get_master_tile();

    if( !tile.is_flat() )
      continue;
   
    if( selectedArea.UpperLeftCorner.getX() >= 0 && 
        selectedArea.isPointInside( Point( (*itPos).getI(), (*itPos).getJ() ) ) )
    {
      _d->drawTileInSelArea( tile, master );  
    }
    else
    {
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
  }  

  // SECOND PART: draw all sprites, impassable land and buildings
  for( itPos = tiles.begin(); itPos != tiles.end(); ++itPos)
  {
    int z = itPos->getZ();
   
    if (z != lastZ)
    {
       // TODO: pre-sort all animations
       lastZ = z;

       Impl::Pictures pictureList;

       Walkers walkerList = _d->city->getWalkerList( WT_ALL );
       for( Walkers::iterator itWalker =  walkerList.begin();
             itWalker != walkerList.end(); ++itWalker)
       {
          // for each walker
          WalkerPtr anim = *itWalker;
          int zAnim = anim->getIJ().getZ();// getJ() - anim.getI();
          if( zAnim > z && zAnim <= z+1 )
          {
             pictureList.clear();
             anim->getPictureList( pictureList );
             for( Impl::Pictures::iterator picIt = pictureList.begin(); picIt != pictureList.end(); ++picIt )
             {
                if( *picIt == NULL )
                {
                   continue;
                }

                _d->engine->drawPicture( **picIt, 2*(anim->getII() + anim->getJJ()) + mOffset.getX(), 
                                                     anim->getII() - anim->getJJ()  + mOffset.getY());
             }
          }
       }
    }   

    if( selectedArea.UpperLeftCorner.getX() > 0 
        && selectedArea.isPointInside( Point( (*itPos).getI(), (*itPos).getJ() ) )  )
    {
      _d->engine->setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );      
    }

    drawTileEx( getTile( *itPos ), z );
    _d->engine->resetTileDrawMask();
  }

  //Third part: drawing build/remove tools
  for( PtrTilesList::iterator itPostTile = _d->postTiles.begin(); itPostTile != _d->postTiles.end(); ++itPostTile )
  {
    int z = (*itPostTile)->getJ() - (*itPostTile)->getI();
    (*itPostTile)->resetWasDrawn();
    drawTileEx( **itPostTile, z );
  }
}

Tile* GuiTilemap::Impl::getTileXY( const Point& pos, bool overborder)
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

TilemapArea &GuiTilemap::getMapArea()
{
  return *_d->mapArea;
}

void GuiTilemap::updatePreviewTiles( bool force )
{
  TilemapBuildCommandPtr bldCommand = _d->changeCommand.as<TilemapBuildCommand>();
  if( bldCommand.isNull() )
    return;

  if( !bldCommand->isMultiBuilding() )
    _d->startCursorPos = _d->lastCursorPos;

  Tile* curTile = _d->getTileXY( _d->lastCursorPos, true );

  if( !curTile )
    return;

  if( curTile && !force && _d->lastTilePos == curTile->getIJ() )
      return;

  _d->lastTilePos = curTile->getIJ();

  discardPreview();

  if( bldCommand->isBorderBuilding() )
  {
    Tile* startTile = _d->getTileXY( _d->startCursorPos, true );  // tile under the cursor (or NULL)
    Tile* stopTile  = _d->getTileXY( _d->lastCursorPos,  true );

    RoadPropagator rp( *_d->tilemap, *startTile );

    ConstWayOnTiles pathWay;
    bool havepath = rp.getPath( *stopTile, pathWay );
    if( havepath )
    {
      for( ConstWayOnTiles::iterator it=pathWay.begin(); it != pathWay.end(); it++ )
      {
	      checkPreviewBuild( (*it)->getIJ() );
      }
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

void GuiTilemap::Impl::getSelectedArea( TilePos& outStartPos, TilePos& outStopPos )
{
  Tile* startTile = getTileXY( startCursorPos, true );  // tile under the cursor (or NULL)
  Tile* stopTile  = getTileXY( lastCursorPos, true );

  TilePos startPosTmp = startTile->getIJ();
  TilePos stopPosTmp  = stopTile->getIJ();

//  std::cout << "GuiTilemap::_getSelectedArea" << " ";
//  std::cout << "(" << startPosTmp.getI() << " " << startPosTmp.getJ() << ") (" << stopPosTmp.getI() << " " << stopPosTmp.getJ() << ")" << std::endl;
    
  outStartPos = TilePos( std::min<int>( startPosTmp.getI(), stopPosTmp.getI() ), std::min<int>( startPosTmp.getJ(), stopPosTmp.getJ() ) );
  outStopPos  = TilePos( std::max<int>( startPosTmp.getI(), stopPosTmp.getI() ), std::max<int>( startPosTmp.getJ(), stopPosTmp.getJ() ) );
}

void GuiTilemap::Impl::clearAll()
{
  TilePos startPos, stopPos;
  getSelectedArea( startPos, stopPos );

  PtrTilesList tiles4clear = tilemap->getFilledRectangle( startPos, stopPos );
  for( PtrTilesList::iterator it = tiles4clear.begin(); it != tiles4clear.end(); it++) 
  {
      city->clearLand( (*it)->getIJ() );
  }
}

void GuiTilemap::Impl::buildAll()
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
  for( PtrTilesList::iterator it=postTiles.begin(); it != postTiles.end(); it++ )
  {   
    if( cnstr->canBuild( (*it)->getIJ() ) && (*it)->is_master_tile())
    {
      city->build( cnstr->getType(), (*it)->getIJ() );
      buildOk = true;
    }   
  }

  if( !buildOk )
  {
    onWarningMessageSignal.emit( "##need_build_on_cleared_area##" ); 
  }
}

void GuiTilemap::handleEvent( NEvent& event )
{
    if( event.EventType == OC3_MOUSE_EVENT )
    {
        switch( event.MouseEvent.Event  )
        {
        case OC3_MOUSE_MOVED:
        {
            _d->lastCursorPos = event.MouseEvent.getPosition();  
            if( !_d->lmbPressed || _d->startCursorPos.getX() < 0 )
                _d->startCursorPos = _d->lastCursorPos;
           
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
            Tile* tile = _d->getTileXY( event.MouseEvent.getPosition() );  // tile under the cursor (or NULL)
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
            Tile* tile = _d->getTileXY( event.MouseEvent.getPosition() );  // tile under the cursor (or NULL)
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
        }
    }
}

void GuiTilemap::discardPreview()
{
  for( Impl::Pictures::iterator it=_d->previewToolPictures.begin(); it != _d->previewToolPictures.end(); it++ )
  {
    GfxEngine::instance().deletePicture( **it );
  }

  _d->previewToolPictures.clear();

  for( PtrTilesList::iterator it=_d->postTiles.begin(); it != _d->postTiles.end(); it++ )
  {
       delete *it;
  }

  _d->postTiles.clear();
}

void GuiTilemap::checkPreviewBuild( const TilePos& pos )
{
  TilemapBuildCommandPtr bldCommand = _d->changeCommand.as<TilemapBuildCommand>();
  if( bldCommand.isNull() )
    return;

  // TODO: do only when needed, when (i, j, _buildInstance) has changed
  ConstructionPtr overlay = bldCommand->getContruction();
  if( overlay.isValid() )
  {
     int size = overlay->getSize();
     if( overlay->canBuild( pos ) )
     {
         _d->previewToolPictures.push_back( new Picture() );
         PictureConverter::maskColor( *_d->previewToolPictures.back(), overlay->getPicture(), 0x00000000, 0x00ff0000, 0x00000000, 0xff000000 );

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
                 tile->set_picture( _d->previewToolPictures.back() );
                 tile->set_master_tile( masterTile );
                 tile->get_terrain().setBuilding( true );
                 tile->get_terrain().setOverlay( overlay.as<LandOverlay>() );
                 _d->postTiles.push_back( tile );
                 //_priorityTiles.push_back( tile );
             }
         }
     }
     else
     {
       Picture& grnPicture = Picture::load( "oc3_land", 1 );
       Picture& redPicture = Picture::load( "oc3_land", 2 );
         
       for (int dj = 0; dj < size; ++dj)
       {
           for (int di = 0; di < size; ++di)
           {
             TilePos rPos = pos + TilePos( di, dj );
             if( !_d->tilemap->is_inside( rPos ) )
                 continue;

             Tile* tile = new Tile( _d->tilemap->at( rPos ) );  // make a copy of tile

             bool isConstructible = tile->get_terrain().isConstructible();
             tile->set_picture( isConstructible ? &grnPicture : &redPicture );
             tile->set_master_tile(0);
             tile->get_terrain().reset();
             tile->get_terrain().setBuilding( true );
             _d->postTiles.push_back( tile );
           }
       }
     }
  }
}

Tile& GuiTilemap::getTile(const TilePos& pos )
{
  return _d->tilemap->at( pos );
}

Signal1< const Tile& >& GuiTilemap::onShowTileInfo()
{
  return _d->onShowTileInfoSignal;
}

void GuiTilemap::setChangeCommand( const TilemapChangeCommandPtr command )
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
    case OV_WATER:
      _d->setDrawFunction( _d.data(), &Impl::drawTileWater );
    break;

    default:
      _d->setDrawFunction( _d.data(), &Impl::drawTileBase );
    break;
    }

    _d->changeCommand = TilemapChangeCommandPtr();
  }
}

Signal1< std::string >& GuiTilemap::onWarningMessage()
{
  return _d->onWarningMessageSignal;
}