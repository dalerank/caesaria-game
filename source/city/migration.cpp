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

#include "migration.hpp"
#include "objects/construction.hpp"
#include "helper.hpp"
#include "core/safetycast.hpp"
#include "gfx/tilemap.hpp"
#include "walker/emigrant.hpp"
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

using namespace constants;
using namespace gfx;

namespace city
{

namespace {
const int possibleTaxLevel = 7;
const int maxIndesirability = 100;
const int defaultEmIndesirability = 50;
}

class Migration::Impl
{
public:
  int lastMonthPopulation;
  int lastMonthMigration;
  int updateTickInerval;
  int emigrantsIndesirability;
  DateTime lastUpdate;

  float getMigrationKoeff(PlayerCity& city);
  Info::Parameters getLastParams(PlayerCity& city);
  void createMigrationToCity(PlayerCity& city);
  void createMigrationFromCity(PlayerCity& city);
  unsigned int calcVacantHouse( PlayerCity& city );
};

SrvcPtr Migration::create(PlayerCityPtr city)
{
  SrvcPtr ret( new Migration( city ) );
  ret->drop();

  return ret;
}

Migration::Migration( PlayerCityPtr city )
  : Srvc( *city.object(), getDefaultName() ), _d( new Impl )
{
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
  const int worklessCitizenAway = GameSettings::get( GameSettings::worklessCitizenAway );

  float migrationKoeff = _d->getMigrationKoeff( _city );
  Info::Parameters params = _d->getLastParams( _city );
  Logger::warning( "MigrationSrvc: current migration koeff=%f", migrationKoeff );

  _d->emigrantsIndesirability = defaultEmIndesirability; //base indesirability value
  float emDesKoeff = math::clamp<float>( (float)GameSettings::get( GameSettings::emigrantSalaryKoeff ), 1.f, 99.f );

  //if salary in city more then empire people more effectivelly go to our city
  int diffSalary = _city.empire()->getWorkerSalary() - _city.funds().workerSalary();
  _d->emigrantsIndesirability += diffSalary * emDesKoeff;

  //emigrant like when lot of food stock int city
  int minMonthWithFood = GameSettings::get( GameSettings::minMonthWithFood );
  _d->emigrantsIndesirability += ( params.monthWithFood < minMonthWithFood
                               ? ((minMonthWithFood - params.monthWithFood) * 3)
                               : -params.monthWithFood );
  //emigrant need workplaces
  _d->emigrantsIndesirability += params.workless == 0
                              ? -10
                              : (params.workless * (params.workless < worklessCitizenAway ? 1 : 2));

  _d->emigrantsIndesirability += params.crimeLevel;
  _d->emigrantsIndesirability += ( params.tax > possibleTaxLevel
                                  ? params.tax * 2
                                  : -params.tax );

  _d->emigrantsIndesirability *= migrationKoeff;
  Logger::warning( "MigrationSrvc: current indesrbl=%d", _d->emigrantsIndesirability );

  int goddesRandom = math::random( maxIndesirability );
  if( goddesRandom > _d->emigrantsIndesirability )
  {
    _d->createMigrationToCity( _city );
    _d->updateTickInerval = math::random( GameDate::days2ticks( DateTime::daysInWeek ) ) + 10;
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

std::string Migration::getReason() const
{
  unsigned int vacantHouse = _d->calcVacantHouse( _city );
  if( vacantHouse == 0 )
    return "##migration_lack_empty_house##";

  if( _d->emigrantsIndesirability > defaultEmIndesirability )
  {
    Info::Parameters params = _d->getLastParams( _city );
    if( params.monthWithFood < (int)GameSettings::get( GameSettings::minMonthWithFood ) )
    {
      if( params.monthWithFood == 0 )
      {
        return "##migration_empty_granary##";
      }

      return "##migration_lessfood_granary##";
    }

    if( params.workless > 5 )
    {
      if( params.workless > (int)GameSettings::get( GameSettings::worklessCitizenAway ) )
        return "##migration_people_away##";

      if( params.workless > 10 )
      {
        if( params.workless > 20 )
        {
          return "##migration_broke_workless##";
        }

        return "##migration_middle_lack_workless##";
      }

      return "##migration_lack_jobs##";
    }

    int diffWages = params.romeWages - params.cityWages;
    if( diffWages > 1 )
    {
      if( diffWages > 5 )
        return "##low_wage_broke_migration##";

      return "##low_wage_lack_migration##";
    }

    if( params.crimeLevel > 25 )
      return "##migration_lack_crime##";

    if( params.tax > 7 )
    {
      if( params.tax > 10 )
      {
        if( params.tax > 15 )
        {
          return "##migration_broke_tax##";
        }

        return "##migration_middle_lack_tax##";
      }
      return "##migration_lack_tax##";
    }
  }

  return "##migration_peoples_arrived_in_city##";
}

std::string Migration::getDefaultName() { return "migration"; }

VariantMap Migration::save() const
{
  VariantMap ret;

  ret[ "lastUpdate" ] = _d->lastUpdate;
  ret[ "lastMonthMigration" ] = _d->lastMonthMigration;
  ret[ "lastMonthPopulation" ] = _d->lastMonthPopulation;

  return ret;
}

void Migration::load(const VariantMap& stream)
{
  _d->lastUpdate = stream.get( "lastUpdate", GameDate::current() ).toDateTime();
  _d->lastMonthMigration = stream.get( "lastMonthMigration", 0 );
  _d->lastMonthPopulation = stream.get( "lastMonthPopulation", 0 );
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

float Migration::Impl::getMigrationKoeff( PlayerCity& city )
{
  return ( std::min<float>( city.population(), 300 ) / 300.f );
}

Info::Parameters Migration::Impl::getLastParams( PlayerCity& city )
{
  SmartPtr<Info> info = ptr_cast<Info>( city.findService( Info::getDefaultName() ) );

  Info::Parameters params;
  if( info.isValid() )
  {
    params = info->getLast();
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

  WalkerList walkers = city.getWalkers( walker::emmigrant );

  if( vh <= walkers.size() * 5 )
  {
    return;
  }

  Tile& roadTile = city.tilemap().at( city.borderInfo().roadEntry );

  EmigrantPtr emigrant = Emigrant::create( &city );

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
    if( !(*i)->getEnterArea().empty() && houseWorkless > minWorkersNumber ) { ++i; }
    else { i = houses.erase( i ); }
  }

  if( !houses.empty() )
  {
    int stepNumber = std::max<int>( rand() % houses.size(), 1 );
    for( int i=0; i < stepNumber; i++ )
    {
      HouseList::iterator house = houses.begin();
      std::advance( house, math::random( houses.size() ) );

      EmigrantPtr emigrant = Emigrant::create( &city );

      if( emigrant.isValid() )
      {
        (*house)->remHabitants( minWorkersNumber );
        emigrant->leaveCity( *(*house)->getEnterArea().front() );
        emigrant->setThinks( "##immigrant_no_work_for_me##" );
      }
    }
  }
}

}//end namespace city
