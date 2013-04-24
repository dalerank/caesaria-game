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

#include <algorithm>
#include <iostream>

#include "oc3_gfx_engine.hpp"
#include "oc3_exception.hpp"
#include "oc3_screen_game.hpp"
#include "oc3_positioni.hpp"
#include "oc3_pictureconverter.hpp"
#include "oc3_event.hpp"
#include "oc3_roadpropagator.hpp"
#include "oc3_sdl_facade.hpp"
#include "oc3_tilemapchangecommand.hpp"
#include "oc3_tilemap.hpp"

class GuiTilemap::Impl
{
public:
  typedef std::vector< Picture* > Pictures;
  
  PtrTilesList postTiles;  // these tiles have draw over "normal" tilemap tiles!
  Pictures previewToolPictures;
  Point lastCursorPos;
  Point startCursorPos;
  bool  lmbPressed;
  // current map offset, for private use
  Point mapOffset;

  TilePos lastTilePos;
  TilemapChangeCommand changeCommand;

oc3_signals public:
  Signal1< Tile* > onShowTileInfoSignal;
};


GuiTilemap::GuiTilemap() : _d( new Impl )
{
   _city = NULL;
   _mapArea = NULL;
}

GuiTilemap::~GuiTilemap() {}

void GuiTilemap::init(City &city, TilemapArea &mapArea, ScreenGame *screen)
{
   _city = &city;
   _tilemap = &_city->getTilemap();
   _mapArea = &mapArea;
   _screenGame = screen;
}

void GuiTilemap::drawTileEx( const Tile& tile, const int depth )
{
    Tile* master = tile.get_master_tile();

    if( master==NULL )
    {
      // single-tile
		  drawTile( tile );
    }
    else
    {
        // multi-tile: draw the master tile.
        int masterZ = master->getJ() - master->getI();
        if( masterZ == depth )
        {
            // it is time to draw the master tile
            if (std::find(_multiTiles.begin(), _multiTiles.end(), master) == _multiTiles.end())
            {
                // master has not been drawn yet
              _multiTiles.push_back(master);  // don't draw that multi-tile again
				      drawTile( *master );
            }
        }
    }
}

void GuiTilemap::drawTile( const Tile& tile )
{
	int i = tile.getI();
	int j = tile.getJ();

	Point mOffset = _d->mapOffset;
	
	Picture& pic = tile.get_picture();
	GfxEngine &engine = GfxEngine::instance();
	engine.drawPicture(pic, 30*(i+j)+mOffset.getX(), 15*(i-j)+mOffset.getY());

	// building foregrounds and animations
	LandOverlay *overlay = tile.get_terrain().getOverlay();
	if( overlay != NULL )
	{
		std::vector<Picture*>& fgPictures = overlay->getForegroundPictures();
		for (std::vector<Picture*>::iterator itPic = fgPictures.begin(); itPic != fgPictures.end(); ++itPic)
		{
			// for each foreground picture
			if (*itPic == NULL)
			{
				// skip void picture
				continue;
			}
			Picture &fgpic = **itPic;
			engine.drawPicture(fgpic, 30*(i+j)+mOffset.getX(), 15*(i-j)+mOffset.getY());
		}
	}
}

