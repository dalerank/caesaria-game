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

#include "marble_quarry.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tile.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "constants.hpp"
#include "core/foreach.hpp"

MarbleQuarry::MarbleQuarry() : Factory(Good::none, Good::marble, constants::building::marbleQuarry, Size(2) )
{
  _animationRef().load( ResourceGroup::commerce, 44, 10);
  _animationRef().setDelay( 4 );
  _fgPicturesRef().resize(2);
}

void MarbleQuarry::timeStep( const unsigned long time )
{
  bool mayAnimate = numberWorkers() > 0;

  if( mayAnimate && _animationRef().isStopped() )
  {
    _animationRef().start();
  }

  if( !mayAnimate && _animationRef().isRunning() )
  {
    _animationRef().stop();
  }

  Factory::timeStep( time );
}

bool MarbleQuarry::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  bool is_constructible = Construction::canBuild( city, pos, aroundTiles );
  bool near_mountain = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = city->tilemap();
  TilesArray perimetr = tilemap.getRectangle( pos + TilePos( -1, -1 ), size() + Size( 2 ), Tilemap::checkCorners);
  foreach( tile, perimetr )
  {
    near_mountain |= (*tile)->getFlag( Tile::tlRock );
  }

  const_cast< MarbleQuarry* >( this )->_setError( near_mountain ? "" : _("##build_near_mountain_only##") );

  return (is_constructible && near_mountain);
}
