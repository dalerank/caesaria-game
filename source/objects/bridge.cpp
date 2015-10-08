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

#include "bridge.hpp"
#include "core/saveadapter.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include "core/priorities.hpp"

Bridge::Bridge(const object::Type type)
  : Construction( type, Size(1) )
{

}

class IdxSet : public std::set<int>
{
public:
  IdxSet& load( const VariantList& stream )
  {
    for( auto& item : stream )
    {
      switch( item.type() )
      {
      case Variant::String:
        {
          StringArray items = utils::split( item.toString(), "->" );
          int start = utils::toInt( items.valueOrEmpty( 0 ) );
          int stop = utils::toInt( items.valueOrEmpty( 1 ) );
          addRange( start, stop );
        }
      break;

      case Variant::Int:
      case Variant::UInt:
        insert( item.toInt() );
      break;

      default:
        Logger::warning( "!!! WARNING: Cant parse IdxSet from type {0}", item.type() );
      break;
      }
    }

    return *this;
  }

  void addRange( int start, int stop )
  {
    for( int k=start; k <= stop; k++ )
      insert( k );
  }
};

class BridgeConfig::Impl
{
public:
  IdxSet forbiden;
  IdxSet northA;
  IdxSet northB;
  IdxSet westA;
  IdxSet westB;
};

namespace {
 typedef std::map<object::Type, BridgeConfig> BridgeConfigs;
 static BridgeConfigs configs;
}

BridgeConfig& BridgeConfig::find( object::Type type )
{
  BridgeConfigs::iterator it = configs.find( type );
  if( it == configs.end() )
  {
    VariantMap allConfigs = config::load( ":/bridge.model" );
    std::string configName = object::toString( type );
    VariantMap configVm = allConfigs.get( configName ).toMap();

    if( !configVm.empty() )
    {
      BridgeConfig& config = configs[ type ];
      config.load( configVm );
    }
    else
    {
      Logger::warning( "!!! WARNING: Cant find bridge config for {0}:{1}", type, configName );
    }
  }

  return configs[ type ];
}

BridgeConfig::BridgeConfig(const BridgeConfig& other) : _d( new Impl )
{
  _d->forbiden = other._d->forbiden;
}

bool BridgeConfig::isForbiden(int imgid) const { return _d->forbiden.count( imgid ) > 0; }
bool BridgeConfig::isNorthA  (int imgid) const { return _d->northA.count( imgid )   > 0; }
bool BridgeConfig::isNorthB  (int imgid) const { return _d->northB.count( imgid )   > 0; }
bool BridgeConfig::isWestA   (int imgid) const { return _d->westA.count( imgid )    > 0; }
bool BridgeConfig::isWestB   (int imgid) const { return _d->westB.count( imgid )    > 0; }

BridgeConfig::BridgeConfig() : _d( new Impl )
{

}

BridgeConfig::~BridgeConfig()
{

}

void BridgeConfig::load(const VariantMap& stream)
{
  _d->forbiden.load( stream.get( "forbiden" ).toList() );
  _d->northA.load  ( stream.get( "northA"   ).toList() );
  _d->northB.load  ( stream.get( "northB"   ).toList() );
  _d->westA.load   ( stream.get( "westA"    ).toList() );
  _d->westB.load   ( stream.get( "westB"    ).toList() );
}
