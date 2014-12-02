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

#include "service.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"

ServiceHelper& ServiceHelper::instance()
{
  static ServiceHelper inst;
  return inst;
}

ServiceHelper::ServiceHelper() : EnumsHelper<Service::Type>( Service::srvCount )
{
#define __REG_SERVICE(a) append( Service::a, "srvc_"CAESARIA_STR_EXT(a) );
  __REG_SERVICE( well )
  __REG_SERVICE( fountain )
  __REG_SERVICE( market )
  __REG_SERVICE( engineer )
  __REG_SERVICE( senate )
  __REG_SERVICE( forum )
  __REG_SERVICE( prefect )
  __REG_SERVICE( religionNeptune )
  __REG_SERVICE( religionCeres )
  __REG_SERVICE( religionVenus )
  __REG_SERVICE( religionMars )
  __REG_SERVICE( religionMercury )
  __REG_SERVICE( oracle )
  __REG_SERVICE( doctor )
  __REG_SERVICE( barber )
  __REG_SERVICE( baths )
  __REG_SERVICE( hospital )
  __REG_SERVICE( school )
  __REG_SERVICE( library )
  __REG_SERVICE( academy )
  __REG_SERVICE( theater )
  __REG_SERVICE( amphitheater )
  __REG_SERVICE( colloseum )
  __REG_SERVICE( hippodrome )
  __REG_SERVICE( recruter )
  __REG_SERVICE( crime )
  __REG_SERVICE( guard )
  __REG_SERVICE( missionary )
#undef __REG_SERVICE
  append( Service::srvCount, "srvc_none" );
}

Service::Type ServiceHelper::getType( const std::string& name )
{
  Service::Type type = instance().findType( name );

  if( type == instance().getInvalid() )
  {
    Logger::warning( "Can't find Service::Type for serviceName %s", name.c_str() );
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find  Service::Type for serviceName" );
  }

  return type;
}

std::string ServiceHelper::getName( Service::Type type )
{
  std::string name = instance().findName( type );

  if( name.empty() )
  {
    Logger::warning( "Can't find service typeName for %d", type );
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find service typeName by ServiceType" );
  }

  return name;
}
