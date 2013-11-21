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

#include "tilemap_camera.hpp"

#include "core/position.hpp"
#include "core/size.hpp"
#include "core/exception.hpp"
#include "core/logger.hpp"
#include "gfx/tile.hpp"
#include "core/foreach.hpp"

struct MovableOrders
{
  bool left;
  bool right;
  bool up;
  bool down;

  bool any() { return left || right || up || down; }
};

class TilemapCamera::Impl
{
public:
  TilePos center;
  Size screenSize;
  Size borderSize;
  Point offset;
  int scrollSpeed;

  Tilemap* tilemap;   // tile map to display
  PointF centerMapXZ; // center of the view(in tiles)
  Size viewSize;      // width of the view(in tiles)  nb_tilesX = 1+2*_view_width
                      // height of the view(in tiles) nb_tilesY = 1+2*_view_height
  TilesArray tiles;   // cached list of visible tiles

  MovableOrders mayMove( PointF point );

public oc3_signals:
  Signal1<Point> onPositionChangedSignal;
};

TilemapCamera::TilemapCamera() : _d( new Impl )
{
  _d->tilemap = NULL;
  _d->scrollSpeed = 4;
  _d->viewSize = Size( 0 );
  _d->center = TilePos( 0, 0 );
  _d->screenSize = Size( 0 );
  _d->centerMapXZ = PointF( 0, 0 );
  _d->borderSize = Size( 180 );
}

TilemapCamera::~TilemapCamera()
{
}

void TilemapCamera::init(Tilemap &tilemap)
{
  _d->tilemap = &tilemap;
}

void TilemapCamera::setViewport(Size newSize)
{
  if( _d->viewSize != newSize )
  {
    _d->tiles.clear();
  }

  _d->screenSize = newSize;

  newSize += _d->borderSize;
  _d->viewSize = Size( (newSize.getWidth() + 59) / 60, ( newSize.getHeight() + 29) / 30 );
  
  Logger::warning( "TilemapArea::setViewport w=%d h=%d", _d->viewSize.getWidth(), _d->viewSize.getHeight() );
}

void TilemapCamera::setCenter(TilePos pos )
{
  _d->center = pos;

  setCenter( Point( pos.getI() + pos.getJ(), _d->tilemap->getSize() - 1 + pos.getJ() - pos.getI() ) );

  _d->onPositionChangedSignal.emit( _d->centerMapXZ.toPoint() );
}

void TilemapCamera::move(PointF relative)
{
  MovableOrders mv = _d->mayMove( _d->centerMapXZ + relative);

  if( relative.getX() < 0 && !mv.left ) { relative.setX( 0 ); }
  if( relative.getX() > 0 && !mv.right ) { relative.setX( 0 ); }
  if( relative.getY() < 0 && !mv.up ) { relative.setY( 0 ); }
  if( relative.getY() > 0 && !mv.down ) { relative.setY( 0 ); }

  if( mv.any() )
  {
    _d->centerMapXZ += relative;
    _d->tiles.clear();

    _d->onPositionChangedSignal.emit( _d->centerMapXZ.toPoint() );
  }
}

void TilemapCamera::setCenter(Point pos)
{
  if( _d->centerMapXZ.toPoint() != pos  )
  {
    _d->tiles.clear();
  }
  
  _d->centerMapXZ = pos.toPointF();
}

int TilemapCamera::getCenterX() const  {   return _d->centerMapXZ.getX();   }
int TilemapCamera::getCenterZ() const  {   return _d->centerMapXZ.getY();   }
TilePos TilemapCamera::getCenter() const  {   return _d->center;   }
void TilemapCamera::setScrollSpeed(int speed){  _d->scrollSpeed = speed; }
int TilemapCamera::getScrollSpeed() const{ return _d->scrollSpeed; }
Tile* TilemapCamera::at(Point pos, bool overborder){  return _d->tilemap->at( pos - _d->offset, overborder );}
Signal1<Point>& TilemapCamera::onPositionChanged(){  return _d->onPositionChangedSignal;}
void TilemapCamera::moveRight(const int amount){  setCenter( Point( getCenterX() + amount, getCenterZ() ) );}
void TilemapCamera::moveLeft(const int amount){  setCenter( Point( getCenterX() - amount, getCenterZ() ) );}
void TilemapCamera::moveUp(const int amount){  setCenter( Point( getCenterX(), getCenterZ() + amount ) );}
void TilemapCamera::moveDown(const int amount){  setCenter( Point( getCenterX(), getCenterZ() - amount ) );}

void TilemapCamera::startFrame()
{
  foreach( Tile* tile, _d->tiles )
    tile->resetWasDrawn();
}

const TilesArray& TilemapCamera::getTiles() const
{
  if( _d->tiles.empty() )
  {
    _d->offset.setX( _d->screenSize.getWidth() / 2 - 30 * (getCenterX() + 1) + 1 );
    _d->offset.setY( _d->screenSize.getHeight()/ 2 + 15 * (getCenterZ() - _d->tilemap->getSize() + 1) - 30 );

    int mapSize = _d->tilemap->getSize();
    int zm = _d->tilemap->getSize() + 1;
    int cx = _d->centerMapXZ.getX();
    int cz = _d->centerMapXZ.getY();

    Size sizeT = _d->viewSize;  // size x

    for (int z = cz + sizeT.getHeight(); z>=cz - sizeT.getHeight(); --z)
    {
      // depth axis. from far to near.

      int xstart = cx - sizeT.getWidth();
      if ((xstart + z) % 2 == 0)
      {
	      ++xstart;
      }

      for (int x = xstart; x<=cx + sizeT.getWidth(); x+=2)
      {
	// left-right axis
        int j = (x + z - zm)/2;
        int i = x - j;

        if( (i >= 0) && (j >= 0) && (i < mapSize) && (j < mapSize) )
        {
          _d->tiles.push_back( &_d->tilemap->at( i, j ));
        }
      }
    }

    /*for( int y=0; y < mapSize; y++ )
    {
      for( int t=0; t <= y; t++ )
      {
        _d->tiles.push_back( &_d->tilemap->at( t, mapSize - 1 - ( y - t ) ) );
      }
    }

    for( int x=1; x < mapSize; x++ )
    {
      for( int t=0; t < mapSize-x; t++ )
      {
        _d->tiles.push_back( &_d->tilemap->at( x + t, t ) );
      }
    }*/
  }

  return _d->tiles;
}


MovableOrders TilemapCamera::Impl::mayMove(PointF point)
{
  MovableOrders ret = { true, true, true, true };

  int mapSize = tilemap->getSize();
  Point mapOffset = Point( screenSize.getWidth() / 2 - 30 * (centerMapXZ.getX() + 1) + 1,
                           screenSize.getHeight() / 2 + 15 * (centerMapXZ.getY()-mapSize + 1) - 30 );

  ret.left = !( (tilemap->at( 0, 0 ).getXY() + mapOffset ).getX() > 0);
  ret.right = (tilemap->at( mapSize - 1, mapSize - 1 ).getXY() + mapOffset).getX() > screenSize.getWidth();
  ret.down = ( (tilemap->at( 0, mapSize - 1 ).getXY() + mapOffset ).getY() < 0 );
  ret.up = (tilemap->at( mapSize - 1, 0 ).getXY() + mapOffset ).getY() > screenSize.getHeight();

  return ret;
}

Point TilemapCamera::getOffset() const{  return _d->offset;}
