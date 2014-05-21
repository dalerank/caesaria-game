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

#include "loader_oc3save.hpp"
#include "gfx/tile.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "objects/objects_factory.hpp"
#include "game.hpp"
#include "core/saveadapter.hpp"
#include "player.hpp"
#include "city/city.hpp"
#include "gamedate.hpp"
#include "world/empire.hpp"
#include "religion/pantheon.hpp"
#include "events/dispatcher.hpp"
#include "core/locale.hpp"
#include "settings.hpp"
#include "core/logger.hpp"

static const int currentVesion = 1;

bool GameLoaderOc3::load( const std::string& filename, Game& game )
{
  Logger::warning( "GameLoaderOc3: start loading from " + filename );
  VariantMap vm = SaveAdapter::load( filename );
  if( vm.empty() )
  {
    Logger::warning( "GameLoaderOc3: empty file " + filename );
    return false;
  }
  
  int fileVersion = vm[ "version" ];
  if( currentVesion == fileVersion )
  {      
    VariantMap scenario_vm = vm[ "scenario" ].toMap();
    game.setTimeMultiplier( (int)vm[ "timemultiplier"] );

    GameDate::instance().init( scenario_vm[ "date" ].toDateTime() );
    events::Dispatcher::instance().load( scenario_vm[ "events" ].toMap() );

    Variant lastTr = scenario_vm[ "translation" ];
    Locale::addTranslation( lastTr.toString() );
    GameSettings::set( GameSettings::lastTranslation, lastTr );
    GameSettings::set( GameSettings::adviserEnabled, scenario_vm.get( "adviserEnabled" ) );

    game.player()->load( vm[ "player" ].toMap() );
    game.city()->load( vm[ "city" ].toMap() );

    game.empire()->load( vm[ "empire" ].toMap() );

    religion::rome::Pantheon::instance().load( vm[ "pantheon" ].toMap() );

    return true;
  }

  Logger::warning( "GameLoaderOc3: unsupported version %d", fileVersion );
  return false;
}

int GameLoaderOc3::getClimateType(const std::string& filename)
{
  Logger::warning( "GameLoaderOc3: check climate type" + filename );
  VariantMap vm = SaveAdapter::load( filename );
  VariantMap scenario_vm = vm[ "scenario" ].toMap();

  return scenario_vm.get( "climate", -1 );
}

bool GameLoaderOc3::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 8 ) == ".oc3save";
}
