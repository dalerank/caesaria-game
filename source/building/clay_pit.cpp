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

#include "clay_pit.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tile.hpp"
#include "game/city.hpp"
#include "core/foreach.hpp"
#include "game/tilemap.hpp"
#include "core/gettext.hpp"
#include "building/constants.hpp"

ClayPit::ClayPit() : Factory( Good::none, Good::clay, constants::building::clayPit, Size(2) )
{
  _fgPicturesRef().resize(2);
}

void ClayPit::timeStep( const unsigned long time )
{
  Factory::timeStep( time );
}

bool ClayPit::canBuild(PlayerCityPtr city, const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( city, pos );
  bool near_water = false;

  Tilemap& tilemap = city->getTilemap();
  TilesArray perimetr = tilemap.getRectangle( pos + TilePos( -1, -1), getSize() + Size( 2 ), Tilemap::checkCorners );
  foreach( Tile* tile, perimetr )
  {
    near_water |= tile->getFlag( Tile::tlWater );
  }

  const_cast<ClayPit*>( this )->_setError( near_water ? "" : _("##clay_pit_need_water##") );

  return (is_constructible && near_water);
} 
