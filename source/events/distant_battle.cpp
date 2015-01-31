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

#include "distant_battle.hpp"
#include "game/game.hpp"
#include "world/empire.hpp"
#include "world/object.hpp"
#include "factory.hpp"

using namespace constants;

namespace events
{

REGISTER_EVENT_IN_FACTORY(DistantBattle, "distant_battle")

GameEventPtr DistantBattle::create()
{
  GameEventPtr ret( new DistantBattle() );
  ret->drop();

  return ret;
}

void DistantBattle::load(const VariantMap& stream)
{
  GameEvent::load( stream );
  _options = stream;
}

VariantMap DistantBattle::save() const
{
  return _options;
}

void DistantBattle::_exec(Game& game, unsigned int)
{
  world::EmpirePtr empire = game.empire();

  world::ObjectPtr obj = world::Object::create( empire );
  obj->load( _options );
  empire->addObject( obj );
}

bool DistantBattle::_mayExec(Game& game, unsigned int ) const { return true; }

}
