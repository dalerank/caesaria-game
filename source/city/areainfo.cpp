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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "areainfo.hpp"
#include "city.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tile_config.hpp"
#include "gfx/tilesarray.hpp"

namespace city
{

static const gfx::TilesArray clearTiles;

const gfx::TilesArray& AreaInfo::tiles() const
{
  return (_tiles != 0 ? *_tiles : clearTiles);
}

const gfx::Tile& AreaInfo::tile() const
{
  return city.isValid() ? city->tilemap().at( pos ) : gfx::tile::getInvalid();
}

AreaInfo::AreaInfo(PlayerCityPtr rcity,
                   const TilePos& rpos,
                   const gfx::TilesArray* tiles)
      : city(rcity), pos(rpos), _tiles( tiles )
  {}

}//end namespace city
