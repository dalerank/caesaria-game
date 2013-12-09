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

Gatehouse::Gatehouse() : Building( building::gatehouse, Size( 2 ) ),
  _d( new Impl )
{
  setPicture( Picture() );

  _fgPicturesRef().resize( 2 );
  _fgPicturesRef().at( 0 ) = Picture::load( ResourceGroup::land2a, 168 );
  _fgPicturesRef().at( 1 ) = Picture::load( ResourceGroup::sprites, 224 );
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

bool Gatehouse::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  Tilemap& tmap = city->getTilemap();

  Direction direction = noneDirection;

  Tile& bTile00 = tmap.at( pos );

  bool freemap[ countDirection ] = { 0 };
  freemap[ noneDirection ] = bTile00.getFlag( Tile::isConstructible );
  freemap[ north ] = bTile00.getFlag( Tile::isConstructible );
  freemap[ south ] = tmap.at( pos - TilePos( 0, 1 ) ).getFlag( Tile::isConstructible );

  bool rmap[ countDirection ] = { 0 };
  rmap[ noneDirection ] = bTile00.getOverlay().is<Road>();
  rmap[ north ] = tmap.at( pos + TilePos( 0, 1 ) ).getOverlay().is<Road>();
  rmap[ northEast ] = tmap.at( pos + TilePos( 1, 1 ) ).getOverlay().is<Road>();
  rmap[ south ] = tmap.at( pos - TilePos( 0, 1 ) ).getOverlay().is<Road>();
  rmap[ west  ] = tmap.at( pos - TilePos( 1, 0 ) ).getOverlay().isValid();
  rmap[ east  ] = tmap.at( pos + TilePos( 1, 0 ) ).getOverlay().isValid();

  if( (rmap[ noneDirection ] || rmap[ noneDirection ] ) && (rmap[ north ] || rmap[ south ]) )
  {
    direction = north;
  }

  switch( direction )
  {
  case north:
    if( ( rmap[ noneDirection ] && (rmap[ west ] || rmap[ east ]) ) )
       return false;
  break;

  default:
    return false;
  }

  return true;
}
