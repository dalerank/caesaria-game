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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "tilemap_camera.hpp"

#include "core/position.hpp"
#include "core/size.hpp"
#include "core/exception.hpp"
#include "core/logger.hpp"
#include "gfx/helper.hpp"
#include "core/foreach.hpp"
#include "objects/overlay.hpp"

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

enum { zoomStep=10, zoomMiniminal=30, zoomDefault=100, zoomMaximum=300 };

class TilemapCamera::Impl
{
public:
  Size tileMapSize;
  Size virtualSize;
  Size screenSize;
  Size borderSize;
  Point offset;

  int scrollSpeed;
  int zoom;
  bool zoomAlsoLess;

  Tilemap* tmap;   // tile map to display
  PointF centerMapXZ; // center of the view(in tiles)
  Size viewSize;      // width of the view(in tiles)  nb_tilesX = 1+2*_view_width
                      // height of the view(in tiles) nb_tilesY = 1+2*_view_height 
  struct
  {
    TilesArray visible;   // cached list of visible tiles
    TilesArray flat;
    TilesArray ground;
  } tiles;

  MovableOrders mayMove( PointF point );
  void resetDrawn();

  Point getOffset( const PointF& center );

  void updateFlatTiles();

  struct {
    Signal1<Point> onPositionChanged;
    Signal1<Direction> onDirectionChanged;
  } signal;
};

TilemapCamera::TilemapCamera() : _d( new Impl )
{
  _d->tileMapSize = tilemap::cellSize();
  _d->tmap = NULL;
  _d->scrollSpeed = 30;
  _d->viewSize = Size( 0 );
  _d->screenSize = Size( 0 );
  _d->virtualSize = Size( 0 );
  _d->centerMapXZ = PointF( 0, 0 );
  _d->zoomAlsoLess = false;
  _d->borderSize = Size( gfx::tilemap::cellSize().width() * 4 );
  _d->tiles.visible.reserve( 2000 );
  _d->zoom = 100;
}

TilemapCamera::~TilemapCamera() {}

void TilemapCamera::init(Tilemap &tilemap, Size size)
{
  _d->tmap = &tilemap;
  _d->screenSize = size;
}

void TilemapCamera::setViewport(Size newSize)
{
  if( _d->viewSize != newSize )
  {
    _d->tiles.visible.clear();
  }

  _d->virtualSize = newSize;

  newSize += _d->borderSize;
  Size vpSize( tilemap::cellPicSize().height() * 2, tilemap::cellPicSize().height() );
  _d->viewSize = Size( ( newSize.width() + (vpSize.width()-1)) / vpSize.width(),
                       ( newSize.height() + (vpSize.height()-1)) / vpSize.height() );
  
  Logger::warning( "TilemapArea::setViewport w=%d h=%d", _d->viewSize.width(), _d->viewSize.height() );
}

void TilemapCamera::setCenter(TilePos pos, bool checkCorner)
{
  Tile tt( pos );
  Tile lctile( _d->tmap->p2tp( Point( _d->screenSize.width() / 2, _d->screenSize.height() / 2 )) );
  if( tt.mappos().x() < lctile.mappos().x() )
  {
    pos = lctile.pos();
  }

  _setCenter( Point( pos.i() + pos.j(), _d->tmap->size() - 1 + pos.j() - pos.i() ), checkCorner );

  emit _d->signal.onPositionChanged( _d->centerMapXZ.toPoint() );
}

void TilemapCamera::move(PointF relative)
{
  float koeffX = (float)_d->virtualSize.width() / (float)_d->screenSize.width();
  relative *= _d->scrollSpeed / 100.f * koeffX;

  MovableOrders mv = _d->mayMove( _d->centerMapXZ + relative);

  if( relative.x() < 0 && !mv.left ) { relative.setX( 0 ); }
  if( relative.x() > 0 && !mv.right ) { relative.setX( 0 ); }
  if( relative.y() < 0 && !mv.up ) { relative.setY( 0 ); }
  if( relative.y() > 0 && !mv.down ) { relative.setY( 0 ); }

  if( mv.any() )
  {
    Tile* currentCenterTile = centerTile();
    _d->centerMapXZ += relative;
    _d->offset = _d->getOffset( _d->centerMapXZ );
    Tile* newCenterTile = centerTile();

    if( currentCenterTile != newCenterTile )
    {
      _d->resetDrawn();
      _d->tiles.visible.clear();
    }

    emit _d->signal.onPositionChanged( _d->centerMapXZ.toPoint() );
  }
}