void GuiTilemap::drawTilemap()
{
  GfxEngine &engine = GfxEngine::instance();

  _multiTiles.clear();  // we will draw everything again
  Tilemap &tilemap = *_tilemap;
  TilemapArea &mapArea = *_mapArea;

  // center the map on the screen
  Point mOffset( engine.getScreenWidth()/2 - 30*(mapArea.getCenterX()+1) + 1,
                 engine.getScreenHeight()/2 + 15*(mapArea.getCenterZ()-tilemap.getSize()+1) - 30 );
  _d->mapOffset = mOffset;  // this is the current offset

  int lastZ = -1000;  // dummy value

  const std::vector<std::pair<int, int> >& tiles = mapArea.getTiles();
  std::vector<std::pair<int, int> >::const_iterator itTiles;

  // FIRST PART: draw all flat land (walkable/boatable)
  for( itTiles = tiles.begin(); itTiles != tiles.end(); ++itTiles )
  {
    int i = (*itTiles).first;
    int j = (*itTiles).second;

    Tile& tile = getTileIJ(i, j);
    Tile* master = tile.get_master_tile();

    if( tile.is_flat() )
    {
      if( master==NULL )
      {
        // single-tile
	      drawTile( tile );
      }
      else
      {
        // multi-tile: draw the master tile.
        if( std::find(_multiTiles.begin(), _multiTiles.end(), master) == _multiTiles.end() )
        {
           // master has not been drawn yet
          _multiTiles.push_back(master);
	        drawTile( *master );
        }
      }
    }
  }

  // SECOND PART: draw all sprites, impassable land and buildings
  for( itTiles = tiles.begin(); itTiles != tiles.end(); ++itTiles)
  {
     int i = (*itTiles).first;
     int j = (*itTiles).second;

     int z = j - i;
     if (z != lastZ)
     {
        // TODO: pre-sort all animations
        lastZ = z;

        std::vector<Picture*> pictureList;

        City::Walkers walkerList = _city->getWalkerList( WT_ALL );
        for( City::Walkers::iterator itWalker =  walkerList.begin();
              itWalker != walkerList.end(); ++itWalker)
        {
           // for each walker
           Walker &anim = **itWalker;
           int zAnim = anim.getJ() - anim.getI();
           if (zAnim > z && zAnim <= z+1)
           {
              pictureList.clear();
              anim.getPictureList(pictureList);
              for( std::vector<Picture*>::iterator picIt = pictureList.begin(); picIt != pictureList.end(); ++picIt )
              {
                 if (*picIt == NULL)
                 {
                    continue;
                 }
                 engine.drawPicture( **picIt, 2*(anim.getII()+anim.getJJ())+mOffset.getX(), anim.getII()-anim.getJJ()+mOffset.getY());
              }
           }
        }
     }

    Tile& tile = getTileIJ(i, j);
    drawTileEx( tile, z );
  }

  //Third part: drawing build/remove tools
  {
    for( PtrTilesList::iterator itPostTile = _d->postTiles.begin(); itPostTile != _d->postTiles.end(); ++itPostTile )
    {
      int z = (*itPostTile)->getJ() - (*itPostTile)->getI();
	    drawTileEx( **itPostTile, z );
    }       
  }
}

Tile* GuiTilemap::getTileXY( const Point& pos, bool overborder)
{
   Point mOffset = pos - _d->mapOffset;  // x relative to the left most pixel of the tilemap
   int i = (mOffset.getX() + 2 * mOffset.getY()) / 60;
   int j = (mOffset.getX() - 2 * mOffset.getY()) / 60;
   
   if( overborder )
   {
       i = math::clamp( i, 0, _tilemap->getSize() - 1 );
       j = math::clamp( j, 0, _tilemap->getSize() - 1 );
   }
   // std::cout << "ij ("<<i<<","<<j<<")"<<std::endl;

   if (i>=0 && j>=0 && i<_tilemap->getSize() && j<_tilemap->getSize())
   {
      // valid coordinate
      return &getTileIJ(i, j);
   }
   else
   {
      // the pixel is outside the tilemap => no tile here
      return NULL;
   }
}

TilemapArea &GuiTilemap::getMapArea()
{
  return *_mapArea;
}

void GuiTilemap::updatePreviewTiles( bool force )
{
  if( !_d->changeCommand.isValid() )
    return;

  Tile* curTile = getTileXY( _d->lastCursorPos, true );
  if( curTile && !force && _d->lastTilePos == curTile->getIJ() )
      return;

  _d->lastTilePos = curTile->getIJ();

  discardPreview();
    
  if( _d->changeCommand.isBorderBuilding() )
  {
    Tile* startTile = getTileXY( _d->startCursorPos, true );  // tile under the cursor (or NULL)
    Tile* stopTile  = getTileXY( _d->lastCursorPos,  true );

    RoadPropagator rp( *_tilemap, *startTile );

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
    _getSelectedArea( startPos, stopPos );

    for( int i = startPos.getI(); i <= stopPos.getI(); i++ )
    {
      for( int j = startPos.getJ(); j <=stopPos.getJ(); j++ )
      {
	      checkPreviewBuild( TilePos( i, j ) );
	      checkPreviewRemove(i, j);
      }
    }
  }
}

