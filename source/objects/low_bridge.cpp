
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
#include "city/statistic.hpp"
#include "core/variant_map.hpp"
#include "core/variant_list.hpp"
#include "gfx/tilemap.hpp"
#include "events/build.hpp"
#include "events/clearland.hpp"
#include "constants.hpp"
#include "walker/walker.hpp"
#include "objects_factory.hpp"
#include "metadata.hpp"

using namespace gfx;
using namespace events;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::low_bridge, LowBridge)
PREDEFINE_CLASS_SMARTLIST(LowBridgeSubTile,List)

namespace {
  Point spanswOffset = Point( 10, -25 );
}

class LowBridge::Impl
{
public:
  LowBridgeSubTileList subtiles;
  Direction direction;
  std::string error;

  void addSpan( const TilePos& pos, int index )
  {
    LowBridgeSubTilePtr ret( new LowBridgeSubTile( pos, index ) );
    ret->drop();

    subtiles.push_back( ret );
  }
};

bool LowBridge::canBuild( const city::AreaInfo& areaInfo ) const
{
  TilePos endPos, startPos;
  _d->direction = direction::none;

  OverlayPtr bridge = areaInfo.city->getOverlay( areaInfo.pos );
  if( bridge.isNull() )
  {
    _d->subtiles.clear();
    LowBridge* thisp = const_cast< LowBridge* >( this );
    thisp->_fgPictures().clear();

    _checkParams( areaInfo.city, _d->direction, startPos, endPos, areaInfo.pos );

    if( _d->direction != direction::none)
    {
      thisp->_computePictures( areaInfo.city, startPos, endPos, _d->direction );
    }
  }

  return (_d->direction != direction::none);
}

LowBridge::LowBridge()
  : Bridge( object::low_bridge ), _d( new Impl )
{
  Picture pic;
  setPicture( pic );
}

void LowBridge::initTerrain(Tile& terrain )
{
}

void LowBridge::_computePictures(PlayerCityPtr city, const TilePos& startPos, const TilePos& endPos, Direction rdirection )
{
  Tilemap& tilemap = city->tilemap();
  //Picture& water = Picture::load( "land1a", 120 );
  switch( rdirection )
  {
  case direction::northWest:
    {
      Bridge::Area area( tilemap, endPos, startPos );

      if (area.size() < 3)
        break;

      area.cropCorners();

      _d->addSpan( area.front()->pos() - startPos - TilePos( 1, 0 ), LowBridgeSubTile::liftingWest );
      for( auto tile : area )
        _d->addSpan( tile->pos() - startPos, LowBridgeSubTile::spanWest );

      _d->addSpan( area.back()->pos() - startPos + TilePos( 1, 0 ), LowBridgeSubTile::descentWest );
    }
  break;

  case direction::northEast:
    {
      Bridge::Area area( tilemap, startPos, endPos );

      if (area.size() < 3)
        break;

      area.cropCorners();

      _d->addSpan( area.back()->pos() - startPos + TilePos( 0, 1 ), LowBridgeSubTile::liftingNorth );
      for( auto it=area.rbegin(); it != area.rend(); ++it )
      {
        _d->addSpan( (*it)->pos() - startPos, LowBridgeSubTile::spanNorth );
      }
      _d->addSpan( area.front()->pos() - startPos - TilePos( 0, 1 ), LowBridgeSubTile::descentNorth );
    }
    break;

  case direction::southEast:
    {
      Bridge::Area area( tilemap, startPos, endPos );

      if( area.size() < 3 )
          break;

      area.cropCorners();

      _d->addSpan( area.front()->pos() - startPos - TilePos( 1, 0 ), LowBridgeSubTile::liftingWest );
      for( auto tile : area )
        _d->addSpan( tile->pos() - startPos, LowBridgeSubTile::spanWest );

      _d->addSpan( area.back()->pos() - startPos + TilePos( 1, 0 ), LowBridgeSubTile::descentWest );
    }
  break;

  case direction::southWest:
    {
      Bridge::Area area( tilemap, endPos, startPos );

      if( area.size() < 3 )
        break;

      area.cropCorners();

      _d->addSpan( area.back()->pos() - startPos + TilePos( 0, 1 ), LowBridgeSubTile::liftingNorth );
      for( auto it=area.rbegin(); it != area.rend(); ++it )
      {
        _d->addSpan( (*it)->pos() - startPos, LowBridgeSubTile::spanNorth );
      }
      _d->addSpan( area.front()->pos() - startPos - TilePos( 0, 1 ), LowBridgeSubTile::descentNorth );
    }
  break;

  default:
  break;
  }

  for( auto tile : _d->subtiles )
  {
    _fgPictures().push_back( tile->_rpicture );
  }
}

