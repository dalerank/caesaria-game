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

#include "pottery.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "core/gettext.hpp"
#include "constants.hpp"
#include "good/goodstore.hpp"

using namespace constants;
using namespace gfx;

Pottery::Pottery() : Factory(Good::clay, Good::pottery, objects::pottery, Size(2))
{
  _fgPicturesRef().resize( 3 );
}

bool Pottery::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  bool ret = Factory::canBuild( city, pos, aroundTiles );
  return ret;
}

bool Pottery::build(PlayerCityPtr city, const TilePos& pos)
{
  Factory::build( city, pos );
  city::Helper helper( city );
  bool haveClaypit = !helper.find<Building>( objects::clayPit ).empty();

  _setError( haveClaypit ? "" : "##need_clay_pit##" );

  return true;
}

void Pottery::timeStep( const unsigned long time )
{
  Factory::timeStep( time );
}

void Pottery::deliverGood()
{
  Factory::deliverGood();
}

void Pottery::_storeChanged()
{
  _fgPicturesRef()[1] = inStockRef().empty() ? Picture() : Picture::load( ResourceGroup::commerce, 157 );
  _fgPicturesRef()[1].setOffset( 45, -10 );
}
