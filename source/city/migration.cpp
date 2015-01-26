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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "migration.hpp"
#include "objects/construction.hpp"
#include "helper.hpp"
#include "city/cityservice_military.hpp"
#include "core/safetycast.hpp"
#include "gfx/tilemap.hpp"
#include "walker/immigrant.hpp"
#include "core/position.hpp"
#include "objects/road.hpp"
#include "objects/house.hpp"
#include "gfx/tile.hpp"
#include "funds.hpp"
#include "core/variant_map.hpp"
#include "objects/constants.hpp"
#include "game/settings.hpp"
#include "world/empire.hpp"
#include "game/gamedate.hpp"
#include "statistic.hpp"
#include "cityservice_info.hpp"
#include "core/logger.hpp"
#include "core/saveadapter.hpp"

using namespace constants;
using namespace gfx;

namespace city
{

namespace {
const int possibleTaxLevel = 7;
const int maxIndesirability = 100;
const int simpleTaxLevel = 10;
const int strongTaxLevel = 15;
const int insaneTaxLevel = 20;
const int defaultEmIndesirability = 50;
}

class Migration::Impl
{
public:
  int lastMonthMigration;
  int updateTickInerval;
  int emigrantsIndesirability;
  int lastMonthComing;
  int lastMonthLeaving;
  int worklessMinInfluence;
  int checkRange;
  DateTime lastUpdate;

  float getMigrationKoeff(PlayerCityPtr city);
  Info::Parameters lastMonthParams(PlayerCityPtr city);
  void createMigrationToCity(PlayerCityPtr city);
  void createMigrationFromCity(PlayerCityPtr city);
  unsigned int calcVacantHouse( PlayerCityPtr city );
  bool isPoorHousing( int shacks, int houses );
};

SrvcPtr Migration::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Migration( city ) );
  ret->drop();

  return ret;
}

Migration::Migration( PlayerCityPtr city )
  : Srvc( city, defaultName() ), _d( new Impl )
{  
  VariantMap options = config::load( ":/migration.model" );
  VARIANT_LOAD_ANYDEF_D( _d, checkRange, DateTime::daysInWeek, options )
  VARIANT_LOAD_ANYDEF_D( _d, worklessMinInfluence, 0, options )

  _d->lastMonthMigration = 0;
  _d->lastMonthComing = 0;
  _d->lastMonthLeaving = 0;
  _d->lastUpdate = game::Date::current();
  _d->updateTickInerval = game::Date::days2ticks( 7 );
  _d->emigrantsIndesirability = 0;
}

void Migration::timeStep( const unsigned int time )
{  
  if( time % _d->updateTickInerval != 1 )
    return;

  Logger::warning( "MigrationSrvc: start calculate" );
  const int worklessCitizenAway = SETTINGS_VALUE( worklessCitizenAway );

  float migrationKoeff = _d->getMigrationKoeff( _city() );
  Info::Parameters params = _d->lastMonthParams( _city() );
  Logger::warning( "MigrationSrvc: current migration koeff=%f", migrationKoeff );

  _d->emigrantsIndesirability = defaultEmIndesirability; //base indesirability value
  float emDesKoeff = math::clamp<float>( (float)SETTINGS_VALUE( emigrantSalaryKoeff ), 1.f, 99.f );

  //if salary in city more then empire people more effectivelly go to our city
  const int diffSalary = _city()->empire()->workerSalary() - _city()->funds().workerSalary();
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
                          ? -10
                          : (curWorklessValue * (curWorklessValue < worklessCitizenAway ? 1 : 2));

  int taxLevelInfluence = ( params[ Info::tax ] > possibleTaxLevel
                            ? params[ Info::tax ] * 2
                            : (possibleTaxLevel-params[ Info::tax ]) );

  const int& monthWithourWar = _city()->age() > 1
                                  ? params[ Info::monthWtWar ]
                                  : DateTime::monthsInYear;

  int warInfluence = ( monthWithourWar < DateTime::monthsInYear
                          ? (DateTime::monthsInYear - monthWithourWar) * 5
                          : -std::min( monthWithourWar, 10 ) );

  warInfluence += params[ Info::milthreat ];

  int slumsInfluence = ( _d->isPoorHousing( params[ Info::slumNumber ], params[ Info::houseNumber ] ) ? 20 : 0);
  int shacksInfluence = ( _d->isPoorHousing( params[ Info::shackNumber ], params[ Info::houseNumber ] ) ? 10 : 0 );

  if( _d->worklessMinInfluence > 0 )
  {
    _d->emigrantsIndesirability += worklessInfluence * std::min<int>( _city()->population(), 150 ) / _d->worklessMinInfluence;
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

  _d->emigrantsIndesirability *= migrationKoeff;

  Logger::warning( "MigrationSrvc: current indesrbl=%d", _d->emigrantsIndesirability );
  if( warInfluence > 50 )
  {
    Logger::warning( "Migration: enemies in city migration broke" );
    return;
  }

  city::MilitaryPtr mil;
  mil << _city()->findService( city::Military::defaultName() );

  if( mil.isValid() )
  {
    bool cityUnderAttack = mil->isUnderAttack();

    if( cityUnderAttack )
      _d->emigrantsIndesirability *= 2;
  }

  int goddesRandom = math::random( maxIndesirability );
  if( goddesRandom > _d->emigrantsIndesirability )
  {
    _d->createMigrationToCity( _city() );
    _d->updateTickInerval = math::random( game::Date::days2ticks( _d->checkRange ) ) + 10;
  }
  else
  {
    _d->updateTickInerval = game::Date::days2ticks( _d->checkRange );
  }

  if( _d->lastUpdate.monthsTo( game::Date::current() ) > 0 )
  {
    _d->lastUpdate = game::Date::current();
    _d->lastMonthMigration = _d->lastMonthComing - _d->lastMonthLeaving;
    _d->lastMonthComing = 0;
    _d->lastMonthLeaving = 0;

    Logger::warning( "MigrationSrvc: current workless=%f indesrbl=%f",
                        curWorklessValue * migrationKoeff,
                        _d->emigrantsIndesirability * migrationKoeff );

    if( curWorklessValue * migrationKoeff > worklessCitizenAway
        || _d->emigrantsIndesirability * migrationKoeff > maxIndesirability )
    {
      _d->createMigrationFromCity( _city() );
    }
  }
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
      if( params[ Info::workless ] > 20 ) { troubles << "##migration_broke_workless##"; }
      else if( params[ Info::workless ] > 10 ) { troubles << "##migration_middle_lack_workless##"; }
      else if( params[ Info::workless ] > 5 ) { troubles << "##migration_lack_workless##"; }
    }

    int diffWages = params[ Info::romeWages ] - params[ Info::cityWages ];
    if( diffWages > 5 ) { troubles << "##low_wage_broke_migration##"; }
    else if( diffWages > 1 ) { troubles <<  "##low_wage_lack_migration##"; }

    if( params[ Info::crimeLevel ] > 25 ) { troubles << "##migration_lack_crime##"; }

    if( params[ Info::tax ] > strongTaxLevel )    { troubles << "##migration_broke_tax##";  }
    else if( params[ Info::tax ] > simpleTaxLevel ) { troubles <<  "##migration_middle_lack_tax##"; }
    else if( params[ Info::tax ] > possibleTaxLevel ) { troubles << "##migration_lack_tax##"; }

    if( params[ Info::sentiment ] < 50 ) { troubles << "##poor_city_mood_lack_migration##";}

    if( _d->isPoorHousing( params[ Info::shackNumber ], params[ Info::houseNumber ] ) ) { troubles << "##poor_housing_discourages_migration##";}
  }

  return troubles.empty()
           ? "##migration_peoples_arrived_in_city##"
           : troubles.random();
}

