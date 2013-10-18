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

#include "oc3_building_wharf.hpp"
#include "oc3_tile.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_city.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_foreach.hpp"

class Wharf::Impl
{
public:
  std::vector<int> saveTileInfo;

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
  setPicture( ResourceGroup::wharf, 52 );
}

/* INCORRECT! */
bool Wharf::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool is_constructible = true;//Construction::canBuild( city, pos );

  DirectionType direction = _d->getDirection( city, pos );

  const_cast< Wharf* >( this )->_setPicture( direction );

  return (is_constructible && direction != D_NONE );
}

void Wharf::build(CityPtr city, const TilePos& pos)
{
  _setPicture( _d->getDirection( city, pos ) );

  CityHelper helper( city );
  TilemapArea area = city->getTilemap().getArea( pos, getSize() );

  foreach( Tile* tile, area ) { _d->saveTileInfo.push_back( TileHelper::encode( *tile ) ); }

  Factory::build( city, pos );
}

void Wharf::destroy()
{
  CityHelper helper( _getCity() );

  TilemapArea area = helper.getArea( this );

  int index=0;
  foreach( Tile* tile, area ) { TileHelper::decode( *tile, _d->saveTileInfo.at( index++ ) ); }

  Factory::destroy();
}

void Wharf::_setPicture(DirectionType direction)
{
  switch( direction )
  {
  case D_SOUTH_WEST: setPicture( ResourceGroup::wharf, 54 ); break;
  case D_NORTH_EAST: setPicture( ResourceGroup::wharf, 52 ); break;
  case D_NORTH_WEST: setPicture( ResourceGroup::wharf, 55 ); break;
  case D_SOUTH_EAST: setPicture( ResourceGroup::wharf, 53 ); break;

  default: break;
  }
}

