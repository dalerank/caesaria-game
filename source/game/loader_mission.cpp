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

#include "loader_mission.hpp"
#include "gfx/tile.hpp"
#include "core/exception.hpp"
#include "gfx/picture.hpp"
#include "core/position.hpp"
#include "objects/objects_factory.hpp"
#include "game.hpp"
#include "core/saveadapter.hpp"
#include "loader.hpp"
#include "city/win_targets.hpp"
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

using namespace religion;

class GameLoaderMission::Impl
{
public:
  static const int currentVesion = 1;
};

GameLoaderMission::GameLoaderMission()
{
}

bool GameLoaderMission::load( const std::string& filename, Game& game )
{
  VariantMap vm = SaveAdapter::load( filename );
  
  if( Impl::currentVesion == vm[ "version" ].toInt() )
  {
    std::string mapToLoad = vm[ "map" ].toString();

    GameLoader mapLoader;
    mapLoader.load( GameSettings::rcpath( mapToLoad ), game );

    PlayerCityPtr city = game.city();
    city->funds().resolveIssue( FundIssue( city::Funds::donation, vm[ "funds" ].toInt() ) );

    GameDate::instance().init( vm[ "date" ].toDateTime() );

    VariantMap vm_events = vm[ "events" ].toMap();
    foreach( it, vm_events )
    {
      events::GameEventPtr e = events::PostponeEvent::create( it->first, it->second.toMap() );
      e->dispatch();
    }

    game.empire()->setCitiesAvailable( false );

    game.empire()->load( vm[ "empire" ].toMap() );

    city::WinTargets targets;
    Variant winOptions = vm[ "win" ];
    Logger::warningIf( winOptions.isNull(), "GameLoaderMission: cannot load mission win options from file " + filename );

    targets.load( winOptions.toMap() );
    city->setWinTargets( targets );

    city::BuildOptions options;
    options.load( vm[ "buildoptions" ].toMap() );
    city->setBuildOptions( options  );

    game.empire()->emperor().updateRelation( city->getName(), 50 );

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
