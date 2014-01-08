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

#include "dock.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "core/foreach.hpp"
#include "walker/seamerchant.hpp"
#include "core/foreach.hpp"
#include "good/goodstore.hpp"
#include "constants.hpp"

using namespace constants;

class Dock::Impl
{
public:
  enum { southPic=29, northPic=5, westPic=41, eastPic=17 };
  std::vector<int> saveTileInfo;
  Direction direction;

  bool isFlatCoast( const Tile& tile ) const
  {
    int imgId = tile.getOriginalImgId();
    return (imgId >= 372 && imgId <= 387);
  }

  Direction getDirection(PlayerCityPtr city, TilePos pos, Size size);
  bool isConstructibleArea( const TilesArray& tiles );
  bool isCoastalArea( const TilesArray& tiles );
};

Dock::Dock(): WorkingBuilding( building::dock, Size(3) ), _d( new Impl )
{
  // dock pictures
  // transport 5        animation = 6~16
  // transport 17       animation = 18~28
  // transport 29       animation = 30~40
  // transport 41       animation = 42~51
  setPicture( ResourceGroup::transport, 5);

  _fgPicturesRef().resize(1);
}

bool Dock::canBuild( PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  bool is_constructible = true;//Construction::canBuild( city, pos );

  Direction direction = _d->getDirection( city, pos, getSize() );

  const_cast< Dock* >( this )->_setDirection( direction );

  return (is_constructible && direction != noneDirection );
}

void Dock::build(PlayerCityPtr city, const TilePos& pos)
{
  _setDirection( _d->getDirection( city, pos, getSize() ) );

  TilesArray area = city->getTilemap().getArea( pos, getSize() );

  foreach( Tile* tile, area ) { _d->saveTileInfo.push_back( TileHelper::encode( *tile ) ); }

  WorkingBuilding::build( city, pos );
}

void Dock::destroy()
{
  CityHelper helper( _getCity() );

  TilesArray area = helper.getArea( this );

  int index=0;
  foreach( Tile* tile, area ) { TileHelper::decode( *tile, _d->saveTileInfo.at( index++ ) ); }

  WorkingBuilding::destroy();
}

void Dock::timeStep(const unsigned long time)
{
  _animationRef().update( time );

  // takes current animation frame and put it into foreground
  _fgPicturesRef().at(0) = _animationRef().getFrame();
}

void Dock::save(VariantMap& stream) const
{
  WorkingBuilding::save( stream );

  stream[ "direction" ] = (int)_d->direction;
  stream[ "saved_tile"] = VariantList( _d->saveTileInfo );
}

void Dock::load(const VariantMap& stream)
{
  Building::load( stream );

  _d->direction = (Direction)stream.get( "direction", (int)southWest ).toInt();
  _d->saveTileInfo << stream.get( "saved_tile" ).toList();
}

bool Dock::isBusy() const
{
  CityHelper helper( _getCity() );
  SeaMerchantList merchants = helper.find<SeaMerchant>( walker::seaMerchant );

  return !merchants.empty();
}

const Tile& Dock::getLandingTile() const
{
  Tilemap& tmap = _getCity()->getTilemap();
  TilePos offset( -999, -999 );
  switch( _d->direction )
  {
  case south: offset = TilePos( 0, -1 ); break;
  case west: offset = TilePos( -1, 0 ); break;
  case north: offset = TilePos( 0, 3 ); break;
  case east: offset = TilePos( 3, 0 ); break;

  default: break;
  }

  return tmap.at( getTilePos() + offset );
}

Dock::~Dock()
{

}

void Dock::_updatePicture(Direction direction)
{
  int index=0;
  Point offset;
  switch( direction )
  {
  case south: index = Impl::southPic; offset = Point( 35, 51 ); break;
  case north: index = Impl::northPic; offset = Point( 107, 61 ); break;
  case west: index = Impl::westPic; offset = Point( 48, 70 );break;
  case east: index = Impl::eastPic; offset = Point( 62, 36 ); break;

  default: break;
  }

  setPicture( ResourceGroup::transport, index );
  _animationRef().clear();
  _animationRef().load( ResourceGroup::transport, index+1, 10 );
  // now fill in reverse order
  _animationRef().load( ResourceGroup::transport, index+10, 10, Animation::reverse );
  _animationRef().setOffset( offset );
}


void Dock::_setDirection(Direction direction)
{
  _d->direction = direction;
  _updatePicture( direction );
}

Direction Dock::Impl::getDirection(PlayerCityPtr city, TilePos pos, Size size)
{
  Tilemap& tilemap = city->getTilemap();

  int s = size.getWidth();
  TilesArray constructibleTiles = tilemap.getArea( pos + TilePos( 0, 1 ), pos + TilePos( s-1, s-1 ) );
  TilesArray coastalTiles = tilemap.getArea( pos, pos + TilePos( s, 0 ) );

  if( isConstructibleArea( constructibleTiles ) && isCoastalArea( coastalTiles ) )
  { return south; }

  constructibleTiles = tilemap.getArea( pos, pos + TilePos( s-1, 1 ) );
  coastalTiles = tilemap.getArea( pos + TilePos( 0, s-1 ), pos + TilePos( s-1, s-1 ) );

  if( isConstructibleArea( constructibleTiles ) && isCoastalArea( coastalTiles ) )
  { return north; }

  constructibleTiles = tilemap.getArea( pos + TilePos( 1, 0 ), pos + TilePos( 2, 2 ) );
  coastalTiles = tilemap.getArea( pos, pos + TilePos( 0, 2 ) );

  if( isConstructibleArea( constructibleTiles ) && isCoastalArea( coastalTiles ) )
  { return west; }

  constructibleTiles = tilemap.getArea( pos, pos + TilePos( 1, 2 ) );
  coastalTiles = tilemap.getArea( pos + TilePos( 2, 0), pos + TilePos( 2, 2 ) );

  if( isConstructibleArea( constructibleTiles ) && isCoastalArea( coastalTiles ) )
  { return east; }

  return noneDirection;
}

bool Dock::Impl::isConstructibleArea(const TilesArray& tiles)
{
  bool ret = true;
  for( TilesArray::const_iterator i=tiles.begin(); i != tiles.end(); i++ )
  {
    ret &= (*i)->getFlag( Tile::isConstructible );
  }

  return ret;
}

bool Dock::Impl::isCoastalArea(const TilesArray& tiles)
{
  bool ret = true;
  for( TilesArray::const_iterator i=tiles.begin(); i != tiles.end(); i++ )
  {
    ret &= (*i)->getFlag( Tile::tlWater ) && isFlatCoast( *(*i) );
  }

  return ret;
}
