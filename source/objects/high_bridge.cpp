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
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "events/build.hpp"
#include "constants.hpp"
#include "walker/walker.hpp"
#include <vector>

using namespace constants;
using namespace gfx;

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
    _picture.addOffset( Point( 30*(_pos.i()+_pos.j()), 15*(_pos.j()-_pos.i()) ) );
    checkSecondPart();
  }

  void checkSecondPart()
  {
    switch( _index )
    {
    case liftingSW2: _picture.addOffset( -29, -16 ); break;
    case descentSW2: _picture.addOffset( -29, -16 ); break;
    
    case liftingSE2:  _picture.addOffset( -28, 1 ); break;
    case liftingSE: _picture.addOffset( 3, -14 ); break;
    case spanSE: _picture.addOffset( 8, -13 ); break;
    case descentSE2: _picture.addOffset( -22, 2 );  break;
    case descentSE: _picture.addOffset( 8, -13 ); break;
    }
  }

  bool canDestroy() const
  {
    return _parent->canDestroy();
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
    bool isWater = terrain.getFlag( Tile::tlWater );
    bool isDeepWater = terrain.getFlag( Tile::tlDeepWater );

    terrain.setFlag( Tile::clearAll, true );
    terrain.setFlag( Tile::tlWater, isWater );
    terrain.setFlag( Tile::tlDeepWater, isDeepWater );
    terrain.setFlag( Tile::tlRoad, true );
  }

  void destroy()
  {
    if( _parent )
    {
      _parent->deleteLater();
    }
  }

  void save(VariantMap &stream) const
  {
    if( pos() == _parent->pos() )
    {
      return _parent->save( stream );
    }
  }

  Point offset( const Tile& tile, const Point& subpos ) const
  {
    switch( _index )
    {
    case liftingSE: return Point( 0, subpos.x() );
    case spanSE:    return Point( 0, 10 );
    case footingSE: return Point( 0, 10 );
    case descentSE: return Point( 0, 10 - subpos.x() );
    case descentSW: return Point( -subpos.y(), 0 );
    case spanSW:    return Point( -10, 0 );
    case footingSW: return Point( -10, 0 );
    case liftingSW: return Point( -(10 - subpos.y()), 0 );

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
  std::string error;

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
  
  TileOverlayPtr ov = city->getOverlay( pos );
  if( ov.isNull() )
  {
    _d->subtiles.clear();
    const_cast< HighBridge* >( this )->_fgPicturesRef().clear();

    _checkParams( city, _d->direction, startPos, endPos, pos );

    if( _d->direction != noneDirection )
    {
      const_cast< HighBridge* >( this )->_computePictures( city, startPos, endPos, _d->direction );
    }
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
  Tilemap& tilemap = city->tilemap();
  //Picture& water = Picture::load( "land1a", 120 );
  switch( dir )
  {
  case constants::northWest:
    {
      TilesArray tiles = tilemap.getArea( endPos, startPos );

      tiles.pop_back();
      tiles.pop_back();
      
      tiles.erase( tiles.begin() );
      _d->addSpan( tiles.front()->pos() - startPos - TilePos( 1, 0 ), HighBridgeSubTile::liftingSE );
      _d->addSpan( tiles.front()->pos() - startPos, HighBridgeSubTile::liftingSE2 );
      tiles.erase( tiles.begin() );

      foreach( tile, tiles )
      {
        _d->addSpan( (*tile)->pos() - startPos, HighBridgeSubTile::spanSE );
      }

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 1, 0 ), HighBridgeSubTile::descentSE );
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 2, 0 ), HighBridgeSubTile::descentSE2 );
    }
  break;

  case northEast:
    {
      TilesArray tiles = tilemap.getArea( startPos, endPos );

      tiles.pop_back();
      tiles.pop_back();
      tiles.erase( tiles.begin() );
      TilePos liftPos = tiles.front()->pos();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 1 ), HighBridgeSubTile::liftingSW );
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 2 ), HighBridgeSubTile::liftingSW2 );

      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
      {
        _d->addSpan( (*it)->pos() - startPos, HighBridgeSubTile::spanSW );
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

      tiles.erase( tiles.begin() );
      _d->addSpan( tiles.front()->pos() - startPos - TilePos( 1, 0 ), HighBridgeSubTile::liftingSE );
      _d->addSpan( tiles.front()->pos() - startPos, HighBridgeSubTile::liftingSE2 );
      tiles.erase( tiles.begin() );

      foreach( tile, tiles )
      {        
        _d->addSpan( (*tile)->pos() - startPos, HighBridgeSubTile::spanSE );
      }

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 1, 0 ), HighBridgeSubTile::descentSE );
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 2, 0 ), HighBridgeSubTile::descentSE2 );
    }
  break;

  case constants::southWest:
    {
      TilesArray tiles = tilemap.getArea( endPos, startPos );
      
      tiles.pop_back();
      tiles.pop_back();
      
      tiles.erase( tiles.begin() );
      TilePos liftPos = tiles.front()->pos();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 1 ), HighBridgeSubTile::liftingSW );
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 2 ), HighBridgeSubTile::liftingSW2 );
      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
      {        
        _d->addSpan( (*it)->pos() - startPos, HighBridgeSubTile::spanSW );
      }
      _d->addSpan( liftPos - startPos, HighBridgeSubTile::descentSW2 );
      _d->addSpan( liftPos - startPos - TilePos( 0, 1 ), HighBridgeSubTile::descentSW );
    }
  break;

  default:
  break;
  }

  foreach( tile, _d->subtiles ) { _fgPicturesRef().push_back( (*tile)->_picture ); }
}

