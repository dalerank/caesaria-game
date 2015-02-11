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

#include "loader_mission.hpp"
#include "gfx/tile.hpp"
#include "core/exception.hpp"
#include "gfx/picture.hpp"
#include "core/position.hpp"
#include "objects/objects_factory.hpp"
#include "game.hpp"
#include "core/saveadapter.hpp"
#include "loader.hpp"
#include "city/victoryconditions.hpp"
#include "city/build_options.hpp"
#include "objects/metadata.hpp"
#include "city/funds.hpp"
#include "world/empire.hpp"
#include "city/city.hpp"
#include "settings.hpp"
#include "core/variant_map.hpp"
#include "events/postpone.hpp"
#include "gamedate.hpp"
#include "core/logger.hpp"
#include "world/emperor.hpp"
#include "religion/pantheon.hpp"
#include "core/locale.hpp"
#include "city/terrain_generator.hpp"
#include "events/fishplace.hpp"
#include "climatemanager.hpp"

using namespace religion;

namespace game
{

namespace loader
{

CAESARIA_LITERALCONST(climate)
CAESARIA_LITERALCONST(adviserEnabled)
CAESARIA_LITERALCONST(fishPlaceEnabled)
static const int currentVesion = 1;
CAESARIA_LITERALCONST(random)

class Mission::Impl
{
public:
  std::string restartFile;
};

Mission::Mission()
 : _d( new Impl )
{
}

bool Mission::load( const std::string& filename, Game& game )
{
  VariantMap vm = config::load( filename );
  _d->restartFile = filename;
  
  if( currentVesion == vm[ "version" ].toInt() )
  {
    std::string mapToLoad = vm[ "map" ].toString();
    int climateType = vm.get( lc_climate, -1 );

    if( climateType >= 0 )
    {
      game::climate::initialize( (ClimateType)climateType );
    }

    if( mapToLoad == lc_random )
    {
      TerrainGenerator targar;
      VariantMap rndvm = vm[ lc_random ].toMap();
      int n2size = rndvm.get( "size", 5 );
      float smooth = rndvm.get( "smooth", 2.6 );
      float terrain = rndvm.get( "terrain", 4 );
      targar.create( game, n2size, smooth, terrain );
    }
    else
    {
      game::Loader mapLoader;
      mapLoader.load( mapToLoad, game );
    }

    PlayerCityPtr city = game.city();

    Variant vCityName = vm[ "city.name" ];
    if( vCityName.isValid() )
    {
      city->setName( vCityName.toString() );
    }

    city->player()->setRank( vm.get( "player.rank", 0 ) );
    city->funds().resolveIssue( FundIssue( city::Funds::donation, vm[ "funds" ].toInt() ) );

    Logger::warning( "GameLoaderMission: load city options ");
    city->setOption( PlayerCity::adviserEnabled, vm.get( lc_adviserEnabled, 1 ) );
    city->setOption( PlayerCity::fishPlaceEnabled, vm.get( lc_fishPlaceEnabled, 1 ) );

    game::Date::instance().init( vm[ "date" ].toDateTime() );

    VariantMap vm_events = vm[ "events" ].toMap();
    foreach( it, vm_events )
    {
      events::GameEventPtr e = events::PostponeEvent::create( it->first, it->second.toMap() );
      e->dispatch();
    }

    game.empire()->setCitiesAvailable( false );
    Logger::warning( "GameLoaderMission: load empire state" );
    game.empire()->load( vm[ "empire" ].toMap() );

    city::VictoryConditions winConditions;
    Variant winOptions = vm[ "win" ];
    Logger::warningIf( winOptions.isNull(), "GameLoaderMission: cannot load mission win options from file " + filename );

    winConditions.load( winOptions.toMap() );
    city->setVictoryConditions( winConditions );

    city::development::Options options;
    options.load( vm[ "buildoptions" ].toMap() );
    city->setBuildOptions( options  );

    game.empire()->emperor().updateRelation( city->name(), 50 );

    VariantMap fishpointsVm = vm[ "fishpoints" ].toMap();
    foreach( it, fishpointsVm )
    {
      events::GameEventPtr e = events::FishPlaceEvent::create( it->second.toTilePos(), events::FishPlaceEvent::add );
      e->dispatch();
    }

    std::string missionName = vfs::Path( filename ).baseName( false ).toString();
    Locale::addTranslation( missionName );
    SETTINGS_SET_VALUE( lastTranslation, Variant( missionName ) );

    //reseting divinities festival date
    DivinityList gods = rome::Pantheon::instance().all();
    foreach( it, gods )
    {
      rome::Pantheon::doFestival( (*it)->name(), 0 );
    }

    return true;
  }
 
  return false;
}

bool Mission::isLoadableFileExtension( const std::string& filename )
{
  return vfs::Path( filename ).isMyExtension( ".mission" );
}

int Mission::climateType(const std::string& filename) { return -1; }
std::string Mission::restartFile() const { return _d->restartFile; }

}//end namespace loader

}//end namespace game
