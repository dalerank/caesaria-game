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

#include "oc3_high_bridge.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tile.hpp"
#include "oc3_city.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_scenario_event.hpp"

#include <vector>

class HighBridgeSubTile : public Construction
{
public:
   enum { liftingSE=73, spanSE=74, footingSE=79, descentSE=75,
          liftingSW=76, spanSW=77, footingSW=80, descentSW=78,
          liftingSE2=173, descentSE2=175,
          liftingSW2=176, descentSW2=178 };
  HighBridgeSubTile( const TilePos& pos, int index )
    : Construction( B_LOW_BRIDGE, Size( 1 ) )
  {
    _pos = pos;
    _index = index;

    _picture = Picture::load( ResourceGroup::transport, index % 100 );
    _picture.addOffset(30*(_pos.getI()+_pos.getJ()), 15*(_pos.getJ()-_pos.getI()));
    checkSecondPart();
  }

  void checkSecondPart()
  {
    switch( _index )
    {
    case liftingSW2: _picture.addOffset( -29, -16 ); break;
    case descentSW2: _picture.addOffset( -29, -16 ); break;
    
    case liftingSE2:  _picture.addOffset( -29, 16 ); _picture.addOffset( 8, -13 ); break;
    case liftingSE: _picture.addOffset( 8, -13 ); break;
    case spanSE: _picture.addOffset( 8, -13 ); break;
    case descentSE2: _picture.addOffset( -29, 16 ); _picture.addOffset( 8, -13 ); break;
    case descentSE: _picture.addOffset( 8, -13 ); break;
    }
  }

  ~HighBridgeSubTile()
  {
  }

  bool isWalkable() const
  {
    return true;
  }

  void build( const TilePos& pos )
  {
    _picture = Picture::load( ResourceGroup::transport, _index % 100 );
    checkSecondPart();
    Construction::build( pos );
    _fgPictures.clear();
    _pos = pos;
    _fgPictures.push_back( _picture );
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

  Point getOffset( const Point& subpos ) const
  {
    switch( _index )
    {
    case liftingSE: return Point( 0, subpos.getX() );
    case spanSE:    return Point( 0, 10 );
    case footingSE: return Point( 0, 10 );
    case descentSE: return Point( 0, 10 - subpos.getX() );
    case descentSW: return Point( -subpos.getY(), 0 );
    case spanSW:    return Point( -10, 0 );
    case footingSW: return Point( -10, 0 );
    case liftingSW: return Point( -(10 - subpos.getY()), 0 );

    default: return Point( 0, 0 );
    }
  }

  TilePos _pos;
  int _index;
  int _info;
  int _imgId;
  Picture _picture;
  HighBridge* _parent;
};

typedef SmartPtr< HighBridgeSubTile > HighBridgeSubTilePtr;
typedef std::vector< HighBridgeSubTilePtr > HighBridgeSubTiles;

class HighBridge::Impl
{
public:
  HighBridgeSubTiles subtiles;
  DirectionType direction;
  int imgLiftId, imgDescntId;

  void addSpan( const TilePos& pos, int index, bool isFooting=false )
  {
    HighBridgeSubTilePtr ret( new HighBridgeSubTile( pos, index ) );
    ret->drop();

    subtiles.push_back( ret );
  }
};

bool HighBridge::canBuild( const TilePos& pos ) const
{
  //bool is_constructible = Construction::canBuild( pos );

  TilePos endPos, startPos;
  _d->direction=D_NONE;
  
  _d->subtiles.clear();
  const_cast< HighBridge* >( this )->_fgPictures.clear();

  _checkParams( _d->direction, startPos, endPos, pos );
 
  if( _d->direction != D_NONE )
  {
    const_cast< HighBridge* >( this )->_computePictures( startPos, endPos, _d->direction );
  }

  return (_d->direction != D_NONE);
}

HighBridge::HighBridge() : Construction( B_HIGH_BRIDGE, Size(1) ), _d( new Impl )
{
  Picture tmp;
  setPicture( tmp );
}

void HighBridge::setTerrain( TerrainTile& terrain )
{

}

void HighBridge::_computePictures( const TilePos& startPos, const TilePos& endPos, DirectionType dir )
{
  Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
  //Picture& water = Picture::load( "land1a", 120 );
  switch( dir )
  {
  case D_NORTH_WEST:
    {
      TilemapArea tiles = tilemap.getFilledRectangle( endPos, startPos );

      tiles.pop_back();
      tiles.pop_back();
      
      tiles.pop_front();      
      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 1, 0 ), HighBridgeSubTile::liftingSE );
      _d->addSpan( tiles.front()->getIJ() - startPos, HighBridgeSubTile::liftingSE2 );
      tiles.pop_front();

      foreach( Tile* tile, tiles )
      {
        _d->addSpan( tile->getIJ() - startPos, HighBridgeSubTile::spanSE );
      }

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 1, 0 ), HighBridgeSubTile::descentSE );     
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 2, 0 ), HighBridgeSubTile::descentSE2 );     
    }
  break;

  case D_NORTH_EAST:
    {
      TilemapArea tiles = tilemap.getFilledRectangle( startPos, endPos );

      tiles.pop_back();
      tiles.pop_back();
      tiles.pop_front();
      TilePos liftPos = tiles.front()->getIJ();
      tiles.pop_front();

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 1 ), HighBridgeSubTile::liftingSW );
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 2 ), HighBridgeSubTile::liftingSW2 );

      for( TilemapArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
      {
        _d->addSpan( (*it)->getIJ() - startPos, HighBridgeSubTile::spanSW );
      }

      _d->addSpan( liftPos - startPos, HighBridgeSubTile::descentSW2 );
      _d->addSpan( liftPos - startPos - TilePos( 0, 1 ), HighBridgeSubTile::descentSW );    
    }
    break;

  case D_SOUTH_EAST:
    {
      TilemapArea tiles = tilemap.getFilledRectangle( startPos, endPos );

      tiles.pop_back();
      tiles.pop_back();

      tiles.pop_front();
      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 1, 0 ), HighBridgeSubTile::liftingSE );
      _d->addSpan( tiles.front()->getIJ() - startPos, HighBridgeSubTile::liftingSE2 );      
      tiles.pop_front();

      foreach( Tile* tile, tiles )
      {        
        _d->addSpan( tile->getIJ() - startPos, HighBridgeSubTile::spanSE );
      }

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 1, 0 ), HighBridgeSubTile::descentSE );
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 2, 0 ), HighBridgeSubTile::descentSE2 );
    }
  break;

  case D_SOUTH_WEST:
    {
      TilemapArea tiles = tilemap.getFilledRectangle( endPos, startPos );
      
      tiles.pop_back();
      tiles.pop_back();
      
      tiles.pop_front();  
      TilePos liftPos = tiles.front()->getIJ();
      tiles.pop_front();

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 1 ), HighBridgeSubTile::liftingSW );
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 2 ), HighBridgeSubTile::liftingSW2 );
      for( TilemapArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
      {        
        _d->addSpan( (*it)->getIJ() - startPos, HighBridgeSubTile::spanSW );
      }
      _d->addSpan( liftPos - startPos, HighBridgeSubTile::descentSW2 );
      _d->addSpan( liftPos - startPos - TilePos( 0, 1 ), HighBridgeSubTile::descentSW );
    }
  break;

  default:
  break;
  }

  for( HighBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); it++ )
  {
    _fgPictures.push_back( (*it)->_picture );
  }
}

