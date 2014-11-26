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
#include "desirability_updater.hpp"
#include "cityservice_military.hpp"
#include "cityservice_health.hpp"
#include "goods_updater.hpp"
#include "service_updater.hpp"

namespace city
{

class ServiceFactory::Impl
{
public:
  typedef std::vector< ServiceCreatorPtr > Creators;
  Creators creators;
};

SrvcPtr ServiceFactory::create( PlayerCityPtr city, const std::string& name )
{
  std::string::size_type sharpPos = name.find( "#" );
  std::string srvcType = sharpPos != std::string::npos ? name.substr( sharpPos+1 ) : name;

  Logger::warning( "CityServiceFactory: try find creator for service " + srvcType );

  ServiceFactory& inst = instance();
  foreach( it, inst._d->creators )
  {
    if( srvcType == (*it)->serviceName() )
    {
      city::SrvcPtr srvc = (*it)->create( city );
      srvc->setName( name );
      return srvc;
    }
  }

  Logger::warning( "CityServiceFactory: not found creator for service " + name );
  return SrvcPtr();
}

ServiceFactory& ServiceFactory::instance()
{
  static city::ServiceFactory inst;
  return inst;
}

void ServiceFactory::addCreator( ServiceCreatorPtr creator )
{
  if( creator.isNull() )
    return;

  foreach( it, _d->creators )
  {
    if( creator->serviceName() == (*it)->serviceName() )
    {
      Logger::warning( "CityServiceFactory: Also have creator for service " + creator->serviceName() );
      return;
    }
  }

  _d->creators.push_back( creator );
}

ServiceFactory::ServiceFactory() : _d( new Impl )
{
  addCreator<Migration>();
  addCreator<WorkersHire>();
  addCreator<ProsperityRating>();
  addCreator<Shoreline>();
  addCreator<Info>();
  addCreator<CultureRating>();
  addCreator<Animals>();
  addCreator<Religion>();
  addCreator<Festival>();
  addCreator<Roads>();
  addCreator<Fishery>();
  addCreator<Disorder>();
  addCreator<request::Dispatcher>();
  addCreator<HealthUpdater>();
  addCreator<DesirabilityUpdater>();
  addCreator<Military>();
  addCreator<HealthCare>();
  addCreator<GoodsUpdater>();
  addCreator<ServiceUpdater>();
}

}//end namespace city
