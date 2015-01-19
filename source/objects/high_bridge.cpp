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
#include "gfx/helper.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "events/build.hpp"
#include "core/variant_map.hpp"
#include "constants.hpp"
#include "walker/walker.hpp"
#include "events/clearland.hpp"
#include <core/logger.hpp>
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::high_bridge, HighBridge)

namespace {
  const Point spanswOffset = Point( 12, -43 );
}

class HighBridgeSubTile : public Construction
{
public:
   enum { liftingWest=73, spanWest=74, footingWest=79, descentWest=75,
          liftingNorth=76, spanNorth=77, footingNorth=80, descentNorth=78,
          liftingWestL=173, descentWestL=175,
          liftingNorthL=176, descentNorthL=178 };
  HighBridgeSubTile( const TilePos& pos, int index )
    : Construction( objects::high_bridge, Size( 1 ) )
  {
    _pos = pos;
    _index = index;

    Picture pic = Picture::load( ResourceGroup::transport, _index );
    pic.addOffset( tile::tilepos2screen( _pos ) );
    setPicture( pic );

    //checkSecondPart();
  }

  bool canDestroy() const
  {
    return _parent ? _parent->canDestroy() : true;
  }

  virtual ~HighBridgeSubTile()   {}
  bool isWalkable() const   {    return true;  }
  bool isNeedRoadAccess() const { return false; }

  bool build( const CityAreaInfo& info )
  {
    if( _index == descentNorth || _index == liftingNorth )
    {
      setSize( Size( 1, 2 ) );
    }
    else if( _index == descentWest || _index == liftingWest )
    {
      setSize( Size( 2, 1 ) );
    }
    else if( _index == descentNorthL || _index == liftingNorthL ||
             _index == descentWestL || _index == liftingWestL )
    {
      return false;
    }

    Construction::build( info );

    setPicture( Picture::getInvalid() );
    _fgPicturesRef().clear();
    Picture pic = Picture::load( ResourceGroup::transport, _index);

    const TilePos& pos = info.pos;
    if( _index == descentNorth )
    {
      Tile& mt = info.city->tilemap().at( pos + TilePos( 0, 1 ) );
      info.city->tilemap().at( pos + TilePos( 0, 1 ) ).setMasterTile( 0 );
      info.city->tilemap().at( pos ).setMasterTile( &mt );

      pic.addOffset( -30, -15 );
      _fgPicturesRef().push_back( pic );
    }
    else if( _index == liftingNorth )
    {
      Tile& mt = info.city->tilemap().at( pos + TilePos( 0, 1 ) );
      Picture landPic = mt.picture();
      landPic.addOffset( tile::tilepos2screen( TilePos( 0, 1 ) ) );
      _fgPicturesRef().push_back( landPic );

      _fgPicturesRef().push_back( pic );
    }
    else if( _index == descentWest )
    {
      Tile& mt = info.city->tilemap().at( pos + TilePos( 1, 0) );
      Picture landPic = mt.picture();
      landPic.addOffset( tile::tilepos2screen( TilePos( 1, 0 ) )  );
      _fgPicturesRef().push_back( landPic );

     pic.addOffset( 8, -14 );
      _fgPicturesRef().push_back( pic );
    }
    else if(  _index == liftingWest )
    {
      Tile& mt = info.city->tilemap().at( info.pos + TilePos( 1, 0) );
      Picture landPic = mt.picture();
      landPic.addOffset( tile::tilepos2screen( TilePos( 1, 0 ) ) );
      _fgPicturesRef().push_back( landPic );

      pic.addOffset( 0, -15 );
      _fgPicturesRef().push_back( pic );
    }
    else if( _index == footingWest || _index == spanWest )
    {
      pic.addOffset( 7, -14 );
      setPicture( pic );
    }
    else
    {
      setPicture( pic );
    }

    _pos = pos;

    return true;
  }

  void hide()
  {
    setPicture( Picture::getInvalid() );
    //_fgPicturesRef().clear();
  }

  void setState( ParameterType name, double value )
  {
    if( _parent && name == Construction::destroyable && value )
    {
      _parent->hide();
    }
  }

  void initTerrain( Tile& terrain )
  {
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
      if( _parent )
        _parent->save( stream );
    }
  }

  Point offset( const Tile& tile, const Point& subpos ) const
  {
    switch( _index )
    {
    case liftingWest: return Point( 0, subpos.x() );
    case spanWest:    return Point( 0, 10 );
    case footingWest: return Point( 0, 10 );
    case descentWest: return Point( 0, 10 - subpos.x() );
    case descentWestL: return Point( 0, -30 - subpos.y() );

    case descentNorth:
    {
      const Tile* t = const_cast<HighBridgeSubTile*>( this )->_masterTile();
      return &tile == t
                ? Point( 0, -15 + subpos.y() * 2 )
                : Point( 0, -30 + subpos.y() * 2 );
    }
    case spanNorth:    return spanswOffset;
    case footingNorth: return Point( -10, 0 );
    case liftingNorth:
    {
      const Tile* t = const_cast<HighBridgeSubTile*>( this )->_masterTile();
      return &tile == t
          ? Point(  0, -30 - subpos.y() * 1.5 )
          : Point( 0, -18 - subpos.y() * 1.1 );
    }

    default: return Point( 0, 0 );
    }
  }

  TilePos _pos;
  int _index;
  int _info;
  int _imgId;
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

