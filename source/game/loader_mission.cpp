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

namespace {
CAESARIA_LITERALCONST(climate)
CAESARIA_LITERALCONST(adviserEnabled)
CAESARIA_LITERALCONST(fishPlaceEnabled)
static const int currentVesion = 1;
CAESARIA_LITERALCONST(random)
}

class GameLoaderMission::Impl
{
public:
  std::string restartFile;
};

GameLoaderMission::GameLoaderMission()
 : _d( new Impl )
{
}

bool GameLoaderMission::load( const std::string& filename, Game& game )
{
  VariantMap vm = SaveAdapter::load( filename );
  _d->restartFile = filename;
  
  if( currentVesion == vm[ "version" ].toInt() )
  {
    std::string mapToLoad = vm[ "map" ].toString();
    int climateType = vm.get( lc_climate, -1 );

    if( climateType >= 0 )
    {
      ClimateManager::initialize( (ClimateType)climateType );
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
      GameLoader mapLoader;
      mapLoader.load( mapToLoad, game );
    }

    PlayerCityPtr city = game.city();

    Variant vCityName = vm[ "city.name" ];
    if( vCityName.isValid() )
    {
      city->setName( vCityName.toString() );
    }

    city->funds().resolveIssue( FundIssue( city::Funds::donation, vm[ "funds" ].toInt() ) );

    Logger::warning( "GameLoaderMission: load city options ");
    city->setOption( PlayerCity::adviserEnabled, vm.get( lc_adviserEnabled, true ) );
    city->setOption( PlayerCity::fishPlaceEnabled, vm.get( lc_fishPlaceEnabled, true ) );

    GameDate::instance().init( vm[ "date" ].toDateTime() );

    VariantMap vm_events = vm[ "events" ].toMap();
    foreach( it, vm_events )
    {
      events::GameEventPtr e = events::PostponeEvent::create( it->first, it->second.toMap() );
      e->dispatch();
    }

    game.empire()->setCitiesAvailable( false );
    Logger::warning( "GameLoaderMission: load empire state" );
    game.empire()->load( vm[ "empire" ].toMap() );

    city::VictoryConditions targets;
    Variant winOptions = vm[ "win" ];
    Logger::warningIf( winOptions.isNull(), "GameLoaderMission: cannot load mission win options from file " + filename );

    targets.load( winOptions.toMap() );
    city->setVictoryConditions( targets );

    city::BuildOptions options;
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
    GameSettings::set( GameSettings::lastTranslation, Variant( missionName ) );

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

bool GameLoaderMission::isLoadableFileExtension( const std::string& filename )
{
  return vfs::Path( filename ).isMyExtension( ".mission" );
}

int GameLoaderMission::climateType(const std::string& filename) { return -1; }
std::string GameLoaderMission::restartFile() const { return _d->restartFile; }
