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

#include "high_bridge.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tile.hpp"
#include "game/city.hpp"
#include "gfx/tilemap.hpp"
#include "events/event.hpp"
#include "constants.hpp"
#include <vector>

using namespace constants;

class HighBridgeSubTile : public Construction
{
public:
   enum { liftingSE=73, spanSE=74, footingSE=79, descentSE=75,
          liftingSW=76, spanSW=77, footingSW=80, descentSW=78,
          liftingSE2=173, descentSE2=175,
          liftingSW2=176, descentSW2=178 };
  HighBridgeSubTile( const TilePos& pos, int index )
    : Construction( building::lowBridge, Size( 1 ) )
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

  void build( PlayerCityPtr city, const TilePos& pos )
  {
    _picture = Picture::load( ResourceGroup::transport, _index % 100 );
    checkSecondPart();
    Construction::build( city, pos );
    _fgPicturesRef().clear();
    _pos = pos;
    _fgPicturesRef().push_back( _picture );
  }

  void initTerrain( Tile& terrain )
  {
    terrain.setFlag( Tile::clearAll, true );
    terrain.setFlag( Tile::tlRoad, true );
  }

  void destroy()
  {
    if( _parent )
    {
      _parent->deleteLater();
    }
  }

  Point getOffset( Tile& tile, const Point& subpos ) const
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
  Direction direction;
  int imgLiftId, imgDescntId;

  void addSpan( const TilePos& pos, int index, bool isFooting=false )
  {
    HighBridgeSubTilePtr ret( new HighBridgeSubTile( pos, index ) );
    ret->drop();

    subtiles.push_back( ret );
  }
};

bool HighBridge::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& ) const
{
  //bool is_constructible = Construction::canBuild( pos );

  TilePos endPos, startPos;
  _d->direction=noneDirection;
  
  _d->subtiles.clear();
  const_cast< HighBridge* >( this )->_fgPicturesRef().clear();

  _checkParams( city, _d->direction, startPos, endPos, pos );
 
  if( _d->direction != noneDirection )
  {
    const_cast< HighBridge* >( this )->_computePictures( city, startPos, endPos, _d->direction );
  }

  return (_d->direction != noneDirection );
}

HighBridge::HighBridge() : Construction( building::highBridge, Size(1) ), _d( new Impl )
{
  Picture tmp;
  setPicture( tmp );
}

void HighBridge::initTerrain(Tile& terrain )
{

}

void HighBridge::_computePictures( PlayerCityPtr city, const TilePos& startPos, const TilePos& endPos, Direction dir )
{
  Tilemap& tilemap = city->getTilemap();
  //Picture& water = Picture::load( "land1a", 120 );
  switch( dir )
  {
  case constants::northWest:
    {
      TilesArray tiles = tilemap.getArea( endPos, startPos );

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

  case northEast:
    {
      TilesArray tiles = tilemap.getArea( startPos, endPos );

      tiles.pop_back();
      tiles.pop_back();
      tiles.pop_front();
      TilePos liftPos = tiles.front()->getIJ();
      tiles.pop_front();

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 1 ), HighBridgeSubTile::liftingSW );
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 2 ), HighBridgeSubTile::liftingSW2 );

      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
      {
        _d->addSpan( (*it)->getIJ() - startPos, HighBridgeSubTile::spanSW );
      }

      _d->addSpan( liftPos - startPos, HighBridgeSubTile::descentSW2 );
      _d->addSpan( liftPos - startPos - TilePos( 0, 1 ), HighBridgeSubTile::descentSW );    
    }
    break;

  case southEast:
    {
      TilesArray tiles = tilemap.getArea( startPos, endPos );

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

  case constants::southWest:
    {
      TilesArray tiles = tilemap.getArea( endPos, startPos );
      
      tiles.pop_back();
      tiles.pop_back();
      
      tiles.pop_front();  
      TilePos liftPos = tiles.front()->getIJ();
      tiles.pop_front();

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 1 ), HighBridgeSubTile::liftingSW );
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 2 ), HighBridgeSubTile::liftingSW2 );
      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
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

  foreach( HighBridgeSubTilePtr tile, _d->subtiles )
  {
    _fgPicturesRef().push_back( tile->_picture );
  }
}

