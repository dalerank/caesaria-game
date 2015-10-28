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
//
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "migration.hpp"
#include "objects/construction.hpp"
#include "statistic.hpp"
#include "city/cityservice_military.hpp"
#include "core/safetycast.hpp"
#include "gfx/tilemap.hpp"
#include "walker/immigrant.hpp"
#include "core/position.hpp"
#include "objects/road.hpp"
#include "objects/house.hpp"
#include "gfx/tile.hpp"
#include "game/funds.hpp"
#include "core/variant_map.hpp"
#include "objects/constants.hpp"
#include "game/settings.hpp"
#include "world/empire.hpp"
#include "game/gamedate.hpp"
#include "statistic.hpp"
#include "events/warningmessage.hpp"
#include "cityservice_info.hpp"
#include "core/logger.hpp"
#include "core/saveadapter.hpp"
#include "cityservice_factory.hpp"
#include "config.hpp"
#include "city/states.hpp"

using namespace gfx;
using namespace config;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Migration,migration)

namespace {
const int maxIndesirability = 100;
const int noWorklessAward = 10;
const int shacksPenalty = 10;
const int warBlockedMigration = 50;
const int cityUnderAttackPenalty = 2;
const int defaultEmIndesirability = 50;
static SimpleLogger LOG_MIGRATION("MigrationService");
}

class Migration::Impl
{
public:
  int lastMonthMigration;
  int updateTickInterval;
  int emigrantsIndesirability;
  int lastMonthComing;
  int lastMonthLeaving;
  int chanceCounter;
  int worklessMinInfluence;
  int checkRange;
  DateTime lastUpdate;
  bool haveTroubles;

  float getMigrationKoeff(PlayerCityPtr city);
  Info::Parameters lastMonthParams(PlayerCityPtr city);
  void createMigrationToCity(PlayerCityPtr city);
  void createMigrationFromCity(PlayerCityPtr city);
  unsigned int calcVacantHouse( PlayerCityPtr city );
  bool isPoorHousing( int shacks, int houses );
};

Migration::Migration( PlayerCityPtr city )
  : Srvc( city, defaultName() ), _d( new Impl )
{  
  LOG_MIGRATION.warn( "Initialize" );

  VariantMap options = config::load( ":/migration.model" );
  VARIANT_LOAD_ANYDEF_D( _d, checkRange, DateTime::daysInWeek, options )
  VARIANT_LOAD_ANYDEF_D( _d, worklessMinInfluence, 0, options )

  _d->lastMonthMigration = 0;
  _d->lastMonthComing = 0;
  _d->lastMonthLeaving = 0;
  _d->chanceCounter = 1;
  _d->haveTroubles = false;
  _d->lastUpdate = game::Date::current();
  _d->updateTickInterval = game::Date::days2ticks( 7 );
  _d->emigrantsIndesirability = 0;
}

