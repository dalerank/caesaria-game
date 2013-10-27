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

#include "wharf.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "game/city.hpp"
#include "game/tilemap.hpp"
#include "core/foreach.hpp"
#include "walker/fishing_boat.hpp"
#include "core/foreach.hpp"
#include "game/goodstore.hpp"

class Wharf::Impl
{
public:
  enum { southWestPic=54, northEastPic=52, northWestPic=55, southEastPic=53 };
  std::vector<int> saveTileInfo;
  DirectionType direction;
  FishingBoatPtr boat;

  DirectionType getDirection( CityPtr city, TilePos pos )
  {
    Tilemap& tilemap = city->getTilemap();
    Tile& t00 = tilemap.at( pos );
    Tile& t10 = tilemap.at( pos + TilePos( 1, 0 ) );
    Tile& t01 = tilemap.at( pos + TilePos( 0, 1 ) );
    Tile& t11 = tilemap.at( pos + TilePos( 1, 1 ) );

    if( t00.getFlag( Tile::tlWater ) && t10.getFlag( Tile::tlWater )
        && t01.getFlag( Tile::isConstructible ) && t11.getFlag( Tile::isConstructible ) )
    {
      return D_SOUTH_WEST;
    }

    if( t01.getFlag( Tile::tlWater ) && t11.getFlag( Tile::tlWater )
        && t00.getFlag( Tile::isConstructible ) && t10.getFlag( Tile::isConstructible ) )
    {
      return D_NORTH_EAST;
    }

    if( t00.getFlag( Tile::tlWater ) && t01.getFlag( Tile::tlWater )
        && t10.getFlag( Tile::isConstructible ) && t11.getFlag( Tile::isConstructible ) )
    {
      return D_NORTH_WEST;
    }

    if( t10.getFlag( Tile::tlWater ) && t11.getFlag( Tile::tlWater )
        && t00.getFlag( Tile::isConstructible ) && t01.getFlag( Tile::isConstructible ) )
    {
      return D_SOUTH_EAST;
    }

    return D_NONE;
  }
};

Wharf::Wharf() : Factory(Good::none, Good::fish, B_WHARF, Size(2)), _d( new Impl )
{
  // transport 52 53 54 55
  setPicture( ResourceGroup::wharf, Impl::northEastPic );
}

bool Wharf::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool is_constructible = true;//Construction::canBuild( city, pos );

  DirectionType direction = _d->getDirection( city, pos );

  const_cast< Wharf* >( this )->_setDirection( direction );

  return (is_constructible && direction != D_NONE );
}

void Wharf::build(CityPtr city, const TilePos& pos)
{
  _setDirection( _d->getDirection( city, pos ) );

  CityHelper helper( city );
  TilemapArea area = city->getTilemap().getArea( pos, getSize() );

  foreach( Tile* tile, area ) { _d->saveTileInfo.push_back( TileHelper::encode( *tile ) ); }

  Factory::build( city, pos );
}

void Wharf::destroy()
{
  CityHelper helper( _getCity() );

  TilemapArea area = helper.getArea( this );

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
  if( time % 22 == 1 && getWorkers() > 0 && getWalkerList().size() == 0 )
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

  if( mayAnimate && _getAnimation().isStopped() )
  {
    _getAnimation().start();
  }

  if( !mayAnimate && _getAnimation().isRunning() )
  {
    _getAnimation().stop();
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

  _d->direction = (DirectionType)stream.get( "direction", (int)D_SOUTH_WEST ).toInt();
  _d->saveTileInfo << stream.get( "saved_tile" ).toList();
}

const Tile& Wharf::getLandingTile() const
{
  Tilemap& tmap = _getCity()->getTilemap();
  TilePos offset( -999, -999 );
  switch( _d->direction )
  {
  case D_SOUTH_WEST: offset = TilePos( 0, -1 ); break;
  case D_NORTH_WEST: offset = TilePos( -1, 0 ); break;
  case D_NORTH_EAST: offset = TilePos( 0, 1 ); break;
  case D_SOUTH_EAST: offset = TilePos( 1, 0 ); break;

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

void Wharf::_setDirection(DirectionType direction)
{
  _d->direction = direction;
  switch( direction )
  {
  case D_SOUTH_WEST: setPicture( ResourceGroup::wharf, Impl::southWestPic ); break;
  case D_NORTH_EAST: setPicture( ResourceGroup::wharf, Impl::northEastPic ); break;
  case D_NORTH_WEST: setPicture( ResourceGroup::wharf, Impl::northWestPic ); break;
  case D_SOUTH_EAST: setPicture( ResourceGroup::wharf, Impl::southEastPic ); break;

  default: break;
  }
}



