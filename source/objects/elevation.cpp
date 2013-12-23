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

#include "elevation.hpp"
#include "constants.hpp"
#include "gfx/tile.hpp"

Elevation::Elevation() : TileOverlay( constants::building::elevation, Size( 2 ) )
{
  setDebugName( CAESARIA_STR_EXT(Elevation) );
}

Elevation::~Elevation()
{

}

void Elevation::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlRoad, true );
}

bool Elevation::isWalkable() const
{
  return true;
}

bool Elevation::isFlat() const
{
  return true;
}

Point Elevation::getOffset( Tile& tile, const Point& subpos) const
{
  TilePos delta = getTilePos() - tile.getIJ();

  return Point( -(5 - subpos.getY()), 0 );
}

bool Elevation::isDestructible() const
{
  return false;
}