void TilemapCamera::_setCenter(Point pos, bool checkBorder)
{ 
  if( _d->centerMapXZ.toPoint() != pos  )
  {
    _d->tiles.visible.clear();

    Point futureOffset = _d->getOffset( pos.toPointF() );
    int mapsize = _d->tmap->size();
    Point tile00 = _d->tmap->at( 0, 0 ).mappos();
    Point tile0x = _d->tmap->at( 0, mapsize-1 ).mappos();
    Point tilex0 = _d->tmap->at( mapsize-1, 0 ).mappos();
    Point tilexx = _d->tmap->at( mapsize-1, mapsize-1 ).mappos();
    if( checkBorder )
    {
      if( futureOffset.x() + tile00.x() > 0
        || (futureOffset.y() + tile0x.y() > 0)
        || (futureOffset.x() + tilexx.x() < _d->virtualSize.width() )
        || (futureOffset.y() + tilex0.y() < _d->virtualSize.height() ) )
        return;
    }
  }  

  _d->centerMapXZ = pos.toPointF();
  emit _d->signal.onPositionChanged( _d->centerMapXZ.toPoint() );
}

TilePos TilemapCamera::center() const
{
  Tile* tile = centerTile();
  return tile ? tile->epos() : gfx::tilemap::invalidLocation();
}

void TilemapCamera::changeZoom(int delta)
{
  int newZoom = math::clamp<int>( _d->zoom + delta, zoomMiniminal, zoomMaximum );

  int mapLast = _d->tmap->size() - 1;
  const Tile& l00 = _d->tmap->at( 0, 0 );
  const Tile& l01 = _d->tmap->at( 0, mapLast );
  const Tile& l10 = _d->tmap->at( mapLast, 0 );
  const Tile& l11 = _d->tmap->at( mapLast, mapLast );

  float koeff = newZoom / 100.f;
  float horizontalDistance = l00.mappos().distanceTo( l11.mappos() ) * koeff;
  float verticalDistance = l01.mappos().distanceTo( l10.mappos() ) * koeff;
  if( horizontalDistance < _d->screenSize.width()
      && verticalDistance < _d->screenSize.height() )
  {
    if( newZoom < _d->zoom )
    {
      if( !_d->zoomAlsoLess )
        _d->zoomAlsoLess = true;
      else
        return;
    }
  }
  _d->zoomAlsoLess = false;
  _d->zoom = newZoom;
}

void TilemapCamera::setZoom(int value) { _d->zoom = value; }
int TilemapCamera::zoom() const{ return _d->zoom; }
int TilemapCamera::maxZoom() const { return zoomMaximum; }
int TilemapCamera::centerX() const  {   return _d->centerMapXZ.x();   }
int TilemapCamera::centerZ() const  {   return _d->centerMapXZ.y();   }
void TilemapCamera::setScrollSpeed(int speed){  _d->scrollSpeed = speed; }
int TilemapCamera::scrollSpeed() const{ return _d->scrollSpeed; }
Tile* TilemapCamera::at(const TilePos& pos) const { return &_d->tmap->at( pos ); }
Signal1<Point>& TilemapCamera::onPositionChanged(){  return _d->signal.onPositionChanged;}
Signal1<Direction>& TilemapCamera::onDirectionChanged(){  return _d->signal.onDirectionChanged;}
void TilemapCamera::moveRight(const int amount){  _setCenter( Point( centerX() + amount, centerZ() ), true );}
void TilemapCamera::moveLeft(const int amount){  _setCenter( Point( centerX() - amount, centerZ() ), true );}
void TilemapCamera::moveUp(const int amount){  _setCenter( Point( centerX(), centerZ() + amount ), true );}
void TilemapCamera::moveDown(const int amount){  _setCenter( Point( centerX(), centerZ() - amount ), true );}
void TilemapCamera::startFrame(){  _d->resetDrawn(); }
void TilemapCamera::refresh(){  _d->tiles.visible.clear(); }

Tile* TilemapCamera::at(const Point& pos, bool overborder) const
{
  float koeffX = (float)_d->virtualSize.width() / (float)_d->screenSize.width();
  float koeffY = (float)_d->virtualSize.height() / (float)_d->screenSize.height();
  Point virtPos = Point( pos.x() * koeffX, pos.y() * koeffY );
  return _d->tmap->at( virtPos - _d->offset, overborder );
}