void GuiTilemap::_getSelectedArea( TilePos& outStartPos, TilePos& outStopPos )
{
  Tile* startTile = getTileXY( _d->startCursorPos, true );  // tile under the cursor (or NULL)
  Tile* stopTile  = getTileXY( _d->lastCursorPos, true );

  TilePos startPosTmp = startTile->getIJ();
  TilePos stopPosTmp  = stopTile->getIJ();

//  std::cout << "GuiTilemap::_getSelectedArea" << " ";
//  std::cout << "(" << startPosTmp.getI() << " " << startPosTmp.getJ() << ") (" << stopPosTmp.getI() << " " << stopPosTmp.getJ() << ")" << std::endl;
    
  outStartPos = TilePos( std::min<int>( startPosTmp.getI(), stopPosTmp.getI() ), std::min<int>( startPosTmp.getJ(), stopPosTmp.getJ() ) );
  outStopPos  = TilePos( std::max<int>( startPosTmp.getI(), stopPosTmp.getI() ), std::max<int>( startPosTmp.getJ(), stopPosTmp.getJ() ) );
}

void GuiTilemap::_clearLand()
{
  TilePos startPos, stopPos;
  _getSelectedArea( startPos, stopPos );
  int i, j;

  for (i = startPos.getI(); i <= stopPos.getI(); i++) 
  {
    for (j = startPos.getJ(); j <= stopPos.getJ(); j++ )
    {
      _city->clearLand( TilePos(i,j) );
    }   
  }
}

