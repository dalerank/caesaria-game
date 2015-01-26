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

#include "freeplay_finalizer.hpp"
#include "core/saveadapter.hpp"
#include "game/settings.hpp"
#include "core/variant_map.hpp"
#include "city/city.hpp"
#include "world/empire.hpp"
#include "world/emperor.hpp"
#include "city/build_options.hpp"
#include "events/postpone.hpp"

using namespace events;

namespace game
{

namespace freeplay
{

void __loadEventsFromSection( const VariantMap& vm )
{
  foreach( it, vm )
  {
    events::GameEventPtr e = events::PostponeEvent::create( it->first, it->second.toMap() );
    e->dispatch();
  }
}

void addPopulationMilestones(PlayerCityPtr city)
{
  VariantMap freeplayVm = config::load( SETTINGS_RC_PATH( freeplay_opts ) );
  __loadEventsFromSection( freeplayVm[ "population_milestones" ].toMap() );
}

void addEvents(PlayerCityPtr city)
{
  VariantMap freeplayVm = config::load( SETTINGS_RC_PATH( freeplay_opts ) );
  __loadEventsFromSection( freeplayVm[ "events" ].toMap() );
}

void resetFavour(PlayerCityPtr city)
{
  world::Emperor& emperor = city->empire()->emperor();
  emperor.updateRelation( city->name(), 50 );
}

void initBuildOptions(PlayerCityPtr city)
{
  city::development::Options bopts;
  bopts = city->buildOptions();
  bopts.setGroupAvailable( city::development::all, true );
  city->setBuildOptions( bopts );
}

}//end namespace freeplay

}//end namespace game