void HighBridge::_checkParams( DirectionType& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const
{
  start = curPos;

  Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
  Tile& tile = tilemap.at( curPos );

  if( tile.getTerrain().isRoad() )
  {
    direction = D_NONE;
    return;
  }

  int imdId = tile.getTerrain().getOriginalImgId();
  if( imdId == 384 || imdId == 385 || imdId == 386 || imdId == 387 )
  {    
    TilemapArea tiles = tilemap.getFilledRectangle( curPos - TilePos( 10, 0), curPos );
    for( TilemapArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getTerrain().getOriginalImgId();
      if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379 )
      {
        stop = (*it)->getIJ();
        direction = abs( stop.getI() - start.getI() ) > 3 ? D_NORTH_WEST : D_NONE;
        break;
      }
    }
  }
  else if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379  )
  {
    TilemapArea tiles = tilemap.getFilledRectangle( curPos, curPos + TilePos( 10, 0) );
    for( TilemapArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getTerrain().getOriginalImgId();
      if( imdId == 384 || imdId == 385 || imdId == 386 || imdId == 387 )
      {
        stop = (*it)->getIJ();
        direction = abs( stop.getI() - start.getI() ) > 3 ? D_SOUTH_EAST : D_NONE;
        break;
      }
    }
  }
  else if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375  )
  {
    TilemapArea tiles = tilemap.getFilledRectangle( curPos, curPos + TilePos( 0, 10) );
    for( TilemapArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getTerrain().getOriginalImgId();
      if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
      {
        stop = (*it)->getIJ();
        direction = abs( stop.getJ() - start.getJ() ) > 3 ? D_NORTH_EAST : D_NONE;
        break;
      }
    }
  }
  else if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
  {
    TilemapArea tiles = tilemap.getFilledRectangle( curPos - TilePos( 0, 10), curPos );
    for( TilemapArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getTerrain().getOriginalImgId();
      if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375 )
      {
        stop = (*it)->getIJ();
        direction = abs( stop.getJ() - start.getJ() ) > 3 ? D_SOUTH_WEST : D_NONE;
        break;
      }
    }
  }
  else 
  {
    direction = D_NONE;
  }
}

void HighBridge::build( const TilePos& pos )
{
  TilePos endPos, startPos;
  _d->direction=D_NONE;

  _d->subtiles.clear();
  _fgPictures.clear();

  CityPtr city = Scenario::instance().getCity();
  Tilemap& tilemap = city->getTilemap();

  _checkParams( _d->direction, startPos, endPos, pos );

  if( _d->direction != D_NONE )
  {    
    _computePictures( startPos, endPos, _d->direction );
   
    foreach( HighBridgeSubTilePtr subtile, _d->subtiles )
    {
      TilePos buildPos = pos + subtile->_pos;
      Tile& tile = tilemap.at( buildPos );
      subtile->setPicture( tile.getPicture() );
      subtile->_imgId = tile.getTerrain().getOriginalImgId();
      subtile->_info = tile.getTerrain().encode();
      subtile->_parent = this;
      
      BuildEvent::create( buildPos, subtile.as<Construction>() );
    }    
  }
}

void HighBridge::destroy()
{ 
  CityPtr city = Scenario::instance().getCity();
  foreach( HighBridgeSubTilePtr subtile,  _d->subtiles )
  {
    subtile->_parent = 0;
    ClearLandEvent::create( subtile->_pos );

    std::string picName = TerrainTileHelper::convId2PicName( subtile->_imgId );
    city->getTilemap().at( subtile->_pos ).setPicture( &Picture::load( picName ) );
    city->getTilemap().at( subtile->_pos ).getTerrain().decode( subtile->_info );
  }
}