void GuiTilemap::_buildAll()
{
  for( PtrTilesList::iterator it=_d->postTiles.begin(); it != _d->postTiles.end(); it++ )
  {
    Construction* cnstr = _d->changeCommand.getContruction();
//    std::cout << "(" << (*it)->getI() << " " << (*it)->getJ() << ") ";
    if( cnstr && cnstr->canBuild( (*it)->getIJ() ) && (*it)->is_master_tile())
    {
      _city->build( *cnstr, (*it)->getIJ() );
    }
  }
//  std::cout << std::endl;
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
            if( !_d->lmbPressed 
                || _d->startCursorPos.getX() < 0 
                || (_d->changeCommand.isValid() && !_d->changeCommand.isMultiBuilding()) )
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
            Tile* tile = getTileXY( event.MouseEvent.getPosition() );  // tile under the cursor (or NULL)
            if( tile == 0 )
                break;


            if( _d->changeCommand.isValid() )
            {                
                if( _d->changeCommand.isRemoveTool() )
                {
                    _clearLand();                      
                }
                else if( _d->changeCommand.getContruction() )
                {
                    _buildAll();               
                }
                _d->startCursorPos = _d->lastCursorPos;
                updatePreviewTiles( true );
            }
            else
            {
                getMapArea().setCenterIJ( tile->getIJ() );
            }

            _d->lmbPressed = false;
            _d->startCursorPos = _d->lastCursorPos;
        }
        break;

        case OC3_RMOUSE_LEFT_UP:
        {
            Tile* tile = getTileXY( event.MouseEvent.getPosition() );  // tile under the cursor (or NULL)
            if( _d->changeCommand.isValid() )
            { 
                _d->changeCommand = TilemapChangeCommand();
                discardPreview();
            }
            else
            {
                _d->onShowTileInfoSignal.emit( tile );
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
    SdlFacade::instance().deletePicture( **it );
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
   // TODO: do only when needed, when (i, j, _buildInstance) has changed
   Construction* overlay = _d->changeCommand.getContruction();
   if( overlay )
   {
      int size = overlay->getSize();
      if( overlay->canBuild( pos ) )
      {
          _d->previewToolPictures.push_back( new Picture() );
          PictureConverter::rgbBalance( *_d->previewToolPictures.back(), overlay->getPicture(), -255, +0, -255 );

          Tile *masterTile=0;
          for (int dj = 0; dj < size; ++dj)
          {
              for (int di = 0; di < size; ++di)
              {
                  Tile* tile = new Tile(_tilemap->at( pos + TilePos( di, dj ) ));  // make a copy of tile

                  if (di==0 && dj==0)
                  {
                      // this is the masterTile
                      masterTile = tile;
                  }
                  tile->set_picture( _d->previewToolPictures.back() );
                  tile->set_master_tile( masterTile );
                  tile->get_terrain().setOverlay( overlay );
                  _d->postTiles.push_back( tile );
                  //_priorityTiles.push_back( tile );
              }
          }
      }
      else
      {
          Picture& grnPicture = PicLoader::instance().get_picture( "oc3_land", 1 );
          Picture& redPicture = PicLoader::instance().get_picture( "oc3_land", 2 );
          
          for (int dj = 0; dj < size; ++dj)
          {
              for (int di = 0; di < size; ++di)
              {
                  Tile* tile = new Tile(_tilemap->at( pos + TilePos( di, dj ) ));  // make a copy of tile

                  bool isConstructible = tile->get_terrain().isConstructible();
                  tile->set_picture( isConstructible ? &grnPicture : &redPicture );
                  tile->set_master_tile(0);
                  tile->get_terrain().reset();
                  _d->postTiles.push_back( tile );
              }
          }
      }
   }
}

void GuiTilemap::checkPreviewRemove(const int i, const int j)
{
    if( _d->changeCommand.isRemoveTool() )
    {
        Tile& cursorTile = _tilemap->at(i, j);
        TerrainTile& terrain = cursorTile.get_terrain();
        //if( terrain.isDestructible() )
        {           
            Picture& pic_clear = PicLoader::instance().get_picture( "oc3_land", 2 );

            LandOverlay* overlay = terrain.getOverlay();
            if (overlay == NULL)
            {
                // this is maybe a lonely tree
                Tile* tile = new Tile(_tilemap->at(i, j));  // make a copy of tile
                tile->set_picture(&pic_clear);
                tile->set_master_tile(NULL);  // single tile
                _d->postTiles.push_back( tile );
                //_priorityTiles.push_back(&tile);
            }
            else
            {
                _d->previewToolPictures.push_back( new Picture() );
                PictureConverter::rgbBalance( *_d->previewToolPictures.back(), overlay->getPicture(), +0, -255, -255 );

                // remove the overlay, and make single tile of cleared land
                int size = overlay->getSize();
                TilePos tilePos = overlay->getTile().getIJ(); // master I

                Tile* masterTile = 0;
                for (int dj = 0; dj<size; ++dj)
                {
                    for (int di = 0; di<size; ++di)
                    {
                        Tile* tile = new Tile(_tilemap->at( tilePos + TilePos( di, dj ) ) );  // make a copy of tile

                        if (di==0 && dj==0)
                        {
                            // this is the masterTile
                            masterTile = tile;
                        }                        

                        tile->set_picture( _d->previewToolPictures.back() );
                        tile->set_master_tile( masterTile );  // single tile
                        TerrainTile &terrain = tile->get_terrain();
                        terrain.setOverlay( 0 );
                        _d->postTiles.push_back( tile );
                    }
                }
            }
        }
    }
}

Tile& GuiTilemap::getTileIJ(const int i, const int j)
{
  return _tilemap->at(i, j);
}

Signal1< Tile* >& GuiTilemap::onShowTileInfo()
{
  return _d->onShowTileInfoSignal;
}

void GuiTilemap::setChangeCommand( const TilemapChangeCommand& command )
{
  _d->changeCommand = command;
  _d->startCursorPos = _d->lastCursorPos;
  //_d->startCursorPos = Point( -1, -1 );
  updatePreviewTiles();
}