Point TilemapCamera::mpos(const Point &pos) const
{
  float koeffX = (float)_d->virtualSize.width() / (float)_d->screenSize.width();
  float koeffY = (float)_d->virtualSize.height() / (float)_d->screenSize.height();
  Point virtPos = Point( pos.x() * koeffX, pos.y() * koeffY );
  return virtPos - _d->offset;
}

Tile* TilemapCamera::centerTile() const
{
  return at( Point( _d->virtualSize.width() / 2, _d->virtualSize.height() / 2 ), true );
}

const TilesArray& TilemapCamera::tiles() const
{
  if( _d->tiles.visible.empty() )
  {
    _d->offset = _d->getOffset( _d->centerMapXZ );    

    int mapSize = _d->tmap->size();
    int zm = _d->tmap->size() + 1;
    int cx = _d->centerMapXZ.x();
    int cz = _d->centerMapXZ.y();

    Size sizeT = _d->viewSize;  // size x

    std::set< Tile* > overborderTiles;

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

        Tile* tile = &_d->tmap->at( i, j );
        if( (i >= 0) && (j >= 0) && (i < mapSize) && (j < mapSize) )
        {
          _d->tiles.visible.push_back( tile );
        }

        Tile* master = tile->master();
        if( master != NULL )
        {
          Point pos = master->mappos() + _d->offset;
          std::set< Tile* >::iterator mIt = overborderTiles.find( master );
          if( pos.x() < 0 && mIt == overborderTiles.end() )
          {
            _d->tiles.visible.push_back( master );
            overborderTiles.insert( master );
          }
        }
      }
    }

    _d->updateFlatTiles();
  }

  return _d->tiles.visible;
}

const TilesArray& TilemapCamera::flatTiles() const { return _d->tiles.flat; }
const TilesArray& TilemapCamera::groundTiles() const { return _d->tiles.ground; }

MovableOrders TilemapCamera::Impl::mayMove(PointF )
{
  MovableOrders ret = { true, true, true, true };

  int mapSize = tmap->size();
  Point mapOffset = getOffset( centerMapXZ );

  ret.left = !( (tmap->at( 0, 0 ).mappos() + mapOffset ).x() > 0);
  ret.right = (tmap->at( mapSize - 1, mapSize - 1 ).mappos() + mapOffset).x() > virtualSize.width();
  ret.down = ( (tmap->at( 0, mapSize - 1 ).mappos() + mapOffset ).y() < 0 );
  ret.up = (tmap->at( mapSize - 1, 0 ).mappos() + mapOffset ).y() > virtualSize.height();

  return ret;
}

void TilemapCamera::Impl::resetDrawn()
{
  for( auto i : tiles.visible ) { i->resetRendered(); }
  for( auto i : tiles.flat ) { i->resetRendered(); }
}

Point TilemapCamera::Impl::getOffset(const PointF &center)
{
  return Point( virtualSize.width() / 2  - tileMapSize.width()  * (center.x() + 1) + 1,
                virtualSize.height() / 2 + tileMapSize.height() * (center.y() - tmap->size() + 1) - tileMapSize.width() );
}

void TilemapCamera::Impl::updateFlatTiles()
{
  unsigned int reserve = tiles.flat.size();
  tiles.flat.clear();
  tiles.ground.clear();
  tiles.flat.reserve( reserve );

  resetDrawn();
  for( auto tile : tiles.visible )
  {
    if( tile->getFlag( Tile::tlElevation ) ||
        tile->getFlag( Tile::tlRock ) )
      continue;

    tiles.ground.push_back( tile );
  }

  for( auto t : tiles.visible )
  {
    int z = t->epos().z();
    Tile* tile = t->master();
    if( !tile )
      tile = t;

    if( tile->isFlat() && tile->epos().z() == z && !tile->rendered() )
    {
      tile->setRendered();
      tiles.flat.push_back( tile );
    }
  }

  const TilesArray& tl = tmap->svkBorderTiles();
  Rect viewRect( Point( -tilemap::cellPicSize().width(), -tilemap::cellPicSize().height() ),
                 virtualSize + tilemap::cellPicSize() * 2 );
  for( auto i : tl )
  {
    if( viewRect.isPointInside( i->mappos() + Point( tilemap::cellPicSize().width()/2, 0 ) + offset)  )
    {
      tiles.flat.push_back( i );
    }
  }
}

Point TilemapCamera::offset() const{  return _d->offset;}

}//end namespace gfx
