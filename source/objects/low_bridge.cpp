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

#include "low_bridge.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tile.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "events/event.hpp"
#include "constants.hpp"
#include "walker/walker.hpp"

#include <vector>

using namespace constants;

class LowBridgeSubTile : public Construction
{
public:
  enum { liftingSE=67, spanSE=68, descentSE=69, liftingSW=70, spanSW=71, descentSW=72 };
  LowBridgeSubTile( const TilePos& pos, int index )
    : Construction( building::lowBridge, Size( 1 ) )
  {
    _pos = pos;
    _index = index;
    _parent = 0;
    _picture = Picture::load( ResourceGroup::transport, index );
    _picture.addOffset( Point( 30*(_pos.i()+_pos.j()), 15*(_pos.j()-_pos.i()) - 10 ) );
  }

  ~LowBridgeSubTile()
  {
  }

  std::string getError() const { return _parent ? _parent->getError() : "";  }
  bool isWalkable() const { return true;  }

  void build( PlayerCityPtr city, const TilePos& pos )
  {
    Construction::build( city, pos );
    _fgPicturesRef().clear();
    _pos = pos;
    _picture = Picture::load( ResourceGroup::transport, _index );
    _picture.addOffset( Point( 10, -10 ) );
    _fgPicturesRef().push_back( _picture );
  }

  void initTerrain( Tile& terrain )
  {
    bool isWater = terrain.getFlag( Tile::tlWater );
    terrain.setFlag( Tile::clearAll, true );
    terrain.setFlag( Tile::tlWater, isWater );
    terrain.setFlag( Tile::tlRoad, true );
  }

  bool canDestroy() const  {    return _parent->canDestroy();  }

  void destroy()
  {
    if( _parent )
    {
      _parent->deleteLater();
    }
  }

  void save(VariantMap &stream) const
  {
    if( getTilePos() == _parent->getTilePos() )
    {
      return _parent->save( stream );
    }
  }

  Point getOffset( Tile& , const Point& subpos ) const
  {
    switch( _index )
    {
    case liftingSE: return Point( -subpos.x()*0.9, subpos.x()*0.7 );
    case spanSE:    return Point( -15, 12 );
    case descentSE: return Point( -10 + subpos.x(), 12 - subpos.x() * 0.7 );
    case descentSW: return Point( -subpos.y()*0.5, subpos.y()*0.9 );
    case spanSW:    return Point( -8, 25 );
    case liftingSW: return Point( subpos.y()*0.6, 20-subpos.y()*0.4 );

    default: return Point( 0, 0 );
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
  Direction direction;
  std::string error;

  void addSpan( const TilePos& pos, int index )
  {
    LowBridgeSubTilePtr ret( new LowBridgeSubTile( pos, index ) );
    ret->drop();

    subtiles.push_back( ret );
  }
};

bool LowBridge::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& ) const
{
  //bool is_constructible = Construction::canBuild( pos );

  TilePos endPos, startPos;
  _d->direction=noneDirection;

  TileOverlayPtr bridge = city->getOverlay( pos );
  if( bridge.isNull() )
  {
    _d->subtiles.clear();
    const_cast< LowBridge* >( this )->_fgPicturesRef().clear();

    _checkParams( city, _d->direction, startPos, endPos, pos );

    if( _d->direction != noneDirection )
    {
      const_cast< LowBridge* >( this )->_computePictures( city, startPos, endPos, _d->direction );
    }
  }

  return (_d->direction != noneDirection);
}

LowBridge::LowBridge() : Construction( constants::building::lowBridge, Size(1) ), _d( new Impl )
{
  Picture pic;
  setPicture( pic );
}

void LowBridge::initTerrain(Tile& terrain )
{

}

void LowBridge::_computePictures(PlayerCityPtr city, const TilePos& startPos, const TilePos& endPos, constants::Direction dir )
{
  Tilemap& tilemap = city->getTilemap();
  //Picture& water = Picture::load( "land1a", 120 );
  switch( dir )
  {
  case northWest:
    {
      TilesArray tiles = tilemap.getArea( endPos, startPos );

      tiles.pop_back();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 1, 0 ), LowBridgeSubTile::liftingSE );
      for( TilesArray::iterator it=tiles.begin(); it != tiles.end(); it++ )
      {
        _d->addSpan( (*it)->getIJ() - startPos, LowBridgeSubTile::spanSE );
      }
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 1, 0 ), LowBridgeSubTile::descentSE );     
    }
  break;

  case northEast:
    {
      TilesArray tiles = tilemap.getArea( startPos, endPos );

      tiles.pop_back();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 1 ), LowBridgeSubTile::liftingSW );
      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
      {
        _d->addSpan( (*it)->getIJ() - startPos, LowBridgeSubTile::spanSW );
      }
      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 0, 1 ), LowBridgeSubTile::descentSW );    
    }
    break;

  case southEast:
    {
      TilesArray tiles = tilemap.getArea( startPos, endPos );

      if( tiles.size() < 3 )
          break;

      tiles.pop_back();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 1, 0 ), LowBridgeSubTile::liftingSE );
      for( TilesArray::iterator it=tiles.begin(); it != tiles.end(); it++ )
      {        
        _d->addSpan( (*it)->getIJ() - startPos, LowBridgeSubTile::spanSE );
        //_d->subtiles.push_back( LowBridgeSubTile( (*it)->getIJ() - startPos, water ) );
      }
      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 1, 0 ), LowBridgeSubTile::descentSE );
    }
  break;

  case southWest:
    {
      TilesArray tiles = tilemap.getArea( endPos, startPos );

      if( tiles.size() < 3 )
        break;

      tiles.pop_back();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.back()->getIJ() - startPos + TilePos( 0, 1 ), LowBridgeSubTile::liftingSW );
      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
      {        
        _d->addSpan( (*it)->getIJ() - startPos, LowBridgeSubTile::spanSW );
        //_d->subtiles.push_back( LowBridgeSubTile( (*it)->getIJ() - startPos, water ) );
      }
      _d->addSpan( tiles.front()->getIJ() - startPos - TilePos( 0, 1 ), LowBridgeSubTile::descentSW );
    }
  break;

  default:
  break;
  }

  for( LowBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); it++ )
  {
    _fgPicturesRef().push_back( (*it)->_picture );
  }
}