void LowBridge::_checkParams(PlayerCityPtr city, Direction& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const
{
  start = curPos;

  Tilemap& tilemap = city->tilemap();
  Tile& tile = tilemap.at( curPos );

  int imdId = tile.imgId();
  BridgeConfig& config = BridgeConfig::find( type() );

  if( config.isNorthA( imdId ) )
  {
    TilesArea tiles( tilemap, curPos - TilePos( 10, 0), curPos - TilePos(1, 0) );
    for( auto it=tiles.rbegin(); it != tiles.rend(); ++it )
    {
      imdId = (*it)->imgId();
      if( config.isNorthB( imdId ) )
      {
        stop = (*it)->pos();
        direction = abs(stop.i() - start.i()) > 1 ? direction::northWest : direction::none;
        break;
      }
      else if ( config.isForbiden( imdId ) || !((*it)->getFlag(Tile::tlWater) || (*it)->getFlag(Tile::tlDeepWater)))
      {
        direction = direction::none;
        break;
      }
    }
  }
  else if( config.isNorthB( imdId ) )
  {
    TilesArea tiles( tilemap, curPos + TilePos(1, 0), curPos + TilePos( 10, 0) );
    for( auto tile : tiles )
    {
      imdId = tile->imgId();
      if( config.isNorthA( imdId ) )
      {
        stop = tile->pos();
        direction = abs(stop.i() - start.i()) > 1 ? direction::southEast : direction::none;
        break;
      }
      else if ( config.isForbiden( imdId ) || !(tile->getFlag(Tile::tlWater) || tile->getFlag(Tile::tlDeepWater)))
      {
        direction = direction::none;
        break;
      }
    }
  }
  else if( config.isWestA( imdId )  )
  {
    TilesArea tiles( tilemap, curPos + TilePos(0, 1), curPos + TilePos( 0, 10) );
    for( auto tile : tiles )
    {
      imdId = tile->imgId();
      if( config.isWestB( imdId ) )
      {
        stop = tile->pos();
        direction = abs(stop.j() - start.j()) > 1 ? direction::northEast : direction::none;
        break;
      }
      else if ( config.isForbiden( imdId ) || !(tile->getFlag(Tile::tlWater) || tile->getFlag(Tile::tlDeepWater)))
      {
        direction = direction::none;
        break;
      }
    }
  }
  else if( config.isWestB( imdId ) )
  {
    TilesArea tiles( tilemap, curPos - TilePos( 0, 10 ), curPos - TilePos(0, 1) );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
    {
      imdId = (*it)->imgId();
      if( config.isWestA( imdId ) )
      {
        stop = (*it)->pos();
        direction = abs(stop.j() - start.j()) > 1 ? direction::southWest : direction::none;
        break;
      }
      else if ( config.isForbiden( imdId ) || !((*it)->getFlag(Tile::tlWater) || (*it)->getFlag(Tile::tlDeepWater)))
      {
        direction = direction::none;
        break;
      }
    }
  }
  else
  {
    direction = direction::none;
  }
}

bool LowBridge::build( const city::AreaInfo& info )
{
  TilePos endPos, startPos;
  _d->direction=direction::none;
  setSize( Size(0) );
  Construction::build( info );

  _d->subtiles.clear();
  _fgPictures().clear();

  Tilemap& tilemap = info.city->tilemap();

  _checkParams( info.city, _d->direction, startPos, endPos, info.pos );
  int signSum = 1;

  if( _d->direction != direction::none)
  {
    switch( _d->direction )
    {
    case direction::northEast:
      _computePictures( info.city, endPos, startPos, direction::southWest );
      std::swap( _d->subtiles.front()->_pos, _d->subtiles.back()->_pos );
      signSum = -1;
    break;

    case direction::northWest:
      _computePictures( info.city, endPos, startPos, direction::southEast );
      std::swap( _d->subtiles.front()->_pos, _d->subtiles.back()->_pos );
      std::swap( startPos, endPos );
      signSum = -1;
    break;

    case direction::southWest:
      _computePictures( info.city, startPos, endPos, _d->direction );
      std::swap( startPos, endPos );
    break;

    case direction::southEast:
      _computePictures( info.city, startPos, endPos, _d->direction );
    break;

    default: break;
    }

    Bridge::Area area( tilemap, startPos, endPos );
    int index=0;
    foreach( t, area )
    {
      LowBridgeSubTilePtr subtile = _d->subtiles[ index ];
      TilePos buildPos = info.pos + subtile->_pos * signSum;
      Tile& tile = tilemap.at( buildPos );
      subtile->setPicture( tile.picture() );
      subtile->_imgId = tile.imgId();
      subtile->_info = tile::encode( tile );
      subtile->_parent = this;

      events::dispatch<BuildAny>( buildPos, subtile.object() );
      index++;
    }
  }

  return true;
}

bool LowBridge::canDestroy() const
{
  for( auto subtile : _d->subtiles )
  {
    WalkerList walkers = _city()->statistic().walkers.find<Walker>( walker::any, pos() + subtile->pos() );
    if( !walkers.empty() )
    {
      _d->error = "##cant_demolish_bridge_with_people##";
      return false;
    }
  }

  bool mayDestroy = state( pr::destroyable );
  if( !mayDestroy )
  {
    _d->error = "##really_destroy_bridge_qst##";
  }

  return mayDestroy;
}

void LowBridge::destroy()
{
  for( auto tile : _d->subtiles )
  {
    tile->_parent = 0;
    events::dispatch<ClearTile>( tile->_pos );

    Tile& mapTile = _map().at( tile->_pos );
    tile::decode( mapTile, tile->_info );
  }
}

std::string LowBridge::errorDesc() const  { return _d->error; }
bool LowBridge::isNeedRoad() const { return false; }

void LowBridge::save(VariantMap& stream) const
{
  Construction::save( stream );

  VariantList vl_tinfo;
  for( auto& subtile : _d->subtiles )
  {
    vl_tinfo.push_back( subtile->_imgId );
  }
  stream[ "terraininfo" ] = vl_tinfo;
}

void LowBridge::load(const VariantMap& stream)
{
  Construction::load( stream );

  VariantList vl_tinfo = stream.get( "terraininfo" ).toList();
  int lenth = math::min( vl_tinfo.size(), _d->subtiles.size() );
  for( int i=0; i < lenth; i++ )
  {    
    _d->subtiles[ i ]->_imgId = vl_tinfo.get( i ).toInt();
  }
}

void LowBridge::hide()
{
  setState( pr::destroyable, 1);
  for( auto tile : _d->subtiles )
    tile->hide();
}

LowBridgeSubTile::LowBridgeSubTile(const TilePos &pos, int index)
  : Bridge( object::low_bridge )
{
  _info = 0;
  _imgId = 0;
  _pos = pos;
  _index = index;
  _parent = 0;
  _rpicture.load( ResourceGroup::transport, index );
  _rpicture.addOffset( _pos.toScreenCoordinates() );
}

LowBridgeSubTile::~LowBridgeSubTile() {}

std::string LowBridgeSubTile::errorDesc() const { return _parent ? _parent->errorDesc() : "";  }

bool LowBridgeSubTile::isWalkable() const { return true;  }

bool LowBridgeSubTile::isNeedRoad() const { return false; }

bool LowBridgeSubTile::build(const city::AreaInfo &areainfo)
{
  Construction::build( areainfo );
  _fgPictures().clear();
  _pos = areainfo.pos;
  Point sbOffset = info().getOption( "subtileOffset" );
  _rpicture.load( ResourceGroup::transport, _index );
  _rpicture.addOffset( sbOffset );
  _fgPictures().push_back( _rpicture );

  return true;
}

void LowBridgeSubTile::setState(Param name, double value)
{
  if( _parent && name == pr::destroyable && value )
  {
    _parent->hide();
  }
}

void LowBridgeSubTile::hide()
{
  _rpicture = Picture::getInvalid();
  _fgPictures().clear();
}

void LowBridgeSubTile::initTerrain(Tile &terrain)
{
  terrain.setFlag( Tile::tlRoad, true );
}

bool LowBridgeSubTile::canDestroy() const
{
  return _parent ? _parent->canDestroy() : true;
}

void LowBridgeSubTile::destroy()
{
  if( _parent )
  {
    _parent->deleteLater();
    _parent = 0;
  }
}

void LowBridgeSubTile::save(VariantMap &stream) const
{
  if( pos() == _parent->pos() )
  {
    return _parent->save( stream );
  }
}

Point LowBridgeSubTile::offset(const Tile &, const Point &subpos) const
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