bool HighBridge::canBuild( const CityAreaInfo& areaInfo ) const
{
  //bool is_constructible = Construction::canBuild( pos );

  TilePos endPos, startPos;
  _d->direction=noneDirection;
  
  TileOverlayPtr ov = areaInfo.city->getOverlay( areaInfo.pos );
  if( ov.isNull() )
  {
    _d->subtiles.clear();
    HighBridge* thisp = const_cast< HighBridge* >( this );
    thisp->_fgPicturesRef().clear();

    _checkParams( areaInfo.city, _d->direction, startPos, endPos, areaInfo.pos );

    if( _d->direction != noneDirection )
    {
      thisp->_computePictures( areaInfo.city, startPos, endPos, _d->direction );
    }
  }

  return (_d->direction != noneDirection );
}

HighBridge::HighBridge() : Construction( objects::high_bridge, Size(1) ), _d( new Impl )
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

  switch( dir )
  {
  case constants::northWest:
    {
      TilesArray tiles = tilemap.getArea( endPos, startPos );

      tiles.pop_back();
      tiles.pop_back();
      
      tiles.erase( tiles.begin() );
      _d->addSpan( tiles.front()->pos() - startPos - TilePos( 1, 0 ), HighBridgeSubTile::liftingWest );
      _d->addSpan( tiles.front()->pos() - startPos, HighBridgeSubTile::liftingWestL );
      tiles.erase( tiles.begin() );

      foreach( tile, tiles )
      {
        _d->addSpan( (*tile)->pos() - startPos, HighBridgeSubTile::spanWest );
      }

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 1, 0 ), HighBridgeSubTile::descentWest );
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 2, 0 ), HighBridgeSubTile::descentWestL );
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

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 1 ), HighBridgeSubTile::liftingNorth );
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 2 ), HighBridgeSubTile::liftingNorthL );

      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
      {
        _d->addSpan( (*it)->pos() - startPos, HighBridgeSubTile::spanNorth );
      }

      _d->addSpan( liftPos - startPos, HighBridgeSubTile::descentNorthL );
      _d->addSpan( liftPos - startPos - TilePos( 0, 1 ), HighBridgeSubTile::descentNorth );
    }
    break;

  case southEast:
    {
      TilesArray tiles = tilemap.getArea( startPos, endPos );

      tiles.pop_back();
      tiles.pop_back();

      tiles.erase( tiles.begin() );
      _d->addSpan( tiles.front()->pos() - startPos - TilePos( 1, 0 ), HighBridgeSubTile::liftingWest );
      _d->addSpan( tiles.front()->pos() - startPos, HighBridgeSubTile::liftingWestL );
      tiles.erase( tiles.begin() );

      foreach( tile, tiles )
      {        
        _d->addSpan( (*tile)->pos() - startPos, HighBridgeSubTile::spanWest );
      }

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 1, 0 ), HighBridgeSubTile::descentWest );
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 2, 0 ), HighBridgeSubTile::descentWestL );
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

      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 1 ), HighBridgeSubTile::liftingNorth );
      _d->addSpan( tiles.back()->pos() - startPos + TilePos( 0, 2 ), HighBridgeSubTile::liftingNorthL );
      for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
      {        
        _d->addSpan( (*it)->pos() - startPos, HighBridgeSubTile::spanNorth );
      }
      _d->addSpan( liftPos - startPos, HighBridgeSubTile::descentNorthL );
      _d->addSpan( liftPos - startPos - TilePos( 0, 1 ), HighBridgeSubTile::descentNorth );
    }
  break;

  default:
  break;
  }

  foreach( tile, _d->subtiles ) { _fgPicturesRef().push_back( (*tile)->picture() ); }
}

bool HighBridge::_checkOnlyWaterUnderBridge( PlayerCityPtr city, const TilePos& start, const TilePos& stop ) const
{
  TilesArray tiles = city->tilemap().getArea( start, stop );
  if( tiles.size() > 2 )
  {
    bool onlyWater = true;
    for( unsigned int k=1; k < tiles.size()-1; k++ )
    {
      const Tile* tile = tiles[ k ];
      onlyWater &= ( !tile->getFlag(Tile::tlCoast) && (tile->getFlag(Tile::tlWater) || tile->getFlag(Tile::tlDeepWater)) );
    }

    return onlyWater;
  }

  return false;
}

