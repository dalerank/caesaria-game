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
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "objects/road.hpp"
#include "core/variant_map.hpp"
#include "objects_factory.hpp"
#include "core/direction.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::gatehouse, Gatehouse)

namespace {
static const Renderer::Pass rpass[2] = { Renderer::overlayAnimation, Renderer::overWalker };
static const Renderer::PassQueue gatehousePass = Renderer::PassQueue( rpass, rpass + 1 );
}

class Gatehouse::Impl
{
public:
  Pictures gatehouseSprite;
  Direction direction;

  void updateSprite();
};

Gatehouse::Gatehouse() : Building( objects::gatehouse, Size( 2 ) ), _d( new Impl )
{
  setPicture( ResourceGroup::land2a, 150 );
  _d->gatehouseSprite.resize( 1 );

  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );
}

bool Gatehouse::_update( const CityAreaInfo& areaInfo )
{
  Tilemap& tmap = areaInfo.city->tilemap();

  _d->direction = noneDirection;
  const TilePos& pos = areaInfo.pos;

  bool freemap[ countDirection ] = { 0 };
  freemap[ noneDirection ] = tmap.at( pos ).getFlag( Tile::isConstructible );
  freemap[ north ] = tmap.at( pos + TilePos( 0, 1 ) ).getFlag( Tile::isConstructible );
  freemap[ east ] = tmap.at( pos + TilePos( 1, 0 ) ).getFlag( Tile::isConstructible );
  freemap[ northEast ] = tmap.at( pos + TilePos( 1, 1 ) ).getFlag( Tile::isConstructible );

  bool rmap[ countDirection ] = { 0 };
  rmap[ noneDirection ] = is_kind_of<Road>( tmap.at( pos ).overlay() );
  rmap[ north ] = is_kind_of<Road>( tmap.at( pos + TilePos( 0, 1 ) ).overlay() );
  rmap[ northEast ] = is_kind_of<Road>( tmap.at( pos + TilePos( 1, 1 ) ).overlay() );
  rmap[ east  ] = is_kind_of<Road>( tmap.at( pos + TilePos( 1, 0 ) ).overlay() );
  rmap[ west ] = is_kind_of<Road>( tmap.at( pos + TilePos( -1, 0 ) ).overlay() );
  rmap[ northWest ] = is_kind_of<Road>(  tmap.at( pos + TilePos( -1, 1 ) ).overlay() );

  int index = 150;
  if( (rmap[ noneDirection ] && rmap[ north ]) ||
      (rmap[ east ] && rmap[ northEast ]) ||
      Building::canBuild( areaInfo ) )
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
    wrongBorder |= rmap[ east ] &&  is_kind_of<Road>( areaInfo.city->getOverlay( pos + TilePos( 2, 0 ) ) );
    wrongBorder |= rmap[ northEast ] && is_kind_of<Road>( areaInfo.city->getOverlay( pos + TilePos( 2, 1 ) ) );
  break;

  case west:
    wrongBorder = ( rmap[ noneDirection ] && is_kind_of<Road>( areaInfo.city->getOverlay( pos + TilePos( 0, -1 ) ) ) );
    wrongBorder |= ( rmap[ east ] && is_kind_of<Road>( areaInfo.city->getOverlay( pos + TilePos( 1, -1 ) ) ) );
    wrongBorder |= ( rmap[ north ] && is_kind_of<Road>( areaInfo.city->getOverlay( pos + TilePos( 0, 2 ) ) ) );
    wrongBorder |= ( rmap[ northEast ] && is_kind_of<Road>( areaInfo.city->getOverlay( pos + TilePos( 1, 2 ) ) ) );
  break;

  default:
    return false;
  }

  return (mayConstruct && !wrongBorder);
}

void Gatehouse::save(VariantMap& stream) const
{
  Building::save( stream );

  VARIANT_SAVE_ENUM_D( stream, _d, direction )
}

void Gatehouse::load(const VariantMap& stream)
{
  Building::load( stream );

  VARIANT_LOAD_ENUM_D( _d, direction, stream )

  _d->updateSprite();
}

bool Gatehouse::isWalkable() const {  return true; }
Renderer::PassQueue Gatehouse::passQueue() const{  return gatehousePass;}

const Pictures& Gatehouse::pictures(Renderer::Pass pass) const
{
  switch( pass )
  {
  case Renderer::overWalker: return _d->gatehouseSprite;
  default: break;
  }

  return Building::pictures( pass );
}

void Gatehouse::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::tlRoad, true );
}

void Gatehouse::destroy()
{
  city::Helper helper( _city() );
  TilesArray tiles = helper.getArea( this );

  foreach( it, tiles ) (*it)->setFlag( Tile::tlRoad, false );
}

bool Gatehouse::build( const CityAreaInfo& info )
{
  _update( info );
  _d->updateSprite();

  TilesArray tiles = info.city->tilemap().getArea( info.pos, size() );
  foreach( it, tiles )
  {
    RoadPtr road = ptr_cast<Road>( (*it)->overlay() );
    if( road.isValid() )
    {
      road->setState( (Construction::Param)Road::lockTerrain, 1 );
    }
  }

  return Building::build( info );
}

bool Gatehouse::canBuild( const CityAreaInfo& areaInfo ) const
{
  return const_cast< Gatehouse* >( this )->_update( areaInfo );
}


void Gatehouse::Impl::updateSprite()
{
  if( direction != noneDirection )
  {
    gatehouseSprite[ 0 ] = Picture::load( ResourceGroup::sprites, direction == north ? 224 : 225 );
    gatehouseSprite[ 0 ].setOffset( direction == north ? Point( 8, 80 ) : Point( 12, 80 ) );
  }
  else
  {
    Logger::warning( "WARNING!!! Gatehouse::updateSprite none direction used" );
  }
}
