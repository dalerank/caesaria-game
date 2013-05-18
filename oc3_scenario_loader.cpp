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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "oc3_scenario_loader.hpp"

#include "oc3_scenario_map_loader.hpp"
//#include "oc3_scenario_sav_loader.hpp"
#include "oc3_scenario_oc3save_loader.hpp"
#include "oc3_scenario_load_finalizer.hpp"

#include <vector>

typedef SmartPtr< ScenarioAbstractLoader > ScenarioAbstractLoaderPtr;

class ScenarioLoader::Impl
{
public:
  typedef std::vector< ScenarioAbstractLoaderPtr > Loaders;
  typedef Loaders::iterator LoaderIterator;
  Loaders loaders;

  void initLoaders();
};

ScenarioLoader::ScenarioLoader() : _d( new Impl )
{
  _d->initLoaders();
}

ScenarioLoader& ScenarioLoader::getInstance()
{
  static ScenarioLoader inst;
  return inst;
}

void ScenarioLoader::Impl::initLoaders()
{
  loaders.push_back( ScenarioAbstractLoaderPtr( new ScenarioMapLoader() ) );
  //loaders.push_back( ScenarioAbstractLoaderPtr( new ScenarioSavLoader() ) );
  loaders.push_back( ScenarioAbstractLoaderPtr( new ScenarioOc3SaveLoader() ) );
}

ScenarioLoader::~ScenarioLoader(void)
{
}

bool ScenarioLoader::load( const std::string& filename, Scenario& scenario )
{
  // try to load file based on file extension
  Impl::LoaderIterator it = _d->loaders.begin();
  for( ; it != _d->loaders.end(); ++it)
  {
    if( (*it)->isLoadableFileExtension( filename ) /*||
        (*it)->isLoadableFileFormat(file) */ )
    {
      bool loadok = (*it)->load( filename, scenario );
      
      if( loadok )
      {
        ScenarioLoadFinalizer finalizer( scenario );
        finalizer.check();
      }

      return loadok;
    }
  }

  return false; // failed to load
}
