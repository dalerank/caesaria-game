
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
#include "gfx/helper.hpp"
#include "city/helper.hpp"
#include "core/variant_map.hpp"
#include "gfx/tilemap.hpp"
#include "events/build.hpp"
#include "events/clearland.hpp"
#include "constants.hpp"
#include "walker/walker.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::low_bridge, LowBridge)

namespace {
  Point spanswOffset = Point( 10, -25 );
}

class LowBridgeSubTile : public Construction
{
public:
  enum { liftingWest=67, spanWest=68, descentWest=69, liftingNorth=70, spanNorth=71, descentNorth=72 };
  LowBridgeSubTile( const TilePos& pos, int index )
    : Construction( objects::low_bridge, Size( 1 ) )
  {
    _info = 0;
    _imgId = 0;
    _pos = pos;
    _index = index;
    _parent = 0;
    _picture = Picture::load( ResourceGroup::transport, index );
    _picture.addOffset( tile::tilepos2screen( _pos ) );
  }

  virtual ~LowBridgeSubTile() {}

  std::string errorDesc() const { return _parent ? _parent->errorDesc() : "";  }
  bool isWalkable() const { return true;  }
  bool isNeedRoadAccess() const { return false; }

  bool build( const CityAreaInfo& info )
  {
    Construction::build( info );
    _fgPicturesRef().clear();
    _pos = info.pos;
    _picture = Picture::load( ResourceGroup::transport, _index );
    _picture.addOffset( Point( 10, -12 ) );
    _fgPicturesRef().push_back( _picture );

    return true;
  }

  virtual void setState(ParameterType name, double value)
  {
    if( _parent && name == Construction::destroyable && value )
    {
      _parent->hide();
    }
  }

  void hide()
  {
    _picture = Picture::getInvalid();
    _fgPicturesRef().clear();
  }

  void initTerrain( Tile& terrain )
  {
    terrain.setFlag( Tile::tlRoad, true );
  }

  bool canDestroy() const
  {
    return _parent ? _parent->canDestroy() : true;
  }

  void destroy()
  {
    if( _parent )
    {
      _parent->deleteLater();
      _parent = 0;
    }
  }

  void save(VariantMap &stream) const
  {
    if( pos() == _parent->pos() )
    {
      return _parent->save( stream );
    }
  }

