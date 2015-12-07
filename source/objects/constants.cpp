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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "constants.hpp"
#include "infodb.hpp"
#include "core/logger.hpp"
#include "core/variant_list.hpp"

namespace object
{

Helper& Helper::instance()
{
  static Helper inst;
  return inst;
}

Helper::Helper() : EnumsHelper<Type>( Type(0) )
{
#define __REG_TYPE(a) append(a, TEXT(a) );
    __REG_TYPE( amphitheater )
    __REG_TYPE( theater )
    __REG_TYPE( hippodrome )
    __REG_TYPE( colloseum )
    __REG_TYPE( actorColony )
    __REG_TYPE( gladiatorSchool )
    __REG_TYPE( lionsNursery )
    __REG_TYPE( chariotSchool )
    __REG_TYPE( house )
    __REG_TYPE( road )
    __REG_TYPE( plaza )
    __REG_TYPE( garden )
    __REG_TYPE( senate )
    __REG_TYPE( forum )
    __REG_TYPE( governorHouse )
    __REG_TYPE( governorVilla )
    __REG_TYPE( governorPalace )
    __REG_TYPE( fort_legionaries )
    __REG_TYPE( fort_javelin )
    __REG_TYPE( fort_horse )
    __REG_TYPE( prefecture )
    __REG_TYPE( barracks )
    __REG_TYPE( military_academy )
    __REG_TYPE( clinic )
    __REG_TYPE( hospital )
    __REG_TYPE( baths )
    __REG_TYPE( barber )
    __REG_TYPE( school )
    __REG_TYPE( academy )
    __REG_TYPE( library )
    __REG_TYPE( missionaryPost )
    __REG_TYPE( small_ceres_temple )
    __REG_TYPE( small_neptune_temple )
    __REG_TYPE( small_mars_temple )
    __REG_TYPE( small_mercury_temple )
    __REG_TYPE( small_venus_temple )
    __REG_TYPE( big_ceres_temple )
    __REG_TYPE( big_neptune_temple )
    __REG_TYPE( big_mars_temple )
    __REG_TYPE( big_mercury_temple )
    __REG_TYPE( big_venus_temple )
    __REG_TYPE( oracle )
    __REG_TYPE( market )
    __REG_TYPE( granery )
    __REG_TYPE( warehouse )
    __REG_TYPE( wheat_farm)
    __REG_TYPE( fig_farm )
    __REG_TYPE( vegetable_farm )
    __REG_TYPE( olive_farm)
    __REG_TYPE( vinard )
    __REG_TYPE( meat_farm )
    __REG_TYPE( quarry )
    __REG_TYPE( iron_mine )
    __REG_TYPE( lumber_mill )
    __REG_TYPE( clay_pit )
    __REG_TYPE( wine_workshop )
    __REG_TYPE( oil_workshop )
    __REG_TYPE( weapons_workshop )
    __REG_TYPE( furniture_workshop )
    __REG_TYPE( pottery_workshop )
    __REG_TYPE( engineering_post )
    __REG_TYPE( statue_small )
    __REG_TYPE( statue_middle )
    __REG_TYPE( statue_big )
    __REG_TYPE( low_bridge )
    __REG_TYPE( high_bridge )
    __REG_TYPE( dock )
    __REG_TYPE( shipyard )
    __REG_TYPE( wharf )
    __REG_TYPE( triumphal_arch )
    __REG_TYPE( well )
    __REG_TYPE( fountain )
    __REG_TYPE( aqueduct )
    __REG_TYPE( reservoir )
    __REG_TYPE( native_hut )
    __REG_TYPE( native_center )
    __REG_TYPE( native_field )
    __REG_TYPE( burning_ruins )
    __REG_TYPE( burned_ruins )
    __REG_TYPE( plague_ruins )
    __REG_TYPE( wolves_den )
    __REG_TYPE( collapsed_ruins )
    __REG_TYPE( gatehouse )
    __REG_TYPE( tower )
    __REG_TYPE( wall )
    __REG_TYPE( fortification )
    __REG_TYPE( elevation )
    __REG_TYPE( rift )
    __REG_TYPE( river )
    __REG_TYPE( tree )
    __REG_TYPE( waymark )
    __REG_TYPE( terrain )
    __REG_TYPE( water )
    __REG_TYPE( meadow )
    __REG_TYPE( roadBlock )
    __REG_TYPE( farmtile )
    __REG_TYPE( coast )
    __REG_TYPE( rock )
    __REG_TYPE( plateau )
    __REG_TYPE( attackTrigger )

    append( object::unknown,        "" );
#undef __REG_TYPE
}

std::string toString(const Type& t)
{
  return Helper::instance().findName( t );
}

Type findType(const std::string &name)
{
  object::Type type = Helper::instance().findType( name );

  Logger::warningIf( type == unknown,
                     "WARNING !!! can't find type for typeName " + ( name.empty() ? "null" : name) );

  return type;
}

std::string toString( const Group& g)
{
  return object::InfoDB::instance().findGroupname( g );
}

VariantList TypeSet::save() const
{
  StringArray ret;
  for( auto& type : *this )
    ret.push_back( toString( type ) );

  return ret;
}

void TypeSet::load(const VariantList& stream)
{
  StringArray names;
  names << stream;
  for( auto& typeStr : names )
  {
    object::Type type = findType( typeStr );
    if( type != object::unknown )
    {
      insert( type );
    }
  }
}


}//end namespace object
