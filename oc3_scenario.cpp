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

#include "oc3_scenario.hpp"
#include "oc3_exception.hpp"
#include "oc3_variant.hpp"
#include "oc3_scenario_oc3save_saver.hpp"
#include "oc3_build_options.hpp"
#include "oc3_win_targets.hpp"
#include "oc3_player.hpp"
#include "oc3_city.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_empire.hpp"
#include "oc3_app_config.hpp"
#include "oc3_scenario_loader.hpp"
#include "oc3_astarpathfinding.hpp"

class Scenario::Impl
{
public:
  CityPtr city;
  Player player;
  std::string description;
  CityWinTargets targets;
  EmpirePtr empire;
};

Scenario& Scenario::instance()
{
  static Scenario inst;
  return inst;
}

Scenario::Scenario() : _d( new Impl )
{
  _d->description = "";
}

CityPtr Scenario::getCity()
{
  return _d->city;
}

const CityPtr Scenario::getCity() const
{
  return _d->city;
}

std::string Scenario::getDescription() const
{
  return _d->description;
}

bool Scenario::save( const io::FilePath& filename ) const
{  
  ScenarioOc3Saver saver( *this );
  saver.save( filename );
  return true;
}

bool Scenario::load( const io::FilePath& filename )
{
  StringHelper::debug( 0xff, "Load game begin" );

  _d->empire->initialize( AppConfig::rcpath( AppConfig::citiesModel ) );

  bool loadok = ScenarioLoader::getInstance().load( filename, *this);   

  if( !loadok )
  {
    StringHelper::debug( 0xff, "LOADING ERROR: can't load game from %s", filename.toString().c_str() );
    return false;
  }  

  LandOverlays llo = _d->city->getOverlayList();
  for ( LandOverlays::iterator itLLO = llo.begin(); itLLO!=llo.end(); ++itLLO)
  {
    ConstructionPtr construction = (*itLLO).as<Construction>();
    if( construction.isValid() )
    {
      construction->computeAccessRoads();
    }
  }

  Pathfinder::getInstance().update( _d->city->getTilemap() );  

  StringHelper::debug( 0xff, "Load game end" );
  return true;
}

CityWinTargets& Scenario::getWinTargets()
{
  return _d->targets;
}

Scenario::~Scenario()
{

}

void Scenario::reset()
{
  _d->city = City::create();
  _d->empire = Empire::create();
}

Player& Scenario::getPlayer() const
{
  return _d->player;
}

EmpirePtr Scenario::getEmpire() const
{
  return _d->empire;
}