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
#include "core/safetycast.hpp"
#include "gfx/tilemap.hpp"
#include "walker/immigrant.hpp"
#include "core/position.hpp"
#include "objects/road.hpp"
#include "objects/house.hpp"
#include "gfx/tile.hpp"
#include "funds.hpp"
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
  int lastMonthPopulation;
  int lastMonthMigration;
  int updateTickInerval;
  int emigrantsIndesirability;
  int worklessMinInfluence;
  int checkRange;
  DateTime lastUpdate;

  float getMigrationKoeff(PlayerCity& city);
  Info::Parameters lastMonthParams(PlayerCity& city);
  void createMigrationToCity(PlayerCity& city);
  void createMigrationFromCity(PlayerCity& city);
  unsigned int calcVacantHouse( PlayerCity& city );
  bool isPoorHousing( int shacks, int houses );
};

SrvcPtr Migration::create(PlayerCityPtr city)
{
  SrvcPtr ret( new Migration( city ) );
  ret->drop();

  return ret;
}

Migration::Migration( PlayerCityPtr city )
  : Srvc( *city.object(), defaultName() ), _d( new Impl )
{  
  VariantMap options = SaveAdapter::load( ":/migration.model" );
  VARIANT_LOAD_ANYDEF_D( _d, checkRange, DateTime::daysInWeek, options )
  VARIANT_LOAD_ANYDEF_D( _d, worklessMinInfluence, 0, options )

  _d->lastMonthMigration = 0;
  _d->lastMonthPopulation = 0;
  _d->lastUpdate = GameDate::current();
  _d->updateTickInerval = GameDate::days2ticks( 7 );
  _d->emigrantsIndesirability = 0;
}

void Migration::update( const unsigned int time )
{  
  if( time % _d->updateTickInerval != 1 )
    return;

  Logger::warning( "MigrationSrvc: start calculate" );
  const int worklessCitizenAway = SETTINGS_VALUE( worklessCitizenAway );

  float migrationKoeff = _d->getMigrationKoeff( _city );
  Info::Parameters params = _d->lastMonthParams( _city );
  Logger::warning( "MigrationSrvc: current migration koeff=%f", migrationKoeff );

  _d->emigrantsIndesirability = defaultEmIndesirability; //base indesirability value
  float emDesKoeff = math::clamp<float>( (float)SETTINGS_VALUE( emigrantSalaryKoeff ), 1.f, 99.f );

  //if salary in city more then empire people more effectivelly go to our city
  const int diffSalary = _city.empire()->workerSalary() - _city.funds().workerSalary();
  int diffSalaryInfluence = diffSalary * emDesKoeff;

  //emigrant like when lot of food stock int city
  const int minMonthWithFood = SETTINGS_VALUE( minMonthWithFood );
  int foodStackInfluence = ( params.monthWithFood < minMonthWithFood
                                   ? ((minMonthWithFood - params.monthWithFood) * 3)
                                   : -params.monthWithFood );

  int sentimentInfluence = (params.sentiment - 50) / 5;

  //emigrant need workplaces
  int worklessInfluence = params.workless == 0
                          ? -10
                          : (params.workless * (params.workless < worklessCitizenAway ? 1 : 2));

  int taxLevelInfluence = ( params.tax > possibleTaxLevel
                            ? params.tax * 2
                            : (possibleTaxLevel-params.tax) );

  int warInfluence = ( params.monthWithourWar < DateTime::monthsInYear
                          ? (DateTime::monthsInYear - params.monthWithourWar) * 5
                          : -std::min( params.monthWithourWar, 10 ) );
  warInfluence += params.milthreat;

  int slumsInfluence = ( _d->isPoorHousing( params.slumNumber, params.houseNumber ) ? 20 : 0);
  int shacksInfluence = ( _d->isPoorHousing( params.shackNumber, params.houseNumber ) ? 10 : 0 );

  _d->emigrantsIndesirability += worklessInfluence * std::min<int>( _city.population(), 150 ) / _d->worklessMinInfluence;

  _d->emigrantsIndesirability += diffSalaryInfluence;
  _d->emigrantsIndesirability += foodStackInfluence;
  _d->emigrantsIndesirability += params.crimeLevel;
  _d->emigrantsIndesirability += taxLevelInfluence;
  _d->emigrantsIndesirability += warInfluence;
  _d->emigrantsIndesirability += shacksInfluence;
  _d->emigrantsIndesirability += slumsInfluence;
  _d->emigrantsIndesirability += sentimentInfluence;

  _d->emigrantsIndesirability *= migrationKoeff;

  Logger::warning( "MigrationSrvc: current indesrbl=%d", _d->emigrantsIndesirability );

  int goddesRandom = math::random( maxIndesirability );
  if( goddesRandom > _d->emigrantsIndesirability )
  {
    _d->createMigrationToCity( _city );
    _d->updateTickInerval = math::random( GameDate::days2ticks( _d->checkRange ) ) + 10;
  }
  else
  {
    _d->updateTickInerval = GameDate::days2ticks( _d->checkRange );
  }

  if( _d->lastUpdate.monthsTo( GameDate::current() ) > 0 )
  {
    _d->lastUpdate = GameDate::current();
    _d->lastMonthMigration = _city.population() - _d->lastMonthPopulation;
    _d->lastMonthPopulation = _city.population();

    Logger::warning( "MigrationSrvc: current workless=%f indesrbl=%f",
                        params.workless * migrationKoeff,
                        _d->emigrantsIndesirability * migrationKoeff );

    if( params.workless * migrationKoeff > worklessCitizenAway
        || _d->emigrantsIndesirability * migrationKoeff > maxIndesirability )
    {
      _d->createMigrationFromCity( _city );
    }
  }
}

