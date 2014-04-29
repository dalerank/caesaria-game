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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "catapult.hpp"
#include "core/gettext.hpp"
#include "city/city.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;

namespace {
  const int attackDistance = 16;
}

Catapult::Catapult(PlayerCityPtr city )
  : WallGuard( city, walker::catapult )
{
  _setType( walker::catapult );
  setSpeedMultiplier( 0.f );
  setAttackDistance( 8 );

  _isActive = false;

  setName( _("##catapult##") );
}

CatapultPtr Catapult::create(PlayerCityPtr city)
{
  CatapultPtr ret( new Catapult( city ) );
  ret->drop();

  return ret;
}

Catapult::~Catapult(){}

void Catapult::setActive(bool active)
{
  _isActive = active;
  _setAction( active ? acWork : acNone );
}