void HighBridge::_checkParams(PlayerCityPtr city, Direction& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const
{
  start = curPos;

  Tilemap& tilemap = city->tilemap();
  Tile& tile = tilemap.at( curPos );

  /*if( tile.getFlag( Tile::tlRoad ) )
  {
    direction = constants::noneDirection;
    return;
  }*/

  int imdId = tile.originalImgId();
  if( imdId == 384 || imdId == 385 || imdId == 386 || imdId == 387 )
  {    
    TilesArray tiles = tilemap.getArea( curPos - TilePos( 10, 0), curPos );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
    {
      imdId = (*it)->originalImgId();
      if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379 )
      {
        stop = (*it)->pos();
        direction = abs( stop.i() - start.i() ) > 3 ? northWest : noneDirection;
        break;
      }
    }
  }
  else if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379  )
  {
    TilesArray tiles = tilemap.getArea( curPos, curPos + TilePos( 10, 0) );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
    {
      imdId = (*it)->originalImgId();
      if( imdId == 384 || imdId == 385 || imdId == 386 || imdId == 387 )
      {
        stop = (*it)->pos();
        direction = abs( stop.i() - start.i() ) > 3 ? southEast : noneDirection;
        break;
      }
    }
  }
  else if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375  )
  {
    TilesArray tiles = tilemap.getArea( curPos, curPos + TilePos( 0, 10) );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
    {
      imdId = (*it)->originalImgId();
      if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
      {
        stop = (*it)->pos();
        direction = abs( stop.j() - start.j() ) > 3 ? northEast : noneDirection;
        break;
      }
    }
  }
  else if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
  {
    TilesArray tiles = tilemap.getArea( curPos - TilePos( 0, 10), curPos );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
    {
      imdId = (*it)->originalImgId();
      if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375 )
      {
        stop = (*it)->pos();
        direction = abs( stop.j() - start.j() ) > 3 ? southWest : noneDirection;
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

  setSize( Size(0) );
  Construction::build( city, pos );

  _d->subtiles.clear();
  _fgPicturesRef().clear();

  Tilemap& tilemap = city->tilemap();

  _checkParams( city, _d->direction, startPos, endPos, pos );

  if( _d->direction != noneDirection )
  {    
    _computePictures( city, startPos, endPos, _d->direction );
   
    foreach( it, _d->subtiles )
    {
      HighBridgeSubTilePtr subtile = *it;
      TilePos buildPos = pos + subtile->_pos;
      Tile& tile = tilemap.at( buildPos );
      subtile->setPicture( tile.picture() );
      subtile->_imgId = tile.originalImgId();
      subtile->_info = TileHelper::encode( tile );
      subtile->_parent = this;
      
      events::GameEventPtr event = events::BuildEvent::create( buildPos, subtile.object() );
      event->dispatch();
    }    
  }
}

bool HighBridge::canDestroy() const
{
  city::Helper helper( _city() );
  foreach( subtile, _d->subtiles )
  {
    WalkerList walkers = helper.find<Walker>( walker::any, (*subtile)->pos() );
    if( !walkers.empty() )
    {
      _d->error = "##cant_demolish_bridge_with_people##";
      return false;
    }
  }

  return true;
}

void HighBridge::destroy()
{ 
  PlayerCityPtr city = _city();
  foreach( it,  _d->subtiles )
  {
    HighBridgeSubTilePtr subtile = *it;
    subtile->_parent = 0;
    events::GameEventPtr event = events::ClearLandEvent::create( subtile->_pos );
    event->dispatch();

    std::string picName = TileHelper::convId2PicName( subtile->_imgId );

    Tile& mapTile = city->tilemap().at( subtile->_pos );
    mapTile.setPicture( &Picture::load( picName ) );

    TileHelper::decode( mapTile, subtile->_info );
  }
}

std::string HighBridge::getError() const {  return _d->error;}
bool HighBridge::isNeedRoadAccess() const{  return false;}

void HighBridge::save(VariantMap& stream) const
{
  Construction::save( stream );

  VariantList vl_tinfo;
  foreach( subtile,  _d->subtiles )
  {
    vl_tinfo.push_back( (*subtile)->_imgId );
  }

  stream[ "terraininfo" ] = vl_tinfo;
}

void HighBridge::load(const VariantMap& stream)
{
  Construction::load( stream );

  VariantList vl_tinfo = stream.get( "terraininfo" ).toList();
  for( unsigned int i=0; i < vl_tinfo.size(); i++ )
  {
    _d->subtiles[ i ]->_imgId = vl_tinfo.get( i ).toInt();
  }
}
