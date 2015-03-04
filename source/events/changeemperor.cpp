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
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "changeemperor.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "world/emperor.hpp"
#include "events/showinfobox.hpp"
#include "world/empire.hpp"
#include "factory.hpp"
#include "core/gettext.hpp"

namespace events
{

REGISTER_EVENT_IN_FACTORY(ChangeEmperor, "change_emperor" )

GameEventPtr ChangeEmperor::create()
{
  GameEventPtr ret( new ChangeEmperor() );
  ret->drop();

  return ret;
}

void ChangeEmperor::load(const VariantMap& stream)
{
  _vars = stream;
}

bool ChangeEmperor::_mayExec(Game& game, unsigned int time) const {  return true; }

void ChangeEmperor::_exec(Game& game, unsigned int)
{
  if( !_vars.empty() )
  {
    world::Emperor& emperor = game.empire()->emperor();
    std::string empName = _vars.get( "name" ).toString();
    StringArray resetCities = _vars.get( "reset_relations" ).toStringArray();
    if( resetCities.empty() )
      resetCities << "all";

    emperor.resetRelations( resetCities );
    emperor.setName( empName );

    std::string text = _vars.get( "text" ).toString();

    if( text.empty() )
    {
      text = _( "##emperor_changed_default_text##" ) + empName;
    }

    GameEventPtr e = events::ShowInfobox::create( "##emperor_changed_title##", text );
    e->dispatch();
  }
}

}
