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
#include "gfx/tilesarray.hpp"
#include "world/config.hpp"
#include "gfx/tilemap.hpp"

using namespace events;
using namespace gfx;

namespace game
{

namespace freeplay
{

void __loadEventsFromSection( const VariantMap& vm )
{
  for( auto& it : vm )
  {
    events::dispatch<PostponeEvent>( it.first, it.second.toMap() );
  }
}

Finalizer::Finalizer(PlayerCityPtr city) :
  _city( city )
{

}

void Finalizer::addPopulationMilestones()
{
  VariantMap freeplayVm = config::load( SETTINGS_RC_PATH( freeplay_opts ) );
  __loadEventsFromSection( freeplayVm[ "population_milestones" ].toMap() );
}

void Finalizer::addEvents()
{
  VariantMap freeplayVm = config::load( SETTINGS_RC_PATH( freeplay_opts ) );
  __loadEventsFromSection( freeplayVm[ "events" ].toMap() );
}

void Finalizer::resetIronCovery(int qty)
{
  TilesArray tiles = _city->tilemap().allTiles().select( Tile::tlRock );

  for( auto tile : tiles )
    tile->setParam( Tile::pIron, math::random( qty ) );
}

void Finalizer::resetFavour()
{
  world::Emperor& emperor = _city->empire()->emperor();
  emperor.updateRelation( _city->name(), config::emperor::defaultFavor );
}

void Finalizer::initBuildOptions()
{
  city::development::Options bopts;
  bopts = _city->buildOptions();
  bopts.setGroupAvailable( city::development::all, true );
  _city->setBuildOptions( bopts );
}

}//end namespace freeplay

}//end namespace game
