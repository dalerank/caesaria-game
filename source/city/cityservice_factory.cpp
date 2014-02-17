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

#include "cityservice_factory.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "city/migration.hpp"
#include "cityservice_workershire.hpp"
#include "cityservice_timers.hpp"
#include "cityservice_prosperity.hpp"
#include "cityservice_religion.hpp"
#include "cityservice_festival.hpp"
#include "cityservice_roads.hpp"
#include "cityservice_fishplace.hpp"
#include "cityservice_shoreline.hpp"
#include "cityservice_info.hpp"
#include "migration.hpp"
#include "requestdispatcher.hpp"
#include "cityservice_disorder.hpp"
#include "cityservice_animals.hpp"
#include "cityservice_culture.hpp"
#include "health_updater.hpp"
#include "random_damage.hpp"
#include "random_fire.hpp"
#include "desirability_updater.hpp"

class CityServiceFactory::Impl
{
public:
  typedef std::vector< CityServiceCreatorPtr > Creators;
  Creators creators;
};

CityServicePtr CityServiceFactory::create( const std::string& name, PlayerCityPtr city )
{
  CityServiceFactory& inst = instance();
  foreach( it, inst._d->creators )
  {
    if( name == (*it)->getServiceName() )
    {
      return (*it)->create( city );
    }
  }

  Logger::warning( "CityServiceFactory: not found creator for service " + name );
  return CityServicePtr();
}

CityServiceFactory& CityServiceFactory::instance()
{
  static CityServiceFactory inst;
  return inst;
}

void CityServiceFactory::addCreator( CityServiceCreatorPtr creator )
{
  if( creator.isNull() )
    return;

  foreach( it, _d->creators )
  {
    if( creator->getServiceName() == (*it)->getServiceName() )
    {
      Logger::warning( "CityServiceFactory: Also have creator for service " + creator->getServiceName() );
      return;
    }
  }

  _d->creators.push_back( creator );
}

CityServiceFactory::CityServiceFactory() : _d( new Impl )
{
  addCreator<CityMigration>();
  addCreator<CityServiceWorkersHire>();
  addCreator<CityServiceProsperity>();
  addCreator<CityServiceShoreline>();
  addCreator<CityServiceInfo>();
  addCreator<CityServiceCulture>();
  addCreator<CityServiceAnimals>();
  addCreator<CityServiceReligion>();
  addCreator<CityServiceFestival>();
  addCreator<CityServiceRoads>();
  addCreator<CityServiceFishPlace>();
  addCreator<CityServiceDisorder>();
  addCreator<CityRequestDispatcher>();
  addCreator<HealthUpdater>();
  addCreator<RandomDamage>();
  addCreator<RandomFire>();
  addCreator<DesirabilityUpdater>();
}
