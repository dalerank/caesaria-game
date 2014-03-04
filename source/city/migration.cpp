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

namespace city
{

class Migration::Impl
{
public:
  PlayerCityPtr city;
  int lastMonthPopulation;
  int lastMonthMigration;
  int updateTickInerval;
  DateTime lastUpdate;

  float getMigrationKoeff()
  {
    return ( std::min<float>( city->getPopulation(), 300 ) / 300.f );
  }

  Info::Parameters getLastParams()
  {
    SmartPtr<Info> info = ptr_cast<Info>( city->findService( Info::getDefaultName() ) );

    Info::Parameters params;
    if( info.isValid() )
    {
      params = info->getLast();
    }

    return params;
  }

  void createMigrationToCity();
  void createMigrationFromCity();
  unsigned int calcVacantHouse();
};

SrvcPtr Migration::create(PlayerCityPtr city )
{
  SrvcPtr ret( new Migration( city ) );
  ret->drop();

  return ret;
}

Migration::Migration( PlayerCityPtr city )
: Srvc( getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->lastMonthMigration = 0;
  _d->lastMonthPopulation = 0;
  _d->lastUpdate = GameDate::current();
  _d->updateTickInerval = GameDate::ticksInMonth() / 2;
}

void Migration::update( const unsigned int time )
{  
  if( time % _d->updateTickInerval != 1 )
    return;

  Logger::warning( "MigrationSrvc: start calculate" );
  const int worklessCitizenAway = GameSettings::get( GameSettings::worklessCitizenAway );
  const int maxIndesirability = 100;

  float migrationKoeff = _d->getMigrationKoeff();
  Info::Parameters params = _d->getLastParams();
  Logger::warning( "MigrationSrvc: current migration koeff=%f", migrationKoeff );

  int emigrantsIndesirability = 50; //base indesirability value
  float emDesKoeff = math::clamp<float>( (float)GameSettings::get( GameSettings::emigrantSalaryKoeff ), 1.f, 99.f );

  //if salary in city more then empire people more effectivelly go to our city
  int diffSalary = _d->city->getEmpire()->getWorkerSalary() - _d->city->getFunds().getWorkerSalary();
  emigrantsIndesirability += diffSalary * emDesKoeff;

  //emigrant like when lot of food stock int city
  int minMonthWithFood = GameSettings::get( GameSettings::minMonthWithFood );
  emigrantsIndesirability += ( params.monthWithFood < minMonthWithFood
                               ? ((minMonthWithFood - params.monthWithFood) * 3)
                               : -params.monthWithFood );
  //emigrant need workplaces
  emigrantsIndesirability += params.workless == 0
                              ? -10
                              : (params.workless * (params.workless < worklessCitizenAway ? 1 : 2));

  emigrantsIndesirability *= migrationKoeff;
  Logger::warning( "MigrationSrvc: current indesrbl=%d", emigrantsIndesirability );


  int goddesRandom = math::random( maxIndesirability );
  if( goddesRandom > emigrantsIndesirability )
  {
    _d->createMigrationToCity();
    _d->updateTickInerval = math::random( GameDate::ticksInMonth() / 2 ) + 10;
  }

  if( _d->lastUpdate.getMonthToDate( GameDate::current() ) > 0 )
  {
    _d->lastUpdate = GameDate::current();
    _d->lastMonthMigration = _d->city->getPopulation() - _d->lastMonthPopulation;
    _d->lastMonthPopulation = _d->city->getPopulation();

    Logger::warning( "MigrationSrvc: current workless=%f indesrbl=%f",
                        params.workless * migrationKoeff,
                        emigrantsIndesirability * migrationKoeff );

    if( params.workless * migrationKoeff > worklessCitizenAway
        || emigrantsIndesirability * migrationKoeff > maxIndesirability )
    {
      _d->createMigrationFromCity();
    }
  }
}

std::string Migration::getReason() const
{
  unsigned int vacantHouse = _d->calcVacantHouse();
  if( vacantHouse == 0 )
    return "##migration_lack_empty_house##";

  if( _d->getMigrationKoeff() > 0.99f )
  {
    Info::Parameters params = _d->getLastParams();
    if( params.monthWithFood < (int)GameSettings::get( GameSettings::minMonthWithFood ) )
      return "##migration_lessfood_granary##";
    if( params.monthWithFood == 0 )
      return "##migration_empty_granary##";
    if( params.workless > 5 )
      return "##migration_lack_jobs##";
    if( params.workless > (int)GameSettings::get( GameSettings::worklessCitizenAway ) )
      return "##migration_people_away##";
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

unsigned int Migration::Impl::calcVacantHouse()
{
  unsigned int vh = 0;
  CityHelper helper( city );
  HouseList houses = helper.find<House>(building::house);
  foreach( house, houses )
  {
    if( (*house)->getAccessRoads().size() > 0 )
    {
      vh += math::clamp<int>( (*house)->getMaxHabitants() - (*house)->getHabitants().count(), 0, 0xff );
    }
  }

  return vh;
}

void Migration::Impl::createMigrationToCity()
{
  unsigned int vh = calcVacantHouse();
  if( vh == 0 )
  {
    return;
  }

  WalkerList walkers = city->getWalkers( walker::emigrant );

  if( vh <= walkers.size() * 5 )
  {
    return;
  }

  Tile& roadTile = city->getTilemap().at( city->getBorderInfo().roadEntry );

  EmigrantPtr emigrant = Emigrant::create( city );

  if( emigrant.isValid() )
  {
    emigrant->send2city( roadTile );
  }
}

void Migration::Impl::createMigrationFromCity()
{
  CityHelper helper( city );
  HouseList houses = helper.find<House>(building::house);
  const int minWorkersNumber = 4;
  for( HouseList::iterator i=houses.begin(); i != houses.end(); )
  {
    int houseWorkless = (*i)->getServiceValue( Service::recruter );
    if( !(*i)->getEnterArea().empty() && houseWorkless > minWorkersNumber ) { i++; }
    else { i = houses.erase( i ); }
  }

  if( !houses.empty() )
  {
    int stepNumber = std::max<int>( rand() % houses.size(), 1 );
    for( int i=0; i < stepNumber; i++ )
    {
      HouseList::iterator house = houses.begin();
      std::advance( house, math::random( houses.size() ) );

      EmigrantPtr emigrant = Emigrant::create( city );

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
