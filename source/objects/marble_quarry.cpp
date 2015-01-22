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
#include "objects_factory.hpp"

using namespace gfx;
using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::quarry, MarbleQuarry)

MarbleQuarry::MarbleQuarry()
  : Factory(good::none, good::marble, objects::quarry, Size(2) )
{
  _animationRef().load( ResourceGroup::commerce, 44, 10);
  _animationRef().setDelay( 4 );
  _fgPicturesRef().resize(2);

  _setClearAnimationOnStop( false );
}

void MarbleQuarry::timeStep( const unsigned long time )
{
  Factory::timeStep( time );
}

bool MarbleQuarry::canBuild( const CityAreaInfo& areaInfo ) const
{
  bool is_constructible = Construction::canBuild( areaInfo );
  bool near_mountain = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = areaInfo.city->tilemap();
  TilesArray perimetr = tilemap.getRectangle( areaInfo.pos + TilePos( -1, -1 ), size() + Size( 2 ), Tilemap::checkCorners);
  foreach( tile, perimetr )
  {
    near_mountain |= (*tile)->getFlag( Tile::tlRock );
  }

  const_cast< MarbleQuarry* >( this )->_setError( near_mountain ? "" : _("##build_near_mountain_only##") );

  return (is_constructible && near_mountain);
}
