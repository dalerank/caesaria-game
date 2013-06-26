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

#include "oc3_scenario_oc3save_loader.hpp"
#include "oc3_tile.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_scenario.hpp"
#include "oc3_saveadapter.hpp"

class ScenarioOc3SaveLoader::Impl
{
public:
  static const int currentVesion = 1;
};

ScenarioOc3SaveLoader::ScenarioOc3SaveLoader()
{

}

bool ScenarioOc3SaveLoader::load( const std::string& filename, Scenario& oScenario )
{
  VariantMap vm = SaveAdapter::load( filename );
  
  if( Impl::currentVesion == vm[ "version" ].toInt() )
  {
    oScenario.load( vm[ "scenario" ].toMap() );
    oScenario.getCity().load( vm[ "city" ].toMap() );

    return true;
  }
 
  return false;
}

bool ScenarioOc3SaveLoader::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 8 ) == ".oc3save";
}
