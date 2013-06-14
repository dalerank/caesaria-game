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

#include "oc3_low_bridge.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tile.hpp"

#include <vector>

class LowBridgeSubTile : public Construction
{
public:
  LowBridgeSubTile( const TilePos& pos, int index )
    : Construction( B_LOW_BRIDGE, Size( 1 ) )
  {
    _pos = pos;
    _index = index;

    _picture = Picture::load( ResourceGroup::transport, index );
    _picture.addOffset(30*(_pos.getI()+_pos.getJ()), 15*(_pos.getJ()-_pos.getI()));
  }

  ~LowBridgeSubTile()
  {
  }

  bool isWalkable() const
  {
    return true;
  }

  void build( const TilePos& pos )
  {
    Construction::build( pos );
    _fgPictures.clear();
    _pos = pos;
    _fgPictures.push_back( &Picture::load( ResourceGroup::transport, _index ) );
  }

  void setTerrain( TerrainTile& terrain )
  {
    terrain.clearFlags();
    terrain.setOverlay( this );
    terrain.setRoad( true );
  }

  void destroy()
  {
    if( _parent )
    {
      _parent->deleteLater();
    }
  }

  TilePos _pos;
  int _index;
  int _info;
  int _imgId;
  Picture _picture;
  LowBridge* _parent;
};

typedef SmartPtr< LowBridgeSubTile > LowBridgeSubTilePtr;
typedef std::vector< LowBridgeSubTilePtr > LowBridgeSubTiles;

class LowBridge::Impl
{
public:
  LowBridgeSubTiles subtiles;
  DirectionType direction;

  void addSpan( const TilePos& pos, int index )
  {
    LowBridgeSubTilePtr ret( new LowBridgeSubTile( pos, index ) );
    ret->drop();

    subtiles.push_back( ret );
  }
};

bool LowBridge::canBuild( const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( pos );

  TilePos endPos, startPos;
  _d->direction=D_NONE;
  
  _d->subtiles.clear();
  const_cast< LowBridge* >( this )->_fgPictures.clear();

  _checkParams( _d->direction, startPos, endPos, pos );
 
  if( _d->direction != D_NONE )
  {
    const_cast< LowBridge* >( this )->_computePictures( startPos, endPos, _d->direction );
  }

  return (_d->direction != D_NONE);
}

LowBridge::LowBridge() : Construction( B_LOW_BRIDGE, Size(1) ), _d( new Impl )
{
  setPicture( Picture() );
}

void LowBridge::setTerrain( TerrainTile& terrain )
{

}

void LowBridge::_computePictures( const TilePos& startPos, const TilePos& endPos, DirectionType dir )
{
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  Picture& water = Picture::load( "land1a", 120 );
  switch( dir )
  {
  case D_NORTH_WEST:
    {
      PtrTilesArea tiles = tilemap.getFilledRectangle( endPos, startPos );

      tiles.pop_back();
      tiles.pop_front();

      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 1, 0 ), 67 );
      for( PtrTilesArea::iterator it=tiles.begin(); it != tiles.end(); it++ )
      {
        _d->addSpan( (*it)->getIJ() - startPos, 68 );
      }
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 1, 0 ), 69 );
      //_d->subtiles.push_back( LowBridgeSubTile( tiles.back()->getIJ() - pos + TilePos( 1, 0 ), tile.getPicture() ) );

      for( LowBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); it++ )
      {
        _fgPictures.push_back( &(*it)->_picture );
      }
    }
  break;

  case D_NORTH_EAST:
    {
      PtrTilesArea tiles = tilemap.getFilledRectangle( startPos, endPos );

      tiles.pop_back();
      tiles.pop_front();

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 1 ), 70 );
      for( PtrTilesArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
      {
        _d->addSpan( (*it)->getIJ() - startPos, 71 );
      }
      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 0, 1 ), 72 );
      
      //_d->subtiles.push_back( LowBridgeSubTile( tiles.back()->getIJ() - pos + TilePos( 1, 0 ), tile.getPicture() ) );

      for( LowBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); it++ )
      {
        _fgPictures.push_back( &(*it)->_picture );
      }
    }
    break;

  case D_SOUTH_EAST:
    {
      PtrTilesArea tiles = tilemap.getFilledRectangle( startPos, endPos );

      if( tiles.size() < 3 )
          break;

      tiles.pop_back();
      tiles.pop_front();

      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 1, 0 ), 67 );
      for( PtrTilesArea::iterator it=tiles.begin(); it != tiles.end(); it++ )
      {        
        _d->addSpan( (*it)->getIJ() - startPos, 68 );
        //_d->subtiles.push_back( LowBridgeSubTile( (*it)->getIJ() - startPos, water ) );
      }
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 1, 0 ), 69 );
      //_d->subtiles.push_back( LowBridgeSubTile( tiles.back()->getIJ() - pos + TilePos( 1, 0 ), tile.getPicture() ) );

      for( LowBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); it++ )
      {
        _fgPictures.push_back( &(*it)->_picture );
      }
    }
  break;

  case D_SOUTH_WEST:
    {
      PtrTilesArea tiles = tilemap.getFilledRectangle( endPos, startPos );

      if( tiles.size() < 3 )
        break;

      tiles.pop_back();
      tiles.pop_front();

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 1 ), 70 );
      for( PtrTilesArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
      {        
        _d->addSpan( (*it)->getIJ() - startPos, 71 );
        //_d->subtiles.push_back( LowBridgeSubTile( (*it)->getIJ() - startPos, water ) );
      }
      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 0, 1 ), 72 );
      
      //_d->subtiles.push_back( LowBridgeSubTile( tiles.back()->getIJ() - pos + TilePos( 1, 0 ), tile.getPicture() ) );

      for( LowBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); it++ )
      {
        _fgPictures.push_back( &(*it)->_picture );
      }
    }
  break;

  default:
  break;
  }
}