void HighBridge::_checkParams(PlayerCityPtr city, Direction& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const
{
  start = curPos;

  Tilemap& tilemap = city->getTilemap();
  Tile& tile = tilemap.at( curPos );

  if( tile.getFlag( Tile::tlRoad ) )
  {
    direction = constants::noneDirection;
    return;
  }

  int imdId = tile.getOriginalImgId();
  if( imdId == 384 || imdId == 385 || imdId == 386 || imdId == 387 )
  {    
    TilesArray tiles = tilemap.getArea( curPos - TilePos( 10, 0), curPos );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getOriginalImgId();
      if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379 )
      {
        stop = (*it)->getIJ();
        direction = abs( stop.getI() - start.getI() ) > 3 ? northWest : noneDirection;
        break;
      }
    }
  }
  else if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379  )
  {
    TilesArray tiles = tilemap.getArea( curPos, curPos + TilePos( 10, 0) );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getOriginalImgId();
      if( imdId == 384 || imdId == 385 || imdId == 386 || imdId == 387 )
      {
        stop = (*it)->getIJ();
        direction = abs( stop.getI() - start.getI() ) > 3 ? southWest : noneDirection;
        break;
      }
    }
  }
  else if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375  )
  {
    TilesArray tiles = tilemap.getArea( curPos, curPos + TilePos( 0, 10) );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getOriginalImgId();
      if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
      {
        stop = (*it)->getIJ();
        direction = abs( stop.getJ() - start.getJ() ) > 3 ? northEast : noneDirection;
        break;
      }
    }
  }
  else if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
  {
    TilesArray tiles = tilemap.getArea( curPos - TilePos( 0, 10), curPos );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
    {
      imdId = (*it)->getOriginalImgId();
      if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375 )
      {
        stop = (*it)->getIJ();
        direction = abs( stop.getJ() - start.getJ() ) > 3 ? southWest : noneDirection;
        break;
      }
    }
  }
  else 
  {
    direction = noneDirection;
  }
}

void HighBridge::build(PlayerCityPtr city, const TilePos& pos )
{
  TilePos endPos, startPos;
  _d->direction=noneDirection;

  _d->subtiles.clear();
  _fgPicturesRef().clear();

  Tilemap& tilemap = city->getTilemap();

  _checkParams( city, _d->direction, startPos, endPos, pos );

  if( _d->direction != noneDirection )
  {    
    _computePictures( city, startPos, endPos, _d->direction );
   
    foreach( HighBridgeSubTilePtr subtile, _d->subtiles )
    {
      TilePos buildPos = pos + subtile->_pos;
      Tile& tile = tilemap.at( buildPos );
      subtile->setPicture( tile.getPicture() );
      subtile->_imgId = tile.getOriginalImgId();
      subtile->_info = TileHelper::encode( tile );
      subtile->_parent = this;
      
      events::GameEventPtr event = events::BuildEvent::create( buildPos, subtile.as<TileOverlay>() );
      event->dispatch();
    }    
  }
}

void HighBridge::destroy()
{ 
  PlayerCityPtr city = _getCity();
  foreach( HighBridgeSubTilePtr subtile,  _d->subtiles )
  {
    subtile->_parent = 0;
    events::GameEventPtr event = events::ClearLandEvent::create( subtile->_pos );
    event->dispatch();

    std::string picName = TileHelper::convId2PicName( subtile->_imgId );

    Tile& mapTile = city->getTilemap().at( subtile->_pos );
    mapTile.setPicture( &Picture::load( picName ) );

    TileHelper::decode( mapTile, subtile->_info );
  }
}