void Migration::timeStep( const unsigned int time )
{
  if( game::Date::isMonthChanged() )
  {
    std::string trouble = reason();
    if( haveTroubles() )
    {
      events::dispatch<events::WarningMessage>(trouble, 1);
    }
  }

  if( time % _d->updateTickInterval != 1 )
    return;

  LOG_MIGRATION.info( "Calculation started" );
  const int worklessCitizenAway = SETTINGS_VALUE( worklessCitizenAway );

  float migrationKoeff = _d->getMigrationKoeff( _city() );
  Info::Parameters params = _d->lastMonthParams( _city() );
  LOG_MIGRATION.info( "Current migration factor is {}", migrationKoeff );

  _d->emigrantsIndesirability = defaultEmIndesirability; //base undesirability value
  float emDesKoeff = math::clamp<float>( (float)SETTINGS_VALUE( emigrantSalaryKoeff ), 1.f, 99.f );

  //if salary in city more then empire people more effectively go to our city
  const int diffSalary = _city()->empire()->workerSalary() - _city()->treasury().workerSalary();
  int diffSalaryInfluence = diffSalary * emDesKoeff;

  //emigrant like when lot of food stock int city
  const int minMonthWithFood = SETTINGS_VALUE( minMonthWithFood );
  int foodStackInfluence = ( params[ Info::monthWithFood ] < minMonthWithFood
                                   ? ((minMonthWithFood - params[ Info::monthWithFood ]) * 3)
                                   : -params[ Info::monthWithFood ] );

  int sentimentInfluence = (params[ Info::sentiment ] - 50) / 5;

  //emigrant need workplaces
  const int& curWorklessValue = params[ Info::workless ];
  int worklessInfluence = curWorklessValue == 0
                          ? -noWorklessAward
                          : (curWorklessValue * (curWorklessValue < worklessCitizenAway ? 1 : 2));

  int taxLevelInfluence = ( params[ Info::tax ] > migration::normalTax
                            ? params[ Info::tax ] * 2
                            : (migration::normalTax-params[ Info::tax ]) );

  const int& monthWithourWar = _city()->states().age > 1
                                  ? params[ Info::monthWtWar ]
                                  : DateTime::monthsInYear;

  int warInfluence = ( monthWithourWar < DateTime::monthsInYear
                          ? (DateTime::monthsInYear - monthWithourWar) * 5
                          : -std::min( monthWithourWar, 10 ) );

  warInfluence += params[ Info::milthreat ];

  int slumsInfluence = ( _d->isPoorHousing( params[ Info::slumNumber ], params[ Info::houseNumber ] ) ? shacksPenalty*2 : 0);
  int shacksInfluence = ( _d->isPoorHousing( params[ Info::shackNumber ], params[ Info::houseNumber ] ) ? shacksPenalty : 0 );

  if( _d->worklessMinInfluence > 0 )
  {
    _d->emigrantsIndesirability += worklessInfluence * std::min<int>( _city()->states().population, 150 ) / _d->worklessMinInfluence;
  }
  else
  {
    _d->emigrantsIndesirability += worklessInfluence;
  }

  _d->emigrantsIndesirability += diffSalaryInfluence;
  _d->emigrantsIndesirability += foodStackInfluence;
  _d->emigrantsIndesirability += params[ Info::crimeLevel ];
  _d->emigrantsIndesirability += taxLevelInfluence;
  _d->emigrantsIndesirability += warInfluence;
  _d->emigrantsIndesirability += shacksInfluence;
  _d->emigrantsIndesirability += slumsInfluence;
  _d->emigrantsIndesirability += sentimentInfluence;
  _d->emigrantsIndesirability += params[ Info::blackHouses ];

  _d->emigrantsIndesirability *= migrationKoeff;

  LOG_MIGRATION.info( "Current undesirability is {}", _d->emigrantsIndesirability );
  if( warInfluence > warBlockedMigration )
  {
    LOG_MIGRATION.info( "Enemies in city: migration stopped" );
    return;
  }

  MilitaryPtr mil = _city()->statistic().services.find<Military>();

  if( mil.isValid() )
  {
    bool cityUnderAttack = mil->isUnderAttack();

    if( cityUnderAttack )
      _d->emigrantsIndesirability *= cityUnderAttackPenalty;
  }  

  if( _d->lastUpdate.monthsTo( game::Date::current() ) > 0 )
  {
    _d->lastUpdate = game::Date::current();
    _d->lastMonthMigration = _d->lastMonthComing - _d->lastMonthLeaving;
    _d->lastMonthComing = 0;
    _d->lastMonthLeaving = 0;

    LOG_MIGRATION.info( "Current workless={} undesrbl={}",
                     curWorklessValue * migrationKoeff,
                     _d->emigrantsIndesirability * migrationKoeff );
  }

  if( curWorklessValue * migrationKoeff > worklessCitizenAway
      || _d->emigrantsIndesirability * migrationKoeff > maxIndesirability )
  {

  }
  else
  {
    _d->chanceCounter++;
    float variance = utils::eventProbability( (maxIndesirability-_d->emigrantsIndesirability)/100.f,
                                              _d->chanceCounter, _d->emigrantsIndesirability );
    if( variance >= 1)
    {
      _d->createMigrationToCity( _city() );
      _d->chanceCounter = 0;
    }
  }

  _d->updateTickInterval = math::random( game::Date::days2ticks( _d->checkRange ) ) + 10;
  LOG_MIGRATION.info( "Calculation finished." );
}

