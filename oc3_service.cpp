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

#include "oc3_service.hpp"
#include "oc3_stringhelper.hpp"

ServiceHelper& ServiceHelper::instance()
{
  static ServiceHelper inst;
  return inst;
}

ServiceHelper::ServiceHelper() : EnumsHelper<Service::Type>( Service::S_MAX )
{
  append( Service::well, "srvc_well" );
  append( Service::fontain, "srvc_fonutain" );
  append( Service::S_MARKET, "srvc_market" );
  append( Service::engineer, "srvc_engineer" );
  append( Service::S_SENATE, "srvc_senate" );
  append( Service::S_FORUM, "srvc_forum" );
  append( Service::S_PREFECT, "srvc_prefect" );
  append( Service::S_TEMPLE_NEPTUNE, "srvc_temple_neptune" );
  append( Service::S_TEMPLE_CERES, "srvc_temple_ceres" );
  append( Service::S_TEMPLE_VENUS, "srvc_temple_venus" );
  append( Service::S_TEMPLE_MARS, "srvc_temple_mars" );
  append( Service::S_TEMPLE_MERCURE, "srvc_temple_mercury" );
  append( Service::S_TEMPLE_ORACLE, "srvc_temple_oracle" );
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
  append( Service::S_BURNING_RUINS, "srvc_burning_ruins" );
  append( Service::S_WORKERS_HUNTER, "srvc_workers_hunter" );
  append( Service::S_MAX, "srvc_none" );
}

Service::Type ServiceHelper::getType( const std::string& name )
{
  Service::Type type = instance().findType( name );

  if( type == instance().getInvalid() )
  {
    StringHelper::debug( 0xff, "Can't find Service::Type for serviceName %s", name.c_str() );
    //_OC3_DEBUG_BREAK_IF( "Can't find  Service::Type for serviceName" );
  }

  return type;
}

std::string ServiceHelper::getName( Service::Type type )
{
  std::string name = instance().findName( type );

  if( name.empty() )
  {
    StringHelper::debug( 0xff, "Can't find service typeName for %d", type );
    //_OC3_DEBUG_BREAK_IF( "Can't find service typeName by ServiceType" );
  }

  return name;
}
