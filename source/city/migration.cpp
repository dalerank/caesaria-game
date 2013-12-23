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

using namespace constants;

class CityMigration::Impl
{
public:
  PlayerCityPtr city;
  int lastMonthPopulation;
  int lastMonthMigration;
  DateTime lastUpdate;
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
  _d->lastMonthPopulation = 0;
  _d->lastUpdate = GameDate::current();
}

void CityMigration::update( const unsigned int time )
{
  if( time % 44 != 1 )
    return;

  if( _d->lastUpdate.getMonthToDate( GameDate::current() ) > 0 )
  {
    _d->lastUpdate = GameDate::current();
    _d->lastMonthMigration = _d->city->getPopulation() - _d->lastMonthPopulation;
    _d->lastMonthPopulation = _d->city->getPopulation();
  }
  
  unsigned int vacantPop=0;
  int emigrantsIndesirability = 50; //base indesirability value
  float emDesKoeff = math::clamp<float>( (float)GameSettings::get( GameSettings::emigrantSalaryKoeff ), 1.f, 99.f );
  //if salary in city more then empire people more effectivelly go to ouu city
  emigrantsIndesirability += (_d->city->getEmpire()->getWorkersSalary() - _d->city->getFunds().getWorkerSalary()) * emDesKoeff;

  int worklessPercent = CityStatistic::getWorklessPercent( _d->city );
  emigrantsIndesirability += worklessPercent == 0
                            ? -10
                            : (worklessPercent * (worklessPercent < 15 ? 1 : 2));

  int goddesRandom = rand() % 100;
  if( goddesRandom < emigrantsIndesirability )
    return;

  CityHelper helper( _d->city );
  HouseList houses = helper.find<House>(building::house);
  foreach( HousePtr house, houses )
  {
    if( house->getAccessRoads().size() > 0 )
    {
      vacantPop += math::clamp( house->getMaxHabitants() - house->getHabitants().count(), 0, 0xff );
    }
  }

  if( vacantPop == 0 )
  {
    return;
  }

  WalkerList walkers = _d->city->getWalkers( walker::emigrant );

  if( vacantPop <= walkers.size() * 5 )
  {
    return;
  }

  Tile& roadTile = _d->city->getTilemap().at( _d->city->getBorderInfo().roadEntry );

  EmigrantPtr emigrant = Emigrant::create( _d->city );

  if( emigrant.isValid() )
  {
    emigrant->send2City( roadTile );
  }
}

std::string CityMigration::getReason() const
{
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

  return ret;
}

void CityMigration::load(const VariantMap& stream)
{
  _d->lastUpdate = stream.get( "lastUpdate", GameDate::current() ).toDateTime();
  _d->lastMonthMigration = stream.get( "lastMonthMigration", 0 );
  _d->lastMonthPopulation = stream.get( "lastMonthPopulation", 0 );
}
