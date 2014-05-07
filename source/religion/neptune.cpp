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

#include "neptune.hpp"
#include "game/gamedate.hpp"
#include "events/showinfobox.hpp"
#include "core/gettext.hpp"
#include "city/helper.hpp"
#include "walker/ship.hpp"

using namespace constants;
using namespace gfx;

namespace religion
{

namespace rome
{


DivinityPtr Neptune::create()
{
  DivinityPtr ret( new Neptune() );
  ret->setInternalName( baseDivinityNames[ romeDivNeptune ] );
  ret->drop();

  return ret;
}

void Neptune::updateRelation(float income, PlayerCityPtr city)
{
  RomeDivinity::updateRelation( income, city );
}

void Neptune::_doWrath(PlayerCityPtr city)
{
  events::GameEventPtr event = events::ShowInfobox::create( _("##wrath_of_neptune_title##"),
                                                            _("##wrath_of_neptune_description##"),
                                                            events::ShowInfobox::send2scribe );
  event->dispatch();

  city::Helper helper( city );
  ShipList boats = helper.find<Ship>( walker::any, city::Helper::invalidPos );

  int destroyBoats = math::random( boats.size() );
  for( int i=0; i < destroyBoats; i++ )
  {
    ShipList::iterator it = boats.begin();
    std::advance( it, math::random( boats.size() ) );
    (*it)->deleteLater();
    boats.erase( it );
  }
}

void Neptune::_doSmallCurse(PlayerCityPtr city)
{

}

void Neptune::_doBlessing(PlayerCityPtr city)
{

}

}//end namespace rome

}//end namespace religion