std::string Migration::reason() const
{
  unsigned int vacantHouse = _d->calcVacantHouse( _city() );
  if( vacantHouse == 0 )
    return "##migration_lack_empty_house##";

  StringArray troubles;
  if( _d->emigrantsIndesirability > defaultEmIndesirability )
  {
    Info::Parameters params = _d->lastMonthParams( _city() );
    if( params[ Info::monthWtWar ] < DateTime::monthsInYear )
    {
      troubles << "##migration_war_deterring##";
    }

    if( params[ Info::blackHouses ] > 0 )
    {
      troubles << "##migration_lack_indesrb_houses##";
    }

    if( params[ Info::monthWithFood ] < (int)SETTINGS_VALUE( minMonthWithFood ) )
    {
      if( params[ Info::monthWithFood ] == 0 ) { troubles << "##migration_empty_granary##"; }
      else if( params[ Info::monthWithFood ] < 2 ) { troubles << "##migration_low_food_stocks##"; }
      else { troubles << "##migration_lessfood_granary##"; }
    }

    if( params[ Info::workless ] > (int)SETTINGS_VALUE( worklessCitizenAway ) )
    {
      troubles << "##migration_people_away##";
    }
    else
    {
      int value = params[ Info::workless ];
      if( value > 25 ) { troubles << "##migration_broke_workless##"; }
      else if( value > 17  ) { troubles << "##migration_high_lack_workless##"; }
      else if( value > 10 ) { troubles << "##migration_middle_lack_workless##"; }
      else if( value > 5 ) { troubles << "##migration_lack_workless##"; }
    }

    int diffWages = params[ Info::romeWages ] - params[ Info::cityWages ];
    if( diffWages > 5 ) { troubles << "##low_wage_broke_migration##"; }
    else if( diffWages > 2 ) { troubles << "##low_wage_midlle_migration##"; }
    else if( diffWages > 1 ) { troubles <<  "##low_wage_lack_migration##"; }

    if( params[ Info::crimeLevel ] > 25 ) { troubles << "##migration_lack_crime##"; }

    if( params[ Info::tax ] > migration::highTax )                   { troubles << "##migration_broke_tax##";  }
    else if( params[ Info::tax ] > migration::uncomfortableTax ) { troubles <<  "##migration_middle_lack_tax##"; }
    else if( params[ Info::tax ] > migration::normalTax )        { troubles << "##migration_lack_tax##"; }

    if( params[ Info::sentiment ] < 50 ) { troubles << "##poor_city_mood_lack_migration##";}

    if( _d->isPoorHousing( params[ Info::shackNumber ], params[ Info::houseNumber ] ) ) { troubles << "##poor_housing_discourages_migration##";}
  }

  _d->haveTroubles = !troubles.empty();
  return troubles.empty()
           ? "##migration_peoples_arrived_in_city##"
           : troubles.random();
}

std::string Migration::leaveCityReason() const
{
  if( lastMonthMigration() < 0 )
  {
    Info::Parameters lastParams = _d->lastMonthParams( _city() );
    if( lastParams[ Info::tax ] > migration::insaneTax )
      return "##people_leave_city_insane_tax##";

    if( lastParams[ Info::payDiff ] > migration::uncomfortableWageDiff )
      return "##people_leave_city_low_wage##";

    if( lastParams[ Info::workless ] > workless::high )
      return "##migration_people_away##";

    return "##people_leave_city_some##";
  }

  return "";
}

std::string Migration::defaultName() { return CAESARIA_STR_EXT(Migration); }

bool Migration::haveTroubles() const{ return _d->haveTroubles; }
int Migration::lastMonthMigration() const { return _d->lastMonthMigration; }

