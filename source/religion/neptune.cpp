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
#include "city/statistic.hpp"
#include "neptune.hpp"
#include "game/gamedate.hpp"
#include "events/showinfobox.hpp"
#include "core/gettext.hpp"
#include "objects/dock.hpp"
#include "walker/ship.hpp"
#include "walker/fishing_boat.hpp"

using namespace gfx;
using namespace events;

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
  events::dispatch<ShowInfobox>( _("##wrath_of_neptune_title##"),
                                 _("##wrath_of_neptune_description##"),
                                 true,
                                 "god_neptune");

  auto boats = city->walkers().select<Ship>();

  ShipList destroyBoats = boats.random( 5 );
  for( auto&& ship : destroyBoats )
  {
    ship->die();
  }
}

void Neptune::_doSmallCurse(PlayerCityPtr city)
{
  events::dispatch<ShowInfobox>( _("##smallcurse_of_neptune_title##"),
                                 _("##smallcurse_of_neptune_description##"),
                                 true,
                                 "god_neptune" );

  DockList docks = city->statistic().objects.find<Dock>();

  DockPtr dock = docks.random();
  if( dock.isValid() )
  {
    dock->collapse();
  }
}

void Neptune::_doBlessing(PlayerCityPtr city)
{
  FishingBoatList boats = city->statistic().walkers.find<FishingBoat>( walker::fishingBoat, TilePos(-1, -1));

  boats = boats.random( math::max<size_t>( boats.size() / 5, 5 ) );
  for( auto boat : boats )
  {
    boat->addFish( boat->fishMax() - boat->fishQty() );
  }
}

}//end namespace rome

}//end namespace religion