  Point offset( const Tile& , const Point& subpos ) const
  {
    switch( _index )
    {
    case liftingWest: return Point( -subpos.x(), subpos.x()*2 );
    case spanWest:    return Point( 0, -30 );
    case descentWest: return Point( subpos.x(), 12 - subpos.x() );
    case descentNorth: return Point( -subpos.y()*0.5, subpos.y()*1.3 );
    case spanNorth:    return spanswOffset;
    case liftingNorth: return Point( subpos.y()*0.6, -30-subpos.y() );

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

bool LowBridge::canBuild( const CityAreaInfo& areaInfo ) const
{
  //bool is_constructible = Construction::canBuild( pos );

  TilePos endPos, startPos;
  _d->direction=noneDirection;

  TileOverlayPtr bridge = areaInfo.city->getOverlay( areaInfo.pos );
  if( bridge.isNull() )
  {
    _d->subtiles.clear();
    LowBridge* thisp = const_cast< LowBridge* >( this );
    thisp->_fgPicturesRef().clear();

    _checkParams( areaInfo.city, _d->direction, startPos, endPos, areaInfo.pos );

    if( _d->direction != noneDirection )
    {
      thisp->_computePictures( areaInfo.city, startPos, endPos, _d->direction );
    }
  }

  return (_d->direction != noneDirection);
}

LowBridge::LowBridge() : Construction( constants::objects::low_bridge, Size(1) ), _d( new Impl )
{
  Picture pic;
  setPicture( pic );
}

void LowBridge::initTerrain(Tile& terrain )
{
}

void LowBridge::_computePictures(PlayerCityPtr city, const TilePos& startPos, const TilePos& endPos, constants::Direction dir )
{
  Tilemap& tilemap = city->tilemap();
  //Picture& water = Picture::load( "land1a", 120 );
  switch( dir )
  {
  case northWest:
    {
      TilesArray tiles = tilemap.getArea( endPos, startPos );

      if (tiles.size() < 3) break;

      tiles.pop_back();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.front()->pos() - startPos - TilePos( 1, 0 ), LowBridgeSubTile::liftingWest );
      foreach( it, tiles )
      {
        _d->addSpan( (*it)->pos() - startPos, LowBridgeSubTile::spanWest );
      }
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 1, 0 ), LowBridgeSubTile::descentWest );
    }
  break;

  case northEast:
    {
      TilesArray tiles = tilemap.getArea( startPos, endPos );

      if (tiles.size() < 3) break;

      tiles.pop_back();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 1 ), LowBridgeSubTile::liftingNorth );
      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
      {
        _d->addSpan( (*it)->pos() - startPos, LowBridgeSubTile::spanNorth );
      }
      _d->addSpan( tiles.front()->pos() - startPos - TilePos( 0, 1 ), LowBridgeSubTile::descentNorth );
    }
    break;

  case southEast:
    {
      TilesArray tiles = tilemap.getArea( startPos, endPos );

      if( tiles.size() < 3 )
          break;

      tiles.pop_back();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.front()->pos() - startPos - TilePos( 1, 0 ), LowBridgeSubTile::liftingWest );
      foreach( it, tiles )
      {
        _d->addSpan( (*it)->pos() - startPos, LowBridgeSubTile::spanWest );
        //_d->subtiles.push_back( LowBridgeSubTile( (*it)->getIJ() - startPos, water ) );
      }
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 1, 0 ), LowBridgeSubTile::descentWest );
    }
  break;

  case southWest:
    {
      TilesArray tiles = tilemap.getArea( endPos, startPos );

      if( tiles.size() < 3 )
        break;

      tiles.pop_back();
      tiles.erase( tiles.begin() );

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 1 ), LowBridgeSubTile::liftingNorth );
      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
      {
        _d->addSpan( (*it)->pos() - startPos, LowBridgeSubTile::spanNorth );
        //_d->subtiles.push_back( LowBridgeSubTile( (*it)->getIJ() - startPos, water ) );
      }
      _d->addSpan( tiles.front()->pos() - startPos - TilePos( 0, 1 ), LowBridgeSubTile::descentNorth );
    }
  break;

  default:
  break;
  }

  for( LowBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); ++it )
  {
    _fgPicturesRef().push_back( (*it)->_picture );
  }
}

void LowBridge::_checkParams(PlayerCityPtr city, constants::Direction& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const
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
    TilesArray tiles = tilemap.getArea( curPos - TilePos( 10, 0), curPos - TilePos(1, 0) );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
    {
      imdId = (*it)->originalImgId();
      if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379 )
      {
        stop = (*it)->pos();
        direction = abs(stop.i() - start.i()) > 1 ? northWest : noneDirection;
        break;
      }
      else if ((imdId > 372 && imdId < 445) || !((*it)->getFlag(Tile::tlWater) || (*it)->getFlag(Tile::tlDeepWater)))
      {
        direction = noneDirection;
        break;
      }
    }
  }
  else if( imdId == 376 || imdId == 377 || imdId == 378 || imdId == 379  )
  {
    TilesArray tiles = tilemap.getArea( curPos + TilePos(1, 0), curPos + TilePos( 10, 0) );
    for( TilesArray::iterator it=tiles.begin(); it != tiles.end(); ++it )
    {
      imdId = (*it)->originalImgId();
      if( imdId == 384 || imdId == 385 || imdId == 386 || imdId == 387 )
      {
        stop = (*it)->pos();
        direction = abs(stop.i() - start.i()) > 1 ? southEast : noneDirection;
        break;
      }
      else if ((imdId > 372 && imdId < 445) || !((*it)->getFlag(Tile::tlWater) || (*it)->getFlag(Tile::tlDeepWater)))
      {
        direction = noneDirection;
        break;
      }
    }
  }
  else if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375  )
  {
    TilesArray tiles = tilemap.getArea( curPos + TilePos(1, 0), curPos + TilePos( 0, 10) );
    for( TilesArray::iterator it=tiles.begin(); it != tiles.end(); ++it )
    {
      imdId = (*it)->originalImgId();
      if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
      {
        stop = (*it)->pos();
        direction = abs(stop.j() - start.j()) > 1 ? northEast : noneDirection;
        break;
      }
      else if ((imdId > 372 && imdId < 445) || !((*it)->getFlag(Tile::tlWater) || (*it)->getFlag(Tile::tlDeepWater)))
      {
        direction = noneDirection;
        break;
      }
    }
  }
  else if( imdId == 380 || imdId == 381 || imdId == 382 || imdId == 383 )
  {
    TilesArray tiles = tilemap.getArea( curPos - TilePos( 0, 10), curPos - TilePos(0, 1) );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
    {
      imdId = (*it)->originalImgId();
      if( imdId == 372 || imdId == 373 || imdId == 374 || imdId == 375 )
      {
        stop = (*it)->pos();
        direction = abs(stop.j() - start.j()) > 1 ? southWest : noneDirection;
        break;
      }
      else if ((imdId > 372 && imdId < 445) || !((*it)->getFlag(Tile::tlWater) || (*it)->getFlag(Tile::tlDeepWater)))
      {
        direction = noneDirection;
        break;
      }
    }
  }
  else
  {
    direction = noneDirection;
  }
}

