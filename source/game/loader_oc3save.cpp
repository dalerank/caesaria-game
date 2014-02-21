// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
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
#include "divinity.hpp"
#include "events/dispatcher.hpp"
#include "core/locale.hpp"
#include "settings.hpp"

static const int currentVesion = 1;

bool GameLoaderOc3::load( const std::string& filename, Game& game )
{
  VariantMap vm = SaveAdapter::load( filename );
  
  if( currentVesion == (int)vm[ "version" ] )
  {
    VariantMap scenario_vm = vm[ "scenario" ].toMap();
    game.setTimeMultiplier( (int)vm[ "timemultiplier"] );

    GameDate::init( scenario_vm[ "date" ].toDateTime() );
    events::Dispatcher::instance().load( scenario_vm[ "events" ].toMap() );

    Variant lastTr = scenario_vm[ "translation" ];
    Locale::addTranslation( lastTr.toString() );
    GameSettings::set( GameSettings::lastTranslation, lastTr );

    game.getPlayer()->load( vm[ "player" ].toMap() );
    game.getCity()->load( vm[ "city" ].toMap() );

    game.getEmpire()->load( vm[ "empire" ].toMap() );

    DivinePantheon::getInstance().load( vm[ "pantheon" ].toMap() );

    return true;
  }
 
  return false;
}

bool GameLoaderOc3::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 8 ) == ".oc3save";
}
