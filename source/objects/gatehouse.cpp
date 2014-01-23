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
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "objects/road.hpp"
#include "core/direction.hpp"

using namespace constants;

namespace {
static const Renderer::Pass rpass[2] = { Renderer::building, Renderer::overWalker };
static const Renderer::PassQueue gatehousePass = Renderer::PassQueue( rpass, rpass + 1 );
}

class Gatehouse::Impl
{
public:
  PicturesArray gatehouseSprite;
  Direction direction;
};

Gatehouse::Gatehouse() : Building( building::gatehouse, Size( 2 ) ), _d( new Impl )
{
  setPicture( ResourceGroup::land2a, 150 );
  _d->gatehouseSprite.resize( 1 );

  _fireIncrement = 0;
  _damageIncrement = 0;
}

bool Gatehouse::_update( PlayerCityPtr city, TilePos pos )
{
  Tilemap& tmap = city->getTilemap();

  _d->direction = noneDirection;

  bool freemap[ countDirection ] = { 0 };
  freemap[ noneDirection ] = tmap.at( pos ).getFlag( Tile::isConstructible );
  freemap[ north ] = tmap.at( pos + TilePos( 0, 1 ) ).getFlag( Tile::isConstructible );
  freemap[ east ] = tmap.at( pos + TilePos( 1, 0 ) ).getFlag( Tile::isConstructible );
  freemap[ northEast ] = tmap.at( pos + TilePos( 1, 1 ) ).getFlag( Tile::isConstructible );

  bool rmap[ countDirection ] = { 0 };
  rmap[ noneDirection ] = is_kind_of<Road>( tmap.at( pos ).getOverlay() );
  rmap[ north ] = is_kind_of<Road>( tmap.at( pos + TilePos( 0, 1 ) ).getOverlay() );
  rmap[ northEast ] = is_kind_of<Road>( tmap.at( pos + TilePos( 1, 1 ) ).getOverlay() );
  rmap[ east  ] = is_kind_of<Road>( tmap.at( pos + TilePos( 1, 0 ) ).getOverlay() );
  rmap[ west ] = is_kind_of<Road>( tmap.at( pos + TilePos( -1, 0 ) ).getOverlay() );
  rmap[ northWest ] = is_kind_of<Road>(  tmap.at( pos + TilePos( -1, 1 ) ).getOverlay() );

  int index = 150;
  if( (rmap[ noneDirection ] && rmap[ north ]) ||
      (rmap[ east ] && rmap[ northEast ]) ||
      Building::canBuild( city, pos, TilesArray() ) )
  {
    _d->direction = north;
    index = 150;
  }

  if( (rmap[ noneDirection ] && rmap[ east ]) ||
      (rmap[ northEast ] && rmap[ north ] ) )
  {
      _d->direction = west;
    index = 151;
  }

  setPicture( ResourceGroup::land2a, index );

  bool mayConstruct = ((rmap[ noneDirection ] || freemap[ noneDirection ]) &&
                       (rmap[ north ] || freemap[ north ]) &&
                       (rmap[ east ] || freemap[ east ]) &&
                       (rmap[ northEast ] || freemap[ northEast ]) );

  bool wrongBorder = false;
  switch( _d->direction )
  {
  case north:
    wrongBorder = ( rmap[ noneDirection ] && rmap[ west ] );
    wrongBorder |= ( rmap[ north ] && rmap[ northWest ] );
    wrongBorder |= rmap[ east ] &&  is_kind_of<Road>( city->getOverlay( pos + TilePos( 2, 0 ) ) );
    wrongBorder |= rmap[ northEast ] && is_kind_of<Road>( city->getOverlay( pos + TilePos( 2, 1 ) ) );
  break;

  case west:
    wrongBorder = ( rmap[ noneDirection ] && is_kind_of<Road>( city->getOverlay( pos + TilePos( 0, -1 ) ) ) );
    wrongBorder |= ( rmap[ east ] && is_kind_of<Road>( city->getOverlay( pos + TilePos( 1, -1 ) ) ) );
    wrongBorder |= ( rmap[ north ] && is_kind_of<Road>( city->getOverlay( pos + TilePos( 0, 2 ) ) ) );
    wrongBorder |= ( rmap[ northEast ] && is_kind_of<Road>( city->getOverlay( pos + TilePos( 1, 2 ) ) ) );
  break;

  default:
    return false;
  }

  return (mayConstruct && !wrongBorder);
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

Renderer::PassQueue Gatehouse::getPassQueue() const
{
  return gatehousePass;
}

const PicturesArray& Gatehouse::getPictures(Renderer::Pass pass) const
{
  switch( pass )
  {
  case Renderer::overWalker: return _d->gatehouseSprite;
  default: break;
  }

  return Building::getPictures( pass );
}

void Gatehouse::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlRoad, true );
}

void Gatehouse::build(PlayerCityPtr city, const TilePos &pos)
{
  _update( city, pos );

  _d->gatehouseSprite[ 0 ] = Picture::load( ResourceGroup::sprites, _d->direction == north ? 224 : 225 );
  _d->gatehouseSprite[ 0 ].setOffset( _d->direction == north ? Point( 8, 80 ) : Point( 12, 80 ) );

  Building::build( city, pos );
}

bool Gatehouse::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  return const_cast< Gatehouse* >( this )->_update( city, pos );
}
