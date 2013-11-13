// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "service.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"

ServiceHelper& ServiceHelper::instance()
{
  static ServiceHelper inst;
  return inst;
}

ServiceHelper::ServiceHelper() : EnumsHelper<Service::Type>( Service::srvCount )
{
  append( Service::well, "srvc_well" );
  append( Service::fontain, "srvc_fonutain" );
  append( Service::market, "srvc_market" );
  append( Service::engineer, "srvc_engineer" );
  append( Service::senate, "srvc_senate" );
  append( Service::forum, "srvc_forum" );
  append( Service::prefect, "srvc_prefect" );
  append( Service::religionNeptune, "srvc_temple_neptune" );
  append( Service::religionCeres, "srvc_temple_ceres" );
  append( Service::religionVenus, "srvc_temple_venus" );
  append( Service::religionMars, "srvc_temple_mars" );
  append( Service::religionMercury, "srvc_temple_mercury" );
  append( Service::oracle, "srvc_temple_oracle" );
  append( Service::doctor, "srvc_doctor" );
  append( Service::barber, "srvc_barber" );
  append( Service::baths, "srvc_baths" );
  append( Service::hospital, "srvc_hospital" );
  append( Service::school, "srvc_school" );
  append( Service::library, "srvc_library" );
  append( Service::college, "srvc_college" );
  append( Service::theater, "srvc_theater" );
  append( Service::amphitheater, "srvc_amphitheater" );
  append( Service::colloseum, "srvc_collosseum" );
  append( Service::hippodrome, "srvc_hippodrome" );
  append( Service::burningRuins, "srvc_burning_ruins" );
  append( Service::recruter, "srvc_workers_hunter" );
  append( Service::srvCount, "srvc_none" );
}

Service::Type ServiceHelper::getType( const std::string& name )
{
  Service::Type type = instance().findType( name );

  if( type == instance().getInvalid() )
  {
    Logger::warning( "Can't find Service::Type for serviceName %s", name.c_str() );
    //_OC3_DEBUG_BREAK_IF( "Can't find  Service::Type for serviceName" );
  }

  return type;
}

std::string ServiceHelper::getName( Service::Type type )
{
  std::string name = instance().findName( type );

  if( name.empty() )
  {
    Logger::warning( "Can't find service typeName for %d", type );
    //_OC3_DEBUG_BREAK_IF( "Can't find service typeName by ServiceType" );
  }

  return name;
}