bool LowBridge::build( const CityAreaInfo& info )
{
  TilePos endPos, startPos;
  _d->direction=noneDirection;
  setSize( Size(0) );
  Construction::build( info );


  _d->subtiles.clear();
  _fgPicturesRef().clear();

  Tilemap& tilemap = info.city->tilemap();

  _checkParams( info.city, _d->direction, startPos, endPos, info.pos );
  int signSum = 1;

  if( _d->direction != noneDirection )
  {
    switch( _d->direction )
    {
    case northEast:
      _computePictures( info.city, endPos, startPos, southWest );
      std::swap( _d->subtiles.front()->_pos, _d->subtiles.back()->_pos );
      signSum = -1;
    break;

    case northWest:
      _computePictures( info.city, endPos, startPos, southEast );
      std::swap( _d->subtiles.front()->_pos, _d->subtiles.back()->_pos );
      std::swap( startPos, endPos );
      signSum = -1;
    break;

    case southWest:
      _computePictures( info.city, startPos, endPos, _d->direction );
      std::swap( startPos, endPos );
    break;

    case southEast:
      _computePictures( info.city, startPos, endPos, _d->direction );
    break;

    default: break;
    }

    TilesArray tiles = tilemap.getArea( startPos, endPos );
    int index=0;
    foreach( t, tiles )
    {
      LowBridgeSubTilePtr subtile = _d->subtiles[ index ];
      TilePos buildPos = info.pos + subtile->_pos * signSum;
      Tile& tile = tilemap.at( buildPos );
      subtile->setPicture( tile.picture() );
      subtile->_imgId = tile.originalImgId();
      subtile->_info = tile::encode( tile );
      subtile->_parent = this;

      events::GameEventPtr event = events::BuildAny::create( buildPos, subtile.object() );
      event->dispatch();
      index++;
    }
  }

  return true;
}

bool LowBridge::canDestroy() const
{
  city::Helper helper( _city() );
  foreach( subtile, _d->subtiles )
  {
    WalkerList walkers = helper.find<Walker>( walker::any, pos() + (*subtile)->pos() );
    if( !walkers.empty() )
    {
      _d->error = "##cant_demolish_bridge_with_people##";
      return false;
    }
  }

  bool mayDestroy = state( Construction::destroyable );
  if( !mayDestroy )
  {
    _d->error = "##really_destroy_bridge_qst##";
  }

  return mayDestroy;
}

void LowBridge::destroy()
{
  foreach( it, _d->subtiles )
  {
    (*it)->_parent = 0;
    events::GameEventPtr event = events::ClearTile::create( (*it)->_pos );
    event->dispatch();

    //std::string picName = TileHelper::convId2PicName( (*it)->_imgId );

    Tile& mapTile = _city()->tilemap().at( (*it)->_pos );
    tile::decode( mapTile, (*it)->_info );
  }
}

std::string LowBridge::errorDesc() const  { return _d->error; }
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

void LowBridge::hide()
{
  setState( Construction::destroyable, 1);
  foreach( it, _d->subtiles )
  {
    (*it)->hide();
  }
}