void LowBridge::_checkParams(PlayerCityPtr city, constants::Direction& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const
{
  start = curPos;

  Tilemap& tilemap = city->getTilemap();
  Tile& tile = tilemap.at( curPos );

  /*if( tile.getFlag( Tile::tlRoad ) )
  {
    direction = constants::noneDirection;
    return;
  }*/

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
        direction = constants::northWest;
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
        direction = southEast;
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
        direction = northEast;
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
        direction = southWest;
        break;
      }
    }
  }
  else 
  {
    direction = noneDirection;
  }
}

void LowBridge::build(PlayerCityPtr city, const TilePos& pos )
{
  TilePos endPos, startPos;
  _d->direction=noneDirection;
  setSize( Size(0) );
  Construction::build( city, pos );


  _d->subtiles.clear();
  _fgPicturesRef().clear();

  Tilemap& tilemap = city->getTilemap();

  _checkParams( city, _d->direction, startPos, endPos, pos );
  int signSum = 1;

  if( _d->direction != noneDirection )
  {    
    switch( _d->direction )
    {
    case northEast:
      _computePictures( city, endPos, startPos, southWest );
      std::swap( _d->subtiles.front()->_pos, _d->subtiles.back()->_pos );
      signSum = -1;      
    break;

    case northWest:
      _computePictures( city, endPos, startPos, southEast );
      std::swap( _d->subtiles.front()->_pos, _d->subtiles.back()->_pos );
      std::swap( startPos, endPos );
      signSum = -1;
    break;

    case southWest:
      _computePictures( city, startPos, endPos, _d->direction );
      std::swap( startPos, endPos );
    break;

    case southEast:
      _computePictures( city, startPos, endPos, _d->direction );
    break;

    default: break;
    }
    
    TilesArray tiles = tilemap.getArea( startPos, endPos );
    int index=0;
    foreach( t, tiles )
    {
      LowBridgeSubTilePtr subtile = _d->subtiles[ index ];
      TilePos buildPos = pos + subtile->_pos * signSum;
      Tile& tile = tilemap.at( buildPos );
      subtile->setPicture( tile.getPicture() );
      subtile->_imgId = tile.getOriginalImgId();
      subtile->_info = TileHelper::encode( tile );
      subtile->_parent = this;
      
      events::GameEventPtr event = events::BuildEvent::create( buildPos, subtile.object() );
      event->dispatch();
      index++;
    }    
  }
}

bool LowBridge::canDestroy() const
{
  CityHelper helper( _getCity() );
  foreach( subtile, _d->subtiles )
  {
    WalkerList walkers = helper.find<Walker>( walker::any, (*subtile)->getTilePos() );
    if( !walkers.empty() )
    {
      _d->error = "##cant_demolish_bridge_with_people##";
      return false;
    }
  }

  return true;
}

void LowBridge::destroy()
{ 
  for( LowBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); it++ )
  {
    (*it)->_parent = 0;
    events::GameEventPtr event = events::ClearLandEvent::create( (*it)->_pos );
    event->dispatch();

    std::string picName = TileHelper::convId2PicName( (*it)->_imgId );

    Tile& mapTile = _getCity()->getTilemap().at( (*it)->_pos );
    mapTile.setPicture( &Picture::load( picName ) );
    TileHelper::decode( mapTile, (*it)->_info );
  }
}

std::string LowBridge::getError() const  { return _d->error; }
bool LowBridge::isNeedRoadAccess() const { return false; }

void LowBridge::save(VariantMap& stream) const
{
  Construction::save( stream );

  VariantList vl_tinfo;
  foreach( subtile,  _d->subtiles )
  {
    vl_tinfo.push_back( (*subtile)->_imgId );
  }
  stream[ "terraininfo" ] = vl_tinfo;
  //stream[ "direction" ] = (int)_d->direction;
}

void LowBridge::load(const VariantMap& stream)
{
  Construction::load( stream );

  VariantList vl_tinfo = stream.get( "terraininfo" ).toList();  
  for( unsigned int i=0; i < vl_tinfo.size(); i++ )
  {
    _d->subtiles[ i ]->_imgId = vl_tinfo.get( i ).toInt();
  }
}

