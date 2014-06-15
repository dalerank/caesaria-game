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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "tilemap_camera.hpp"

#include "core/position.hpp"
#include "core/size.hpp"
#include "core/exception.hpp"
#include "core/logger.hpp"
#include "gfx/tile.hpp"
#include "core/foreach.hpp"
#include "tileoverlay.hpp"

#include <set>

namespace gfx
{

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
  void resetDrawn();

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
  _d->borderSize = Size( 90 );
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
  _d->viewSize = Size( (newSize.width() + 59) / 60, ( newSize.height() + 29) / 30 );
  
  Logger::warning( "TilemapArea::setViewport w=%d h=%d", _d->viewSize.width(), _d->viewSize.height() );
}

void TilemapCamera::setCenter(TilePos pos )
{
  _d->center = pos;

  setCenter( Point( pos.i() + pos.j(), _d->tilemap->size() - 1 + pos.j() - pos.i() ) );

  oc3_emit _d->onPositionChangedSignal( _d->centerMapXZ.toPoint() );
}

void TilemapCamera::move(PointF relative)
{
  MovableOrders mv = _d->mayMove( _d->centerMapXZ + relative);

  if( relative.x() < 0 && !mv.left ) { relative.setX( 0 ); }
  if( relative.x() > 0 && !mv.right ) { relative.setX( 0 ); }
  if( relative.y() < 0 && !mv.up ) { relative.setY( 0 ); }
  if( relative.y() > 0 && !mv.down ) { relative.setY( 0 ); }

  if( mv.any() )
  {
    _d->centerMapXZ += relative;
    _d->resetDrawn();
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
  oc3_emit _d->onPositionChangedSignal( _d->centerMapXZ.toPoint() );
}

int TilemapCamera::centerX() const  {   return _d->centerMapXZ.x();   }
int TilemapCamera::centerZ() const  {   return _d->centerMapXZ.y();   }
TilePos TilemapCamera::center() const  {   return _d->center;   }
void TilemapCamera::setScrollSpeed(int speed){  _d->scrollSpeed = speed; }
int TilemapCamera::scrollSpeed() const{ return _d->scrollSpeed; }
Tile* TilemapCamera::at(const Point& pos, bool overborder) const {  return _d->tilemap->at( pos - _d->offset, overborder );}

Tile* TilemapCamera::at(const TilePos& pos) const { return &_d->tilemap->at( pos ); }
Signal1<Point>& TilemapCamera::onPositionChanged(){  return _d->onPositionChangedSignal;}
void TilemapCamera::moveRight(const int amount){  setCenter( Point( centerX() + amount, centerZ() ) );}
void TilemapCamera::moveLeft(const int amount){  setCenter( Point( centerX() - amount, centerZ() ) );}
void TilemapCamera::moveUp(const int amount){  setCenter( Point( centerX(), centerZ() + amount ) );}
void TilemapCamera::moveDown(const int amount){  setCenter( Point( centerX(), centerZ() - amount ) );}
void TilemapCamera::startFrame(){  _d->resetDrawn(); }

Tile* TilemapCamera::centerTile() const
{
  return at( Point( _d->screenSize.width() / 2, _d->screenSize.height() / 2 ), true );
}

const TilesArray& TilemapCamera::tiles() const
{
  if( _d->tiles.empty() )
  {
    _d->offset.setX( _d->screenSize.width() / 2 - 30 * (_d->centerMapXZ.x() + 1) + 1 );
    _d->offset.setY( _d->screenSize.height()/ 2 + 15 * (_d->centerMapXZ.y() - _d->tilemap->size() + 1) - 30 );

    int mapSize = _d->tilemap->size();
    int zm = _d->tilemap->size() + 1;
    int cx = _d->centerMapXZ.x();
    int cz = _d->centerMapXZ.y();

    Size sizeT = _d->viewSize;  // size x

    std::set< Tile* > overvorderTiles;

    for (int z = cz + sizeT.height(); z>=cz - sizeT.height(); --z)
    {
      // depth axis. from far to near.

      int xstart = cx - sizeT.width();
      if ((xstart + z) % 2 == 0)
      {
	      ++xstart;
      }

      for (int x = xstart; x<=cx + sizeT.width(); x+=2)
      {
	// left-right axis
        int j = (x + z - zm)/2;
        int i = x - j;

        Tile* tile = &_d->tilemap->at( i, j );
        if( (i >= 0) && (j >= 0) && (i < mapSize) && (j < mapSize) )
        {
          _d->tiles.push_back( tile );
        }

        Tile* master = tile->masterTile();
        if( master != NULL )
        {
          Point pos = master->mapPos() + _d->offset;
          std::set< Tile* >::iterator mIt = overvorderTiles.find( master );
          if( pos.x() < 0 && mIt == overvorderTiles.end() )
          {
            _d->tiles.push_back( master );
            overvorderTiles.insert( master );
          }
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

  int mapSize = tilemap->size();
  Point mapOffset = Point( screenSize.width() / 2 - 30 * (centerMapXZ.x() + 1) + 1,
                           screenSize.height() / 2 + 15 * (centerMapXZ.y()-mapSize + 1) - 30 );

  ret.left = !( (tilemap->at( 0, 0 ).mapPos() + mapOffset ).x() > 0);
  ret.right = (tilemap->at( mapSize - 1, mapSize - 1 ).mapPos() + mapOffset).x() > screenSize.width();
  ret.down = ( (tilemap->at( 0, mapSize - 1 ).mapPos() + mapOffset ).y() < 0 );
  ret.up = (tilemap->at( mapSize - 1, 0 ).mapPos() + mapOffset ).y() > screenSize.height();

  return ret;
}

void TilemapCamera::Impl::resetDrawn()
{
  foreach( tile, tiles ) { (*tile)->resetWasDrawn(); }
}

Point TilemapCamera::offset() const{  return _d->offset;}

}//end namespace gfx
