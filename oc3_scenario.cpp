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
#include "oc3_city_build_options.hpp"
#include "oc3_win_targets.hpp"
#include "oc3_player.hpp"
#include "oc3_city.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_empire.hpp"
#include "oc3_app_config.hpp"
#include "oc3_scenario_loader.hpp"
#include "oc3_astarpathfinding.hpp"
#include "oc3_cityfunds.hpp"
#include "oc3_gamedate.hpp"
#include "oc3_scenario_event_resolver.hpp"

class Scenario::Impl
{
public:
  float timeMultiplier;

  CityPtr city;
  Player player;
  std::string description;
  CityWinTargets targets;
  EmpirePtr empire;
  double time, saveTime;
  ScenarioEventResolverPtr eventResolver;

  void resolveMonthChange( const DateTime& time )
  {
    city->getFunds().resolveIssue( FundIssue( CityFunds::playerSalary, -player.getSalary() ) );
    player.appendMoney( player.getSalary() );
  }
};

Scenario& Scenario::instance()
{
  static Scenario inst;
  return inst;
}

Scenario::Scenario() : _d( new Impl )
{
  _d->description = "";
  _d->time = 0;
  _d->saveTime = 0;
  _d->timeMultiplier = 100;
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

  _d->empire->initPlayerCity( _d->city.as<EmpireCity>() );

  LandOverlays llo = _d->city->getOverlayList();
  foreach( LandOverlayPtr overlay, llo )
  {
    ConstructionPtr construction = overlay.as<Construction>();
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

void Scenario::addEvent(ScenarioEventPtr event)
{
  _d->eventResolver->addEvent( event );
}

Scenario::~Scenario()
{

}

void Scenario::reset()
{
  _d->empire = Empire::create();
  _d->city = City::create( _d->empire );
  _d->eventResolver = ScenarioEventResolver::create( _d->city );

  GameDate::instance().onMonthChanged().disconnectAll();

  CONNECT( &GameDate::instance(), onMonthChanged(), _d.data(), Impl::resolveMonthChange );
}

void Scenario::changeTimeMultiplier(int percent)
{
  _d->timeMultiplier = math::clamp<int>( _d->timeMultiplier + percent, 10, 300 );
}

Player& Scenario::getPlayer() const
{
  return _d->player;
}

EmpirePtr Scenario::getEmpire() const
{
  return _d->empire;
}

unsigned int Scenario::timeStep()
{
  _d->time += _d->timeMultiplier / 100.f;

  while( (_d->time - _d->saveTime) > 1 )
  {
    _d->empire->timeStep( _d->time );

    GameDate::timeStep( _d->time );

    _d->saveTime += 1;

    _d->eventResolver->update( _d->time );
  }  

  return (unsigned int)_d->saveTime;
}
