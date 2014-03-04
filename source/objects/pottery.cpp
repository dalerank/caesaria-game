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

#include "pottery.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "core/gettext.hpp"
#include "constants.hpp"

using namespace constants;

Pottery::Pottery() : Factory(Good::clay, Good::pottery, building::pottery, Size(2))
{
  _animationRef().load(ResourceGroup::commerce, 133, 7);
  _animationRef().setDelay( 3 );
  _fgPicturesRef().resize(2);
}

bool Pottery::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  bool ret = Factory::canBuild( city, pos, aroundTiles );
  return ret;
}

void Pottery::build(PlayerCityPtr city, const TilePos& pos)
{
  Factory::build( city, pos );
  city::Helper helper( city );
  bool haveClaypit = !helper.find<Building>( building::clayPit ).empty();

  _setError( haveClaypit ? "" : "##need_clay_pit##" );
}

void Pottery::timeStep( const unsigned long time )
{
  Factory::timeStep( time );
}

void Pottery::deliverGood()
{
  Factory::deliverGood();
}
