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
#include "city/city.hpp"
#include "city/build_options.hpp"
#include "events/postpone.hpp"

using namespace events;

void __loadEventsFromSection( const VariantMap& vm )
{
  foreach( it, vm )
  {
    events::GameEventPtr e = events::PostponeEvent::create( it->first, it->second.toMap() );
    e->dispatch();
  }
}

void FreeplayFinalizer::addPopulationMilestones(PlayerCityPtr city)
{
  VariantMap freeplayVm = SaveAdapter::load( SETTINGS_RC_PATH( freeplay_opts ) );
  __loadEventsFromSection( freeplayVm[ "population_milestones" ].toMap() );
}

void FreeplayFinalizer::addEvents(PlayerCityPtr city)
{
  VariantMap freeplayVm = SaveAdapter::load( SETTINGS_RC_PATH( freeplay_opts ) );
  __loadEventsFromSection( freeplayVm[ "events" ].toMap() );
}

void FreeplayFinalizer::initBuildOptions(PlayerCityPtr city)
{
  city::BuildOptions bopts;
  bopts = city->buildOptions();
  bopts.setGroupAvailable( BM_MAX, true );
  city->setBuildOptions( bopts );
}
