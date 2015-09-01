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
#include "core/logger.hpp"
#include "city/city.hpp"
#include "cityservice_timers.hpp"

namespace city
{

typedef std::vector< ServiceCreatorPtr > Creators;

class ServiceFactory::Impl
{
public:
  Creators creators;
};

SrvcPtr ServiceFactory::create( PlayerCityPtr city, const std::string& name )
{
  std::string::size_type sharpPos = name.find( "#" );
  const std::string srvcType = sharpPos != std::string::npos ? name.substr( sharpPos+1 ) : name;

  Logger::warning( "CityServiceFactory: try find creator for service " + srvcType );

  ServiceFactory& inst = instance();
  for( auto creator : inst._d->creators )
  {
    if( srvcType == creator->serviceName() )
    {
      city::SrvcPtr srvc = creator->create( city );
      srvc->setName( name );
      return srvc;
    }
  }

  Logger::warning( "CityServiceFactory: not found creator for service " + name );
  return SrvcPtr();
}

void ServiceFactory::addCreator( ServiceCreatorPtr creator )
{
  if( creator.isNull() )
    return;

  for( auto item : _d->creators )
  {
    if( creator->serviceName() == item->serviceName() )
    {
      Logger::warning( "CityServiceFactory: Also have creator for service " + creator->serviceName() );
      return;
    }
  }

  _d->creators.push_back( creator );
}

ServiceFactory::~ServiceFactory()
{

}

ServiceFactory::ServiceFactory() : _d( new Impl )
{
}

}//end namespace city