std::string Migration::leaveCityReason() const
{
  if( lastMonthMigration() < 0 )
  {
    Info::Parameters lastParams = _d->lastMonthParams( _city() );
    if( lastParams[ Info::tax ] > insaneTaxLevel )
      return "##people_leave_city_insane_tax##";

    if( lastParams[ Info::payDiff ] > 5 )
      return "##people_leave_city_low_wage##";

    if( lastParams[ Info::workless ] > 15 )
      return "##migration_people_away##";

    return "##people_leave_city_some##";
  }

  return "";
}

std::string Migration::defaultName() { return CAESARIA_STR_EXT(Migration); }
int Migration::lastMonthMigration() const { return _d->lastMonthMigration; }

VariantMap Migration::save() const
{
  VariantMap ret;

  VARIANT_SAVE_ANY_D( ret, _d, lastUpdate )
  VARIANT_SAVE_ANY_D( ret, _d, lastMonthMigration )
  VARIANT_SAVE_ANY_D( ret, _d, lastMonthComing )
  VARIANT_SAVE_ANY_D( ret, _d, lastMonthLeaving )

  return ret;
}

void Migration::load(const VariantMap& stream)
{
  VARIANT_LOAD_TIME_D( _d, lastUpdate, stream )
  VARIANT_LOAD_ANY_D( _d, lastMonthMigration, stream )
  VARIANT_LOAD_ANY_D( _d, lastMonthComing, stream )
  VARIANT_LOAD_ANY_D( _d, lastMonthLeaving, stream )
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
  Helper helper( city );
  HouseList houses = helper.find<House>(objects::house);
  foreach( house, houses )
  {
    if( (*house)->getAccessRoads().size() > 0 )
    {
      vh += math::clamp<int>( (*house)->maxHabitants() - (*house)->habitants().count(), 0, 0xff );
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
  return ( std::min<float>( city->population(), 300 ) / 300.f );
}

Info::Parameters Migration::Impl::lastMonthParams( PlayerCityPtr city )
{
  InfoPtr info;
  info << city->findService( Info::defaultName() );

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

  EmigrantList migrants;
  migrants << city->walkers();

  if( vh <= migrants.size() * 5 )
  {
    return;
  }

  Tile& roadTile = city->tilemap().at( city->borderInfo().roadEntry );

  ImmigrantPtr emigrant = Immigrant::create( city );

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
  Helper helper( city );
  HouseList houses = helper.find<House>(objects::house);
  const int minWorkersNumber = 4;
  for( HouseList::iterator i=houses.begin(); i != houses.end(); )
  {
    int houseWorkless = (*i)->getServiceValue( Service::recruter );
    if( !(*i)->enterArea().empty() && houseWorkless > minWorkersNumber ) { ++i; }
    else { i = houses.erase( i ); }
  }

  if( !houses.empty() )
  {
    int stepNumber = std::max<int>( rand() % houses.size(), 1 );
    for( int i=0; i < stepNumber; i++ )
    {
      HouseList::iterator house = houses.begin();
      std::advance( house, math::random( houses.size() ) );

      ImmigrantPtr emigrant = Immigrant::create( city );

      if( emigrant.isValid() )
      {
        (*house)->remHabitants( minWorkersNumber );
        emigrant->leaveCity( *(*house)->enterArea().front() );
        emigrant->setThinks( "##immigrant_no_work_for_me##" );
      }
    }
  }
}

}//end namespace city