VariantMap Migration::save() const
{
  VariantMap ret;

  VARIANT_SAVE_ANY_D( ret, _d, lastUpdate )
  VARIANT_SAVE_ANY_D( ret, _d, lastMonthMigration )
  VARIANT_SAVE_ANY_D( ret, _d, lastMonthComing )
  VARIANT_SAVE_ANY_D( ret, _d, lastMonthLeaving )
  VARIANT_SAVE_ANY_D( ret, _d, haveTroubles )

  return ret;
}

void Migration::load(const VariantMap& stream)
{
  VARIANT_LOAD_TIME_D( _d, lastUpdate, stream )
  VARIANT_LOAD_ANY_D( _d, lastMonthMigration, stream )
  VARIANT_LOAD_ANY_D( _d, lastMonthComing, stream )
  VARIANT_LOAD_ANY_D( _d, lastMonthLeaving, stream )
  VARIANT_LOAD_ANY_D( _d, haveTroubles, stream )
}

void Migration::citizenLeaveCity(WalkerPtr walker)
{
  EmigrantPtr emigrant = ptr_cast<Emigrant>( walker );
  if( emigrant.isValid() )
  {
    _d->lastMonthLeaving += emigrant->peoples().count();
  }
}

unsigned int Migration::Impl::calcVacantHouse( PlayerCityPtr city )
{
  unsigned int vh = 0;
  HouseList houses = city->statistic().houses.find();
  for( auto house : houses )
  {
    if( house->roadside().size() > 0 && house->state( pr::settleLock ) == 0 )
    {
      vh += math::clamp<int>( house->capacity() - house->habitants().count(), 0, 0xff );
    }
  }

  return vh;
}

bool Migration::Impl::isPoorHousing(int shacks, int houses)
{
  return math::percentage( shacks, houses ) > 30;
}

float Migration::Impl::getMigrationKoeff( PlayerCityPtr city )
{
  return ( std::min<float>( city->states().population, 300 ) / 300.f );
}

Info::Parameters Migration::Impl::lastMonthParams( PlayerCityPtr city )
{
  InfoPtr info = city->statistic().services.find<Info>();

  Info::Parameters params;
  if( info.isValid() )
  {
    params = info->lastParams();
  }

  return params;
}

void Migration::Impl::createMigrationToCity( PlayerCityPtr city )
{
  unsigned int vh = calcVacantHouse( city );
  if( vh == 0 )
  {
    return;
  }

  auto migrants = city->walkers().select<Emigrant>();

  if( vh <= migrants.size() * 5 )
  {
    return;
  }

  Tile& roadTile = city->tilemap().at( city->getBorderInfo( PlayerCity::roadEntry ).epos() );

  ImmigrantPtr emigrant = Walker::create<Immigrant>( city );

  if( emigrant.isValid() )
  {
    bool success = emigrant->send2city( roadTile );
    emigrant->setSpeedMultiplier( 0.8f + math::random( 40 ) / 100.f );

    if( success )
      lastMonthComing += emigrant->peoples().count();
  }
}

void Migration::Impl::createMigrationFromCity( PlayerCityPtr city )
{
  HouseList houses = city->statistic().houses.find();
  const int minWorkersNumber = 4;
  for( auto itHouse=houses.begin(); itHouse != houses.end(); )
  {
    int houseWorkless = (*itHouse)->unemployed();

    if( !(*itHouse)->enterArea().empty() && houseWorkless > minWorkersNumber ) { ++itHouse; }
    else { itHouse = houses.erase( itHouse ); }
  }

  if( !houses.empty() )
  {
    int number = math::random( houses.size() );
    HouseList randHouses = houses.random( number );
    for( auto house : randHouses )
    {
      ImmigrantPtr emigrant = Walker::create<Immigrant>( city );
      if( emigrant.isValid() )
      {
        house->removeHabitants( minWorkersNumber );
        emigrant->leaveCity( *(house->enterArea().front()) );
        emigrant->setThinks( "##immigrant_no_work_for_me##" );
      }
    }
  }
}

}//end namespace city