std::string Migration::reason() const
{
  unsigned int vacantHouse = _d->calcVacantHouse( _city );
  if( vacantHouse == 0 )
    return "##migration_lack_empty_house##";

  StringArray troubles;
  if( _d->emigrantsIndesirability > defaultEmIndesirability )
  {
    Info::Parameters params = _d->lastMonthParams( _city );
    if( params.monthWithourWar < DateTime::monthsInYear )
    {
      troubles << "##migration_war_deterring##";
    }

    if( params.monthWithFood < (int)SETTINGS_VALUE( minMonthWithFood ) )
    {
      if( params.monthWithFood == 0 ) { troubles << "##migration_empty_granary##"; }
      else if( params.monthWithFood < 2 ) { troubles << "##migration_low_food_stocks##"; }
      else { troubles << "##migration_lessfood_granary##"; }
    }

    if( params.workless > (int)SETTINGS_VALUE( worklessCitizenAway ) )
    {
      troubles << "##migration_people_away##";
    }
    else
    {
      if( params.workless > 20 ) { troubles << "##migration_broke_workless##"; }
      else if( params.workless > 10 ) { troubles << "##migration_middle_lack_workless##"; }
      else if( params.workless > 5 ) { troubles << "##migration_lack_workless##"; }
    }

    int diffWages = params.romeWages - params.cityWages;
    if( diffWages > 5 ) { troubles << "##low_wage_broke_migration##"; }
    else if( diffWages > 1 ) { troubles <<  "##low_wage_lack_migration##"; }

    if( params.crimeLevel > 25 ) { troubles << "##migration_lack_crime##"; }

    if( params.tax > strongTaxLevel )    { troubles << "##migration_broke_tax##";  }
    else if( params.tax > simpleTaxLevel ) { troubles <<  "##migration_middle_lack_tax##"; }
    else if( params.tax > possibleTaxLevel ) { troubles << "##migration_lack_tax##"; }

    if( params.sentiment < 50 ) { troubles << "##poor_city_mood_lack_migration##";}

    if( _d->isPoorHousing( params.shackNumber, params.houseNumber ) ) { troubles << "##poor_housing_discourages_migration##";}
  }

  return troubles.empty()
           ? "##migration_peoples_arrived_in_city##"
           : troubles.random();
}

std::string Migration::leaveCityReason() const
{
  if( lastMonthMigration() < 0 )
  {
    Info::Parameters lastParams = _d->lastMonthParams( _city );
    if( lastParams.tax > insaneTaxLevel )
      return "##people_leave_city_insane_tax##";

    if( lastParams.payDiff > 5 )
      return "##people_leave_city_low_wage##";

    if( lastParams.workless > 15 )
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
  VARIANT_SAVE_ANY_D( ret, _d, lastMonthPopulation )

  return ret;
}

void Migration::load(const VariantMap& stream)
{
  VARIANT_LOAD_TIME_D( _d, lastUpdate, stream )
  VARIANT_LOAD_ANY_D( _d, lastMonthMigration, stream )
  VARIANT_LOAD_ANY_D( _d, lastMonthPopulation, stream )
}

unsigned int Migration::Impl::calcVacantHouse( PlayerCity& city )
{
  unsigned int vh = 0;
  Helper helper( &city );
  HouseList houses = helper.find<House>(building::house);
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

float Migration::Impl::getMigrationKoeff( PlayerCity& city )
{
  return ( std::min<float>( city.population(), 300 ) / 300.f );
}

Info::Parameters Migration::Impl::lastMonthParams( PlayerCity& city )
{
  InfoPtr info;
  info << ptr_cast<Info>( city.findService( Info::defaultName() ) );

  Info::Parameters params;
  if( info.isValid() )
  {
    params = info->lastParams();
  }

  return params;
}

void Migration::Impl::createMigrationToCity( PlayerCity& city )
{
  unsigned int vh = calcVacantHouse( city );
  if( vh == 0 )
  {
    return;
  }

  EmigrantList migrants;
  migrants << city.walkers( walker::any );

  if( vh <= migrants.size() * 5 )
  {
    return;
  }

  Tile& roadTile = city.tilemap().at( city.borderInfo().roadEntry );

  ImmigrantPtr emigrant = Immigrant::create( &city );

  if( emigrant.isValid() )
  {
    emigrant->send2city( roadTile );
    emigrant->setSpeedMultiplier( 0.8f + math::random( 40 ) / 100.f );
  }
}

void Migration::Impl::createMigrationFromCity( PlayerCity& city )
{
  Helper helper( &city );
  HouseList houses = helper.find<House>(building::house);
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

      ImmigrantPtr emigrant = Immigrant::create( &city );

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