static bool __isFlatCoastTile( const Tile& tile )
{
  return (tile.getFlag( Tile::tlCoast ) && !tile.getFlag( Tile::tlRubble ));
}

void HighBridge::_checkParams(PlayerCityPtr city, Direction& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const
{
  start = curPos;

  Tilemap& tilemap = city->tilemap();
  Tile& tile = tilemap.at( curPos );
  direction = noneDirection;

  if( !__isFlatCoastTile( tile ) )
    return;

  {
    TilesArray tiles = tilemap.getArea( curPos - TilePos( 10, 0), curPos - TilePos(1, 0) );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
    {

      if( __isFlatCoastTile( **it ) )
      {
        stop = (*it)->pos();
        direction = abs( stop.i() - start.i() ) > 3 ? northWest : noneDirection;
        break;
      }      
    }

    bool mayBuild = _checkOnlyWaterUnderBridge( city, start, stop );
    if( !mayBuild )
      direction = noneDirection;
  }

  if( direction == noneDirection )
  {
    TilesArray tiles = tilemap.getArea( curPos + TilePos(1, 0), curPos + TilePos( 10, 0) );
    foreach( it, tiles )
    {
      if( __isFlatCoastTile( **it ) )
      {
        stop = (*it)->pos();
        direction = abs( stop.i() - start.i() ) > 3 ? southEast : noneDirection;
        break;
      }
    }

    bool mayBuild = _checkOnlyWaterUnderBridge( city, start, stop );
    if( !mayBuild )
      direction = noneDirection;
  }

  if( direction == noneDirection )
  {
    TilesArray tiles = tilemap.getArea( curPos + TilePos(0, 1), curPos + TilePos( 0, 10) );
    for( TilesArray::iterator it=tiles.begin(); it != tiles.end(); ++it )
    {
      if( __isFlatCoastTile( **it ) )
      {
        stop = (*it)->pos();
        direction = abs( stop.j() - start.j() ) > 3 ? northEast : noneDirection;
        break;
      }
    }

    bool mayBuild = _checkOnlyWaterUnderBridge( city, start, stop );
    if( !mayBuild )
      direction = noneDirection;
  }

  if( direction == noneDirection )
  {
    TilesArray tiles = tilemap.getArea( curPos - TilePos( 0, 10), curPos - TilePos(0, 1) );
    for( TilesArray::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); ++it )
    {
      if( __isFlatCoastTile( **it ) )
      {
        stop = (*it)->pos();
        direction = abs( stop.j() - start.j() ) > 3 ? southWest : noneDirection;
        break;
      }
    }

    bool mayBuild = _checkOnlyWaterUnderBridge( city, start, stop );
    if( !mayBuild )
      direction = noneDirection;
  }
}

bool HighBridge::build( const CityAreaInfo& info  )
{
  TilePos endPos, startPos;
  _d->direction=noneDirection;

  setSize( Size(0) );
  Construction::build( info );

  _d->subtiles.clear();
  _fgPicturesRef().clear();

  Tilemap& tilemap = info.city->tilemap();

  _checkParams( info.city, _d->direction, startPos, endPos, info.pos );

  if( _d->direction != noneDirection )
  {    
    _computePictures( info.city, startPos, endPos, _d->direction );
   
    foreach( it, _d->subtiles )
    {
      HighBridgeSubTilePtr subtile = *it;
      TilePos buildPos = info.pos + subtile->_pos;
      Tile& tile = tilemap.at( buildPos );
      //subtile->setPicture( tile.picture() );
      subtile->_imgId = tile.originalImgId();
      subtile->_info = tile::encode( tile );
      subtile->_parent = this;
      
      events::GameEventPtr event = events::BuildAny::create( buildPos, subtile.object() );
      event->dispatch();
    }    
  }

  return true;
}

bool HighBridge::canDestroy() const
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

  if( !state( Construction::destroyable ) )
  {
    _d->error = "##destroy_bridge_warning##";
  }

  return state( Construction::destroyable );
}

void HighBridge::destroy()
{ 
  PlayerCityPtr city = _city();
  foreach( it, _d->subtiles )
  {
    (*it)->_parent = 0;
    (*it)->setState( Construction::destroyable, true );
    (*it)->deleteLater();
  }

  foreach( it,  _d->subtiles )
  {
    HighBridgeSubTilePtr subtile = *it;

    events::GameEventPtr event = events::ClearTile::create( subtile->_pos );
    event->dispatch();

    Tile& mapTile = city->tilemap().at( subtile->_pos );
    mapTile.setFlag( Tile::tlRoad, false );
  }

  _d->subtiles.clear();
}

std::string HighBridge::errorDesc() const {  return _d->error;}
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

void HighBridge::hide()
{
  setState( Construction::destroyable, 1);
  foreach( it, _d->subtiles )
  {
    (*it)->hide();
  }
}
