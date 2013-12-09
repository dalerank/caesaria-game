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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "gatehouse.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "game/city.hpp"
#include "gfx/tilemap.hpp"
#include "game/road.hpp"
#include "core/direction.hpp"

using namespace constants;

class Gatehouse::Impl
{
public:
};

Gatehouse::Gatehouse() : Building( building::gatehouse, Size( 2 ) ), _d( new Impl )
{
  setPicture( ResourceGroup::land2a, 150 );
}

void Gatehouse::save(VariantMap& stream) const
{
  Building::save( stream );
}

void Gatehouse::load(const VariantMap& stream)
{
  Building::load( stream );
}

bool Gatehouse::isWalkable() const
{
  return true;
}

bool Gatehouse::isRoad() const
{
  return true;
}

bool Gatehouse::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  Tilemap& tmap = city->getTilemap();

  Direction direction = noneDirection;

  bool freemap[ countDirection ] = { 0 };
  freemap[ noneDirection ] = tmap.at( pos ).getFlag( Tile::isConstructible );
  freemap[ north ] = tmap.at( pos + TilePos( 0, 1 ) ).getFlag( Tile::isConstructible );
  freemap[ east ] = tmap.at( pos + TilePos( 1, 0 ) ).getFlag( Tile::isConstructible );
  freemap[ northEast ] = tmap.at( pos + TilePos( 1, 1 ) ).getFlag( Tile::isConstructible );

  bool rmap[ countDirection ] = { 0 };
  rmap[ noneDirection ] = tmap.at( pos ).getOverlay().is<Road>();
  rmap[ north ] = tmap.at( pos + TilePos( 0, 1 ) ).getOverlay().is<Road>();
  rmap[ northEast ] = tmap.at( pos + TilePos( 1, 1 ) ).getOverlay().is<Road>();
  rmap[ east  ] = tmap.at( pos + TilePos( 1, 0 ) ).getOverlay().is<Road>();
  rmap[ west ] = tmap.at( pos + TilePos( -1, 0 ) ).getOverlay().is<Road>();
  rmap[ northWest ] = tmap.at( pos + TilePos( -1, 1 ) ).getOverlay().is<Road>();

  if( (rmap[ noneDirection ] && rmap[ north ]) ||
      (rmap[ east ] && rmap[ northEast ]) ||
      Building::canBuild( city, pos, aroundTiles ) )
  {
    direction = north;
    const_cast< Gatehouse* >( this )->setPicture( ResourceGroup::land2a, 150 );
  }
  else if( (rmap[ noneDirection ] && rmap[ east ]) ||
           (rmap[ northEast ] && rmap[ north ] ) )
  {
    direction = west;
    const_cast< Gatehouse* >( this )->setPicture( ResourceGroup::land2a, 151 );
  }

  bool mayConstruct = ((rmap[ noneDirection ] || freemap[ noneDirection ]) &&
                       (rmap[ north ] || freemap[ north ]) &&
                       (rmap[ east ] || freemap[ east ]) &&
                       (rmap[ northEast ] || freemap[ northEast ]) );

  bool wrongBorder = false;
  switch( direction )
  {
  case north:
    wrongBorder = ( rmap[ noneDirection ] && rmap[ west ] );
    wrongBorder |= ( rmap[ north ] && rmap[ northWest ] );
    wrongBorder |= ( rmap[ east ] && tmap.at( pos + TilePos( 2, 0 ) ).getOverlay().is<Road>() );
    wrongBorder |= ( rmap[ northEast ] && tmap.at( pos + TilePos( 2, 1 ) ).getOverlay().is<Road>() );
  break;

  case west:
    wrongBorder = ( rmap[ noneDirection ] && tmap.at( pos + TilePos( 0, -1 ) ).getOverlay().is<Road>() );
    wrongBorder |= ( rmap[ east ] && tmap.at( pos + TilePos( 1, -1 ) ).getOverlay().is<Road>() );
    wrongBorder |= ( rmap[ north ] && tmap.at( pos + TilePos( 0, 2 ) ).getOverlay().is<Road>() );
    wrongBorder |= ( rmap[ northEast ] && tmap.at( pos + TilePos( 1, 2 ) ).getOverlay().is<Road>() );
  break;

  default:
    return false;
  }

  return (mayConstruct && !wrongBorder);
}
