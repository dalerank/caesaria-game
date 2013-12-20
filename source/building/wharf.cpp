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

#include "wharf.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "game/cityhelper.hpp"
#include "gfx/tilemap.hpp"
#include "core/foreach.hpp"
#include "walker/fishing_boat.hpp"
#include "core/foreach.hpp"
#include "game/goodstore.hpp"
#include "constants.hpp"

using namespace constants;

class Wharf::Impl
{
public:
  enum { southPic=54, northPic=52, westPic=55, eastPic=53 };
  std::vector<int> saveTileInfo;
  Direction direction;
  FishingBoatPtr boat;

  Direction getDirection( PlayerCityPtr city, TilePos pos )
  {
    Tilemap& tilemap = city->getTilemap();
    Tile& t00 = tilemap.at( pos );
    Tile& t10 = tilemap.at( pos + TilePos( 1, 0 ) );
    Tile& t01 = tilemap.at( pos + TilePos( 0, 1 ) );
    Tile& t11 = tilemap.at( pos + TilePos( 1, 1 ) );

    if( t00.getFlag( Tile::tlWater ) && t10.getFlag( Tile::tlWater )
        && t01.getFlag( Tile::isConstructible ) && t11.getFlag( Tile::isConstructible ) )
    {
      return south;
    }

    if( t01.getFlag( Tile::tlWater ) && t11.getFlag( Tile::tlWater )
        && t00.getFlag( Tile::isConstructible ) && t10.getFlag( Tile::isConstructible ) )
    {
      return north;
    }

    if( t00.getFlag( Tile::tlWater ) && t01.getFlag( Tile::tlWater )
        && t10.getFlag( Tile::isConstructible ) && t11.getFlag( Tile::isConstructible ) )
    {
      return west;
    }

    if( t10.getFlag( Tile::tlWater ) && t11.getFlag( Tile::tlWater )
        && t00.getFlag( Tile::isConstructible ) && t01.getFlag( Tile::isConstructible ) )
    {
      return east;
    }

    return noneDirection;
  }
};

Wharf::Wharf() : Factory(Good::none, Good::fish, building::wharf, Size(2)), _d( new Impl )
{
  // transport 52 53 54 55
  setPicture( ResourceGroup::wharf, Impl::northPic );
}

bool Wharf::canBuild( PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  bool is_constructible = true;//Construction::canBuild( city, pos );

  Direction direction = _d->getDirection( city, pos );

  const_cast< Wharf* >( this )->_setDirection( direction );

  return (is_constructible && direction != noneDirection );
}

void Wharf::build(PlayerCityPtr city, const TilePos& pos)
{
  _setDirection( _d->getDirection( city, pos ) );

  CityHelper helper( city );
  TilesArray area = city->getTilemap().getArea( pos, getSize() );

  foreach( Tile* tile, area ) { _d->saveTileInfo.push_back( TileHelper::encode( *tile ) ); }

  Factory::build( city, pos );
}

void Wharf::destroy()
{
  CityHelper helper( _getCity() );

  TilesArray area = helper.getArea( this );

  if( _d->boat.isValid() )
  {
    _d->boat->die();
  }

  int index=0;
  foreach( Tile* tile, area ) { TileHelper::decode( *tile, _d->saveTileInfo.at( index++ ) ); }

  Factory::destroy();
}

void Wharf::timeStep(const unsigned long time)
{
  WorkingBuilding::timeStep(time);

  //try get good from storage building for us
  if( time % 22 == 1 && getWorkersCount() > 0 && getWalkers().size() == 0 )
  {
    receiveGood();
    deliverGood();

    if( _d->boat.isNull() )
    {
      _d->boat = FishingBoat::create( _getCity() );
      _d->boat->send2City( this, getLandingTile().getIJ() );
    }
  }

  //start/stop animation when workers found
  bool mayAnimate = mayWork();

  if( mayAnimate && _animationRef().isStopped() )
  {
    _animationRef().start();
  }

  if( !mayAnimate && _animationRef().isRunning() )
  {
    _animationRef().stop();
  }

  //no workers or no good in stock... stop animate
  if( !mayAnimate )
  {
    return;
  }

  if( getProgress() >= 100.0 )
  {
    if( getGoodStore().getCurrentQty( getInGoodType() ) < getGoodStore().getMaxQty( getOutGoodType() )  )
    {
      updateProgress( -100.f );
      //gcc fix for temporaly ref object
      GoodStock tmpStock( getOutGoodType(), 100, 100 );
      getGoodStore().store( tmpStock, 100 );
    }
  }
  else
  {
    if( _d->boat.isValid() && !_d->boat->isBusy() && getOutGood().empty() )
    {
      _d->boat->startCatch();
    }
  }
}

void Wharf::save(VariantMap& stream) const
{
  Factory::save( stream );

  stream[ "direction" ] = (int)_d->direction;
  stream[ "saved_tile"] = VariantList( _d->saveTileInfo );
}

void Wharf::load(const VariantMap& stream)
{
  Factory::load( stream );

  _d->direction = (Direction)stream.get( "direction", (int)southWest ).toInt();
  _d->saveTileInfo << stream.get( "saved_tile" ).toList();
}

const Tile& Wharf::getLandingTile() const
{
  Tilemap& tmap = _getCity()->getTilemap();
  TilePos offset( -999, -999 );
  switch( _d->direction )
  {
  case south: offset = TilePos( 0, -1 ); break;
  case west: offset = TilePos( -1, 0 ); break;
  case north: offset = TilePos( 0, 2 ); break;
  case east: offset = TilePos( 2, 0 ); break;

  default: break;
  }

  return tmap.at( getTilePos() + offset );
}

FishingBoatPtr Wharf::getBoat() const
{
  return _d->boat;
}

void Wharf::assignBoat(FishingBoatPtr boat)
{
  _d->boat = boat;
}

void Wharf::_setDirection(Direction direction)
{
  _d->direction = direction;
  switch( direction )
  {
  case south: setPicture( ResourceGroup::wharf, Impl::southPic ); break;
  case north: setPicture( ResourceGroup::wharf, Impl::northPic ); break;
  case west: setPicture( ResourceGroup::wharf, Impl::westPic ); break;
  case east: setPicture( ResourceGroup::wharf, Impl::eastPic ); break;

  default: break;
  }
}



