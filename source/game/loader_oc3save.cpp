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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "loader_oc3save.hpp"
#include "gfx/tile.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "objects/objects_factory.hpp"
#include "game.hpp"
#include "core/saveadapter.hpp"
#include "player.hpp"
#include "core/variant_map.hpp"
#include "city/city.hpp"
#include "gamedate.hpp"
#include "world/empire.hpp"
#include "religion/pantheon.hpp"
#include "events/dispatcher.hpp"
#include "core/locale.hpp"
#include "settings.hpp"
#include "core/logger.hpp"
#include "saver.hpp"

namespace game
{

namespace loader
{

static const int currentVesion = 1;

class OC3::Impl
{
public:
  std::string restartFile;
};

bool OC3::load( const std::string& filename, Game& game )
{
  Logger::warning( "GameLoaderOc3: start loading from " + filename );
  VariantMap vm = config::load( filename );
  if( vm.empty() )
  {
    Logger::warning( "GameLoaderOc3: empty file " + filename );
    return false;
  }
  
  int fileVersion = vm[ SaverOptions::version ];
  if( currentVesion == fileVersion )
  {      
    _d->restartFile = vm[ SaverOptions::restartFile ].toString();

    VariantMap scenario_vm = vm[ "scenario" ].toMap();
    game.setTimeMultiplier( (int)vm[ "timemultiplier"] );

    game::Date::instance().init( scenario_vm[ "date" ].toDateTime() );
    events::Dispatcher::instance().load( scenario_vm[ "events" ].toMap() );

    Variant lastTr = scenario_vm[ "translation" ];
    Locale::addTranslation( lastTr.toString() );
    SETTINGS_SET_VALUE( lastTranslation, lastTr );

    game.player()->load( vm[ "player" ].toMap() );
    game.city()->load( vm[ "city" ].toMap() );

    game.empire()->load( vm[ "empire" ].toMap() );

    religion::rome::Pantheon::instance().load( vm[ "pantheon" ].toMap() );

    return true;
  }

  Logger::warning( "GameLoaderOc3: unsupported version %d", fileVersion );
  return false;
}

int OC3::climateType(const std::string& filename)
{
  Logger::warning( "GameLoaderOc3: check climate type" + filename );
  VariantMap vm = config::load( filename );
  VariantMap scenario_vm = vm[ "scenario" ].toMap();

  return scenario_vm.get( "climate", -1 );
}

bool OC3::isLoadableFileExtension( const std::string& filename )
{
  return vfs::Path( filename ).isMyExtension( ".oc3save" );
}

std::string OC3::restartFile() const { return _d->restartFile; }

OC3::OC3() : _d( new Impl )
{

}

}//end namespace loader

}//end namespace game
