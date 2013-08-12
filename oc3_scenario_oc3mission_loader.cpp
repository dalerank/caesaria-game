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
#include "oc3_scenario.hpp"
#include "oc3_saveadapter.hpp"
#include "oc3_scenario_loader.hpp"
#include "oc3_win_targets.hpp"
#include "oc3_city_build_options.hpp"
#include "oc3_building_data.hpp"
#include "oc3_cityfunds.hpp"
#include "oc3_empire.hpp"
#include "oc3_city.hpp"

class ScenarioOc3MissionLoader::Impl
{
public:
  static const int currentVesion = 1;
};

ScenarioOc3MissionLoader::ScenarioOc3MissionLoader()
{

}

bool ScenarioOc3MissionLoader::load( const std::string& filename, Scenario& oScenario )
{
  VariantMap vm = SaveAdapter::load( filename );
  
  if( Impl::currentVesion == vm[ "version" ].toInt() )
  {
    std::string mapToLoad = vm[ "map" ].toString();

    ScenarioLoader::getInstance().load( mapToLoad, oScenario );
    CityPtr city = oScenario.getCity();
    city->getFunds().resolveIssue( FundIssue( CityFunds::donation, vm[ "funds" ].toInt() ) );

    oScenario.getEmpire()->load( vm[ "empire" ].toMap() );

    oScenario.getWinTargets().load( vm[ "targets" ].toMap() );

    CityBuildOptions& boptions = city->getBuildOptions();
    VariantList saveOptions = vm[ "buildoptions" ].toList();
    boptions.clear();
    boptions.setIndustryAvaible( BM_FARM, false );
    boptions.setIndustryAvaible( BM_RAW_MATERIAL, false );
    boptions.setIndustryAvaible( BM_FACTORY, false );

    for( VariantList::iterator it = saveOptions.begin(); it != saveOptions.end(); it++ )
    {
      BuildingType btype = BuildingDataHolder::getType( (*it).toString() );
      boptions.setBuildingAvailble( btype, true );
    }

    return true;
  }
 
  return false;
}

bool ScenarioOc3MissionLoader::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 11 ) == ".oc3mission";
}
