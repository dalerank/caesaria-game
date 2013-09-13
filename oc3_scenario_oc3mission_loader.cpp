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

#include "oc3_scenario_oc3mission_loader.hpp"
#include "oc3_tile.hpp"
#include "oc3_exception.hpp"
#include "oc3_picture.hpp"
#include "oc3_positioni.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_game.hpp"
#include "oc3_saveadapter.hpp"
#include "oc3_scenario_loader.hpp"
#include "oc3_win_targets.hpp"
#include "oc3_city_build_options.hpp"
#include "oc3_building_data.hpp"
#include "oc3_cityfunds.hpp"
#include "oc3_empire.hpp"
#include "oc3_city.hpp"
#include "oc3_game_settings.hpp"

class GameMissionLoader::Impl
{
public:
  static const int currentVesion = 1;
};

GameMissionLoader::GameMissionLoader()
{

}

bool GameMissionLoader::load( const std::string& filename, Game& game )
{
  VariantMap vm = SaveAdapter::load( filename );
  
  if( Impl::currentVesion == vm[ "version" ].toInt() )
  {
    std::string mapToLoad = vm[ "map" ].toString();

    GameLoader mapLoader;
    mapLoader.load( GameSettings::rcpath( mapToLoad ), game );

    CityPtr city = game.getCity();
    city->getFunds().resolveIssue( FundIssue( CityFunds::donation, vm[ "funds" ].toInt() ) );

    game.getEmpire()->setCitiesAvailable( false );

    game.getEmpire()->load( vm[ "empire" ].toMap() );

    CityWinTargets targets;
    targets.load( vm[ "win" ].toMap() );
    game.setWinTargets( targets );

    CityBuildOptions options;
    options.load( vm[ "buildoptions" ].toMap() );
    city->setBuildOptions( options  );

    return true;
  }
 
  return false;
}

bool GameMissionLoader::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 11 ) == ".oc3mission";
}
