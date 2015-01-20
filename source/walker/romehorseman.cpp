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

#include "romehorseman.hpp"
#include "city/city.hpp"
#include "name_generator.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "objects/military.hpp"
#include "pathway/pathway_helper.hpp"
#include "gfx/tilemap.hpp"
#include "animals.hpp"
#include "enemysoldier.hpp"
#include "core/foreach.hpp"
#include "game/gamedate.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_SOLDIER_IN_WALKERFACTORY( walker::romeHorseman, walker::romeHorseman, RomeSoldier, romeSoldier )

RomeHorseman::RomeHorseman( PlayerCityPtr city, walker::Type type )
    : RomeSoldier( city, type )
{
}

RomeHorsemanPtr RomeHorseman::create( PlayerCityPtr city, walker::Type type)
{
  RomeHorsemanPtr ret( new RomeHorseman( city, type ) );
  ret->drop();

  return ret;
}

/*
void RomeSoldier::send2city(FortPtr base, TilePos pos )
{
  setPos( pos );
  _d->base = base;
  _back2fort();

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}
*/
