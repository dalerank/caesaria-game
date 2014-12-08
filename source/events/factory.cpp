// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#include "factory.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "random_damage.hpp"
#include "random_fire.hpp"
#include "showtutorialwindow.hpp"
#include "changebuildingoptions.hpp"
#include "changeempireoptions.hpp"
#include "cityindebt.hpp"
#include "enemy_attack.hpp"
#include "showinfobox.hpp"
#include "earthquake.hpp"
#include "start_work.hpp"
#include "distant_battle.hpp"
#include "trading_options.hpp"
#include "contaminated_water.hpp"
#include "changeemperor.hpp"
#include "random_animals.hpp"

namespace events
{

class GameEventFactory::Impl
{
public:
  typedef std::map< std::string, GameEventCreatorPtr > Creators;
  Creators creators;
};

GameEventPtr GameEventFactory::create( const std::string& name )
{
  GameEventFactory& inst = instance();
  Impl::Creators::iterator it = inst._d->creators.find( name );
  if( it != inst._d->creators.end() )
  {
    return it->second->create();
  }

  Logger::warning( "GameEventFactory: not found creator for event " + name );
  return GameEventPtr();
}

GameEventFactory& GameEventFactory::instance()
{
  static GameEventFactory inst;
  return inst;
}

void GameEventFactory::addCreator( const std::string& name, GameEventCreatorPtr creator )
{
  if( creator.isNull() )
    return;

  Impl::Creators::iterator it = _d->creators.find( name );

  if( it != _d->creators.end() )
  {
    Logger::warning( "GameEventFactory: Also have creator for event " + name );
    return;
  }

  _d->creators[ name ] = creator;
}

GameEventFactory::GameEventFactory() : _d( new Impl )
{
  addCreator<ShowTutorial>( "tutorial_window" );
  addCreator<ChangeBuildingOptions>( "building_options" );
  addCreator<ChangeEmpireOptions>( "empire_options" );
  addCreator<ChangeEmperor>( "change_emperor" );
  addCreator<RandomDamage>( "random_collapse" );
  addCreator<RandomFire>( "random_fire" );
  addCreator<CityIndebt>( "city_indebt" );
  addCreator<EnemyAttack>( "enemy_attack" );
  addCreator<ShowInfobox>( "messagebox" );
  addCreator<EarthQuake>( "earthquake" );
  addCreator<StartWork>( "start_work" );
  addCreator<DistantBattle>( "distant_battle" );
  addCreator<ChangeTradingOptions>( "trading_options" );
  addCreator<ContaminatedWater>( "contaminated_water" );
  addCreator<RandomAnimals>( "random_animals" );
}

}//end namespace events
