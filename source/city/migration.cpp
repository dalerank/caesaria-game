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

using namespace constants;

class CityMigration::Impl
{
public:
  PlayerCityPtr city;
  int lastMonthPopulation;
  int lastMonthMigration;
  int vacantHouse;
  DateTime lastUpdate;

  bool useAdvancedCheck()
  {
    return city->getPopulation() > 150;
  }

  CityServiceInfo::Parameters getLastParams()
  {
    SmartPtr< CityServiceInfo > info = city->findService( CityServiceInfo::getDefaultName() ).as<CityServiceInfo>();

    CityServiceInfo::Parameters params;
    if( info.isValid() )
    {
      params = info->getLast();
    }

    return params;
  }
};

CityServicePtr CityMigration::create(PlayerCityPtr city )
{
  CityServicePtr ret( new CityMigration( city ) );
  ret->drop();

  return ret;
}

CityMigration::CityMigration( PlayerCityPtr city )
: CityService( getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->lastMonthMigration = 0;
  _d->vacantHouse = 0;
  _d->lastMonthPopulation = 0;
  _d->lastUpdate = GameDate::current();
}

void CityMigration::update( const unsigned int time )
{
  if( time % 44 != 1 )
    return;

  CityServiceInfo::Parameters params = _d->getLastParams();

  if( _d->lastUpdate.getMonthToDate( GameDate::current() ) > 0 )
  {
    _d->lastUpdate = GameDate::current();
    _d->lastMonthMigration = _d->city->getPopulation() - _d->lastMonthPopulation;
    _d->lastMonthPopulation = _d->city->getPopulation();
  }
  
  unsigned int vacantHouse=0;
  int emigrantsIndesirability = 50; //base indesirability value
  float emDesKoeff = math::clamp<float>( (float)GameSettings::get( GameSettings::emigrantSalaryKoeff ), 1.f, 99.f );
  //if salary in city more then empire people more effectivelly go to ouu city
  int diffSalary = _d->city->getEmpire()->getWorkersSalary() - _d->city->getFunds().getWorkerSalary();
  emigrantsIndesirability += diffSalary * emDesKoeff;

  emigrantsIndesirability += params.workless == 0
                             ? -10
                             : (params.workless * (params.workless < 15 ? 1 : 2));

  //emigrant like when lot of food stock int city
  if( _d->useAdvancedCheck() )
  {
    int minMonthWithFood = GameSettings::get( GameSettings::minMonthWithFood );
    emigrantsIndesirability += ( params.monthWithFood < minMonthWithFood
                                 ? (params.monthWithFood * 3)
                                 : (-params.monthWithFood * 2) );
  }

  int goddesRandom = rand() % 100;
  if( goddesRandom < emigrantsIndesirability )
    return;

  CityHelper helper( _d->city );
  HouseList houses = helper.find<House>(building::house);
  foreach( HousePtr house, houses )
  {
    if( house->getAccessRoads().size() > 0 )
    {
      vacantHouse += math::clamp( house->getMaxHabitants() - house->getHabitants().count(), 0, 0xff );
    }
  }

  _d->vacantHouse = vacantHouse;
  if( vacantHouse == 0 )
  {
    return;
  }

  WalkerList walkers = _d->city->getWalkers( walker::emigrant );

  if( vacantHouse <= walkers.size() * 5 )
  {
    return;
  }

  Tile& roadTile = _d->city->getTilemap().at( _d->city->getBorderInfo().roadEntry );

  EmigrantPtr emigrant = Emigrant::create( _d->city );

  if( emigrant.isValid() )
  {
    emigrant->send2city( roadTile );
  }
}

std::string CityMigration::getReason() const
{
  if( _d->vacantHouse == 0 ) { return "##migration_lack_empty_house##"; }
  else if( _d->useAdvancedCheck() )
  {
    CityServiceInfo::Parameters params = _d->getLastParams();
    if( params.monthWithFood < (int)GameSettings::get( GameSettings::minMonthWithFood ) )
      return "##migration_lessfood_granary##";
    if( params.monthWithFood == 0 )
      return "##migration_empty_granary##";
    if( params.workless > 5 )
      return "##migration_lack_jobs##";
    if( params.workless > 15 )
      return "##migration_people_away##";
  }
  return "##unknown_migration_reason##";
}

std::string CityMigration::getDefaultName()
{
  return "migration";
}

VariantMap CityMigration::save() const
{
  VariantMap ret;

  ret[ "lastUpdate" ] = _d->lastUpdate;
  ret[ "lastMonthMigration" ] = _d->lastMonthMigration;
  ret[ "lastMonthPopulation" ] = _d->lastMonthPopulation;
  ret[ "vacantHouse" ] = _d->vacantHouse;

  return ret;
}

void CityMigration::load(const VariantMap& stream)
{
  _d->lastUpdate = stream.get( "lastUpdate", GameDate::current() ).toDateTime();
  _d->lastMonthMigration = stream.get( "lastMonthMigration", 0 );
  _d->lastMonthPopulation = stream.get( "lastMonthPopulation", 0 );
  _d->vacantHouse = stream.get( "vacantHouse", 0 );
}
