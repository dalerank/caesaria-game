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

#include "objects/construction.hpp"
#include "city/helper.hpp"
#include "neptune.hpp"
#include "game/gamedate.hpp"
#include "events/showinfobox.hpp"
#include "core/gettext.hpp"
#include "objects/dock.hpp"
#include "walker/ship.hpp"
#include "walker/fishing_boat.hpp"

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
                                                            events::ShowInfobox::send2scribe,
                                                            ":/smk/God_Neptune.smk");
  event->dispatch();

  ShipList boats;
  boats << city->walkers();

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
  events::GameEventPtr event = events::ShowInfobox::create( _("##smallcurse_of_neptune_title##"),
                                                            _("##smallcurse_of_neptune_description##"),
                                                            events::ShowInfobox::send2scribe );
  event->dispatch();

  DockList docks;
  docks << city->overlays();

  DockPtr dock = docks.random();
  if( dock.isValid() )
  {
    dock->collapse();
  }
}

void Neptune::_doBlessing(PlayerCityPtr city)
{
  city::Helper helper( city );
  FishingBoatList boats = helper.find<FishingBoat>( walker::fishingBoat, city::Helper::invalidPos );

  FishingBoatPtr boat = boats.random();
  foreach( it, boats )
  {
    if( (*it)->fishQty() < boat->fishQty() )
      boat = *it;
  }

  boat->addFish( boat->fishMax() - boat->fishQty() );
}

}//end namespace rome

}//end namespace religion
