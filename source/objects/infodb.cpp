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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2015 dalerank, dalerankn8@gmail.com

#include "infodb.hpp"

#include <map>
#include "core/gettext.hpp"
#include "core/saveadapter.hpp"
#include "core/utils.hpp"
#include "core/enumerator.hpp"
#include "core/foreach.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "gfx/picture_info_bank.hpp"
#include "core/variant_list.hpp"

using namespace gfx;

namespace object
{

class BuildingClassHelper : public EnumsHelper<object::Group>
{
public:
  BuildingClassHelper() : EnumsHelper<object::Group>( object::group::unknown )
  {
#define __REG_GROUPNAME(a) append( object::group::a, CAESARIA_STR_EXT(a) );
    __REG_GROUPNAME(industry)
    __REG_GROUPNAME(food)
    __REG_GROUPNAME(disaster)
    __REG_GROUPNAME(religion)
    __REG_GROUPNAME(military)
    __REG_GROUPNAME(native)
    __REG_GROUPNAME(water)
    __REG_GROUPNAME(administration)
    __REG_GROUPNAME(bridge)
    __REG_GROUPNAME(trade)
    __REG_GROUPNAME(tower)
    __REG_GROUPNAME(gate)
    __REG_GROUPNAME(security)
    __REG_GROUPNAME(education)
    __REG_GROUPNAME(health)
    __REG_GROUPNAME(sight)
    __REG_GROUPNAME(garden)
    __REG_GROUPNAME(road)
    __REG_GROUPNAME(entertainment)
    __REG_GROUPNAME(house)
    __REG_GROUPNAME(wall)
#undef __REG_GROUPNAME
    append( object::group::unknown, "" );
    append( object::group::obtain, "rawmaterial" );
    append( object::group::engineering, "engineer" );
  }
};

void InfoDB::_loadConfig(object::Type type, const std::string& name, const VariantMap& options, bool force )
{
  Info bData( type, name );
  bData.initialize( options );

  addData( bData, force );
}

class ObjectsMap : public std::map<object::Type, Info>
{
public:
  const Info& valueOrEmpty( object::Type type )
  {
    ObjectsMap::const_iterator mapIt = find( type );
    if( mapIt == end() )
    {
      Logger::warning("MetaDataHolder::Unknown objects {}", type );
      return Info::invalid;
    }
    return mapIt->second;
  }
};

class InfoDB::Impl
{
public:
  BuildingClassHelper classHelper;

  typedef std::map<good::Product, object::Type> FactoryInMap;

  ObjectsMap objectsInfo;// key=building_type, value=data
  vfs::Path configFile;
  FactoryInMap mapBuildingByInGood;
};

InfoDB& InfoDB::instance()
{
  static InfoDB inst;
  return inst;
}

object::Type InfoDB::getConsumerType(const good::Product inGoodType) const
{
  object::Type res = object::unknown;

  Impl::FactoryInMap::iterator mapIt;
  mapIt = _d->mapBuildingByInGood.find(inGoodType);
  if (mapIt != _d->mapBuildingByInGood.end())
  {
    res = mapIt->second;
  }
  return res;
}

const Info& InfoDB::find(const object::Type buildingType)
{
  return instance()._d->objectsInfo.valueOrEmpty( buildingType );
}

bool InfoDB::hasData(const object::Type buildingType) const
{
  return _d->objectsInfo.count( buildingType ) > 0;
}

object::Types InfoDB::availableTypes() const
{
  object::Types ret;
  for( auto& info : _d->objectsInfo ) { ret.push_back( info.first );  }
  return ret;
}

void InfoDB::reload(const object::Type type)
{
  VariantMap constructions = config::load( _d->configFile );

  std::string nameType = object::toString( type );
  VariantMap config = constructions.get( nameType ).toMap();

  if( !config.empty() )
  {
    _loadConfig( type, nameType, config, true );
  }
}

void InfoDB::addData(const Info& data, bool force )
{
  object::Type buildingType = data.type();

  if( force )
    _d->objectsInfo.erase( buildingType );

  if( hasData(buildingType) )
  {
    Logger::warning( "MetaDataHolder: Info is already set for " + data.name() );
    return;
  }

  _d->objectsInfo.insert( std::make_pair(buildingType,data) );
}


InfoDB::InfoDB() : _d( new Impl )
{
}

void InfoDB::initialize( vfs::Path filename )
{
  // populate _mapBuildingByInGood
  _d->configFile = filename;
  _d->mapBuildingByInGood[good::iron  ] = object::weapons_workshop;
  _d->mapBuildingByInGood[good::timber] = object::furniture_workshop;
  _d->mapBuildingByInGood[good::clay  ] = object::pottery_workshop;
  _d->mapBuildingByInGood[good::olive ] = object::oil_workshop;
  _d->mapBuildingByInGood[good::grape ] = object::wine_workshop;

  VariantMap constructions = config::load( filename );

  for( auto& kv : constructions )
  {
    const object::Type btype = object::findType( kv.first );

    if( btype == object::unknown )
    {
      Logger::warning( "!!!WARNING: can't associate type with " + kv.first );
      continue;
    }

    if( hasData( btype ) )
    {
      Logger::warning( "!!!WARNING: type {} also initialized ", kv.first );
      continue;
    }

    _loadConfig( btype, kv.first, kv.second.toMap(), false );
  }
}

InfoDB::~InfoDB() {}

object::Group InfoDB::findGroup( const std::string& name )
{
  object::Group type = instance()._d->classHelper.findType( name );

  if( type == instance()._d->classHelper.getInvalid() )
  {
    Logger::warning( "!!! MetaDataHolder: can't find object class for className " + name );
    return object::group::unknown;
  }

  return type;
}

std::string InfoDB::findGroupname(object::Group group)
{
  return instance()._d->classHelper.findName( group );
}

std::string InfoDB::findPrettyName(object::Type type)
{
  return instance().find( type ).prettyName();
}

std::string InfoDB::findDescription(object::Type type)
{
  return instance().find( type ).description();
}

}//end namespace object
