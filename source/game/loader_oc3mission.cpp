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

#include "loader_oc3mission.hpp"
#include "gfx/tile.hpp"
#include "core/exception.hpp"
#include "gfx/picture.hpp"
#include "core/position.hpp"
#include "tileoverlay_factory.hpp"
#include "game.hpp"
#include "core/saveadapter.hpp"
#include "loader.hpp"
#include "win_targets.hpp"
#include "build_options.hpp"
#include "building/metadata.hpp"
#include "cityfunds.hpp"
#include "empire.hpp"
#include "city.hpp"
#include "settings.hpp"

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

    CityPtr city = game.getCity();
    city->getFunds().resolveIssue( FundIssue( CityFunds::donation, vm[ "funds" ].toInt() ) );

    game.getEmpire()->setCitiesAvailable( false );

    game.getEmpire()->load( vm[ "empire" ].toMap() );

    CityWinTargets targets;
    targets.load( vm[ "win" ].toMap() );
    city->setWinTargets( targets );

    CityBuildOptions options;
    options.load( vm[ "buildoptions" ].toMap() );
    city->setBuildOptions( options  );

    return true;
  }
 
  return false;
}

bool GameLoaderMission::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 11 ) == ".oc3mission";
}
