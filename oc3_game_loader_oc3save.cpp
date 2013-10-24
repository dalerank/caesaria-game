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
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_game_loader_oc3save.hpp"
#include "oc3_tile.hpp"
#include "oc3_exception.hpp"
#include "core/position.hpp"
#include "oc3_tileoverlay_factory.hpp"
#include "oc3_game.hpp"
#include "oc3_saveadapter.hpp"
#include "oc3_player.hpp"
#include "oc3_city.hpp"
#include "oc3_gamedate.hpp"
#include "oc3_empire.hpp"

static const int currentVesion = 1;

bool GameLoaderOc3::load( const std::string& filename, Game& game )
{
  VariantMap vm = SaveAdapter::load( filename );
  
  if( currentVesion == (int)vm[ "version" ] )
  {
    VariantMap scenario_vm = vm[ "scenario" ].toMap();

    GameDate::init( scenario_vm[ "date" ].toDateTime() );

    game.getPlayer()->load( vm[ "player" ].toMap() );
    game.getCity()->load( vm[ "city" ].toMap() );

    game.getEmpire()->load( vm[ "empire" ].toMap() );

    return true;
  }
 
  return false;
}

bool GameLoaderOc3::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 8 ) == ".oc3save";
}
