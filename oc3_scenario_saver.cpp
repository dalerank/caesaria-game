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

#include "oc3_scenario_saver.hpp"
#include "oc3_variant.hpp"
#include "oc3_saveadapter.hpp"
#include "oc3_scenario.hpp"
#include "oc3_player.hpp"

class ScenarioSaver::Impl
{
public:
  const Scenario& scenario;

  Impl( const Scenario& s ) : scenario( s )
  {

  }
};

ScenarioSaver::ScenarioSaver( const Scenario& scenario )
  : _d( new Impl( scenario ) )
{
}


void ScenarioSaver::save(const std::string& filename)
{
  VariantMap vm;
  vm[ "version" ] = Variant( 1 );

  VariantMap vm_scenario;
  _d->scenario.save( vm_scenario );
  vm[ "scenario" ] = vm_scenario;

  VariantMap plm;
  _d->scenario.getPlayer().save( plm );
  vm[ "player" ] = plm;

  VariantMap vm_city;
  _d->scenario.getCity()->save( vm_city );
  vm[ "city" ] = vm_city;


  SaveAdapter::save( vm, filename );
}

ScenarioSaver::~ScenarioSaver()
{

}