void LowBridge::_checkParams( DirectionType& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const
{
  start = curPos;

  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  Tile& tile = tilemap.at( curPos );

  int imdId = tile.getTerrain().getOriginalImgId();
  if( imdId == 384 || imdId == 385 || imdId == 386 || imdId == 387 )
  {    
    PtrTilesArea tiles = tilemap.getFilledRectangle( curPos - TilePos( 10, 0), curPos );
    for( PtrTilesArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getTerrain().getOriginalImgId();
      if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379 )
      {
        stop = (*it)->getIJ();
        direction = D_NORTH_WEST;
        break;
      }
    }
  }
  else if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379  )
  {
    PtrTilesArea tiles = tilemap.getFilledRectangle( curPos, curPos + TilePos( 10, 0) );
    for( PtrTilesArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getTerrain().getOriginalImgId();
      if( imdId == 384 || imdId == 385 || imdId == 386 || imdId == 387 )
      {
        stop = (*it)->getIJ();
        direction = D_SOUTH_EAST;
        break;
      }
    }
  }
  else if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375  )
  {
    PtrTilesArea tiles = tilemap.getFilledRectangle( curPos, curPos + TilePos( 0, 10) );
    for( PtrTilesArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getTerrain().getOriginalImgId();
      if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
      {
        stop = (*it)->getIJ();
        direction = D_NORTH_EAST;
        break;
      }
    }
  }
  else if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
  {
    PtrTilesArea tiles = tilemap.getFilledRectangle( curPos - TilePos( 0, 10), curPos );
    for( PtrTilesArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getTerrain().getOriginalImgId();
      if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375 )
      {
        stop = (*it)->getIJ();
        direction = D_SOUTH_WEST;
        break;
      }
    }
  }
  else 
  {
    direction = D_NONE;
  }
}

void LowBridge::build( const TilePos& pos )
{
  TilePos endPos, startPos;
  _d->direction=D_NONE;

  _d->subtiles.clear();
  _fgPictures.clear();

  City& city = Scenario::instance().getCity();
  Tilemap& tilemap = city.getTilemap();

  _checkParams( _d->direction, startPos, endPos, pos );
  int signSum = 1;

  if( _d->direction != D_NONE )
  {    
    switch( _d->direction )
    {
    case D_NORTH_EAST:
      _computePictures( endPos, startPos, D_SOUTH_WEST );
      std::swap( _d->subtiles.front()->_pos, _d->subtiles.back()->_pos );
      signSum = -1;      
    break;

    case D_NORTH_WEST:
      _computePictures( endPos, startPos, D_SOUTH_EAST );
      std::swap( _d->subtiles.front()->_pos, _d->subtiles.back()->_pos );
      std::swap( startPos, endPos );
      signSum = -1;
    break;

    case D_SOUTH_WEST:
      _computePictures( startPos, endPos, _d->direction );        
      std::swap( startPos, endPos );
    break;

    case D_SOUTH_EAST:     
      _computePictures( startPos, endPos, _d->direction ); 
    break;
    }
    
    PtrTilesArea tiles = tilemap.getFilledRectangle( startPos, endPos );
    int index=0;
    for( PtrTilesArea::iterator it=tiles.begin(); it != tiles.end(); it++ )
    {
      LowBridgeSubTilePtr subtile = _d->subtiles[ index ];
      TilePos buildPos = pos + subtile->_pos * signSum;
      Tile& tile = tilemap.at( buildPos );
      subtile->setPicture( tile.getPicture() );
      subtile->_imgId = tile.getTerrain().getOriginalImgId();
      subtile->_info = tile.getTerrain().encode();
      subtile->_parent = this;
      
      city.build( subtile.as<Construction>(), buildPos );
      index++;
    }    
  }
}

void LowBridge::destroy()
{ 
  City& city = Scenario::instance().getCity();
  for( LowBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); it++ )
  {
    (*it)->_parent = 0;
    city.clearLand( (*it)->_pos );

    std::string picName = TerrainTileHelper::convId2PicName( (*it)->_imgId );
    city.getTilemap().at( (*it)->_pos ).setPicture( &Picture::load( picName ) );
    city.getTilemap().at( (*it)->_pos ).getTerrain().decode( (*it)->_info );
  }
}