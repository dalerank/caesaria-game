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

#include "elevation.hpp"
#include "constants.hpp"
#include "gfx/helper.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::elevation, Elevation)

namespace {
  static const int startElevationId = 845;
}

class Elevation::Impl
{
public:
  int basicImgId;
};

Elevation::Elevation()
  : TileOverlay( constants::objects::elevation, Size( 2 ) ), _d( new Impl )
{
  setDebugName( CAESARIA_STR_EXT(Elevation) );
}

Elevation::~Elevation(){}

void Elevation::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlRoad, true );

  terrain.setPicture( Picture::getInvalid() );
}

bool Elevation::isWalkable() const{  return true;}
bool Elevation::isFlat() const{  return true;}

Point Elevation::offset( const Tile& tile, const Point& subpos) const
{
  return Point( -(5 - subpos.y()), 0 );
}

void Elevation::changeDirection(Tile* masterTile, constants::Direction direction)
{
  int imgid = _d->basicImgId - startElevationId;

  TileOverlay::changeDirection( masterTile, direction );
  setPicture( imgid::toPicture( startElevationId + (imgid + (direction - 1) / 2 ) % 4 ) );
}

bool Elevation::isDestructible() const { return false; }

bool Elevation::build( const CityAreaInfo& info )
{
  bool res = TileOverlay::build( info );
  _d->basicImgId = info.city->tilemap().at( info.pos ).originalImgId();

  return res;
}
