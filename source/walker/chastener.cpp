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

#include "chastener.hpp"
#include "city/city.hpp"
#include "name_generator.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "objects/military.hpp"
#include "pathway/pathway_helper.hpp"
#include "gfx/tilemap.hpp"
#include "animals.hpp"
#include "enemysoldier.hpp"
#include "world/empire.hpp"
#include "world/emperor.hpp"
#include "core/foreach.hpp"
#include "helper.hpp"
#include "game/gamedate.hpp"
#include "core/variant_map.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_SOLDIER_IN_WALKERFACTORY( walker::romeChastenerSoldier, walker::romeChastenerSoldier, Chastener, chasterner )

Chastener::Chastener( PlayerCityPtr city, walker::Type type )
    : EnemySoldier( city, type )
{
  addFriend( walker::romeChastenerElephant );
}

ChastenerPtr Chastener::create( PlayerCityPtr city, walker::Type type)
{
  ChastenerPtr ret( new Chastener( city, type ) );
  ret->initialize( WalkerHelper::getOptions( type ) );
  ret->drop();

  return ret;
}

int Chastener::agressive() const { return -2; }

bool Chastener::die()
{
  _city()->empire()->emperor().remSoldiers( _city()->name(), 1 );

  return EnemySoldier::die();
}
