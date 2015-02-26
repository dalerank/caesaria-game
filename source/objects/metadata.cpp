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

#include "metadata.hpp"

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

using namespace gfx;

const char* MetaDataOptions::cost = "cost";
const char* MetaDataOptions::requestDestroy = "requestDestroy";
const char* MetaDataOptions::employers = "employers";
const char* MetaDataOptions::c3logic = "c3logic";

MetaData MetaData::invalid = MetaData( object::unknown, "unknown" );

class BuildingClassHelper : public EnumsHelper<object::Group>
{
public:
  BuildingClassHelper() : EnumsHelper<object::Group>( object::group::unknown )
  {
    append( object::group::industry, "industry" );
    append( object::group::obtain, "rawmaterial" );
    append( object::group::food, "food" );
    append( object::group::disaster, "disaster" );
    append( object::group::religion, "religion" );
    append( object::group::military, "military" );
    append( object::group::native, "native" );
    append( object::group::water, "water" );
    append( object::group::administration, "administration" );
    append( object::group::bridge, "bridge" );
    append( object::group::engineering, "engineer" );
    append( object::group::trade, "trade" );
    append( object::group::tower, "tower" );
    append( object::group::gate, "gate" );
    append( object::group::security, "security" );
    append( object::group::education, "education" );
    append( object::group::health, "health" );
    append( object::group::sight, "sight" );
    append( object::group::garden, "garden" );
    append( object::group::road, "road" );
    append( object::group::entertainment, "entertainment" );
    append( object::group::house, "house" );
    append( object::group::wall, "wall" );
    append( object::group::unknown, "" );
  }
};

class MetaData::Impl
{
public:
  Desirability desirability;
  object::Type tileovType;
  object::Group group;
  std::string name;  // debug name  (english, ex:"iron")
  std::string sound;
  StringArray desc;
  VariantMap options;
  std::string prettyName;

  std::map< int, StringArray > pictures;
};

MetaData::MetaData(const object::Type buildingType, const std::string& name )
  : _d( new Impl )
{
  _d->prettyName = "##" + name + "##";
  _d->tileovType = buildingType;
  _d->group = object::group::unknown;
  _d->name = name;  
}

MetaData::MetaData(const MetaData &a) : _d( new Impl )
{
  *this = a;
}

MetaData::~MetaData(){}
std::string MetaData::name() const{  return _d->name;}
std::string MetaData::sound() const{  return _d->sound;}
std::string MetaData::prettyName() const {  return _d->prettyName;}

std::string MetaData::description() const
{
  if( _d->desc.empty() )
    return "##" + _d->name + "_info##";

  return _d->desc[ rand() % _d->desc.size() ];
}

object::Type MetaData::type() const {  return _d->tileovType;}
Desirability MetaData::desirability() const{  return _d->desirability;}

Picture MetaData::picture(int size) const
{
  StringArray& array = _d->pictures[ size ];
  return Picture::load( array.random() );
}

Variant MetaData::getOption(const std::string &name, Variant defaultVal ) const
{
  VariantMap::iterator it = _d->options.find( name );
  return it != _d->options.end() ? it->second : defaultVal;
}

MetaData& MetaData::operator=(const MetaData &a)
{
  _d->tileovType = a._d->tileovType;
  _d->name = a._d->name;
  _d->prettyName = a._d->prettyName;
  _d->sound = a._d->sound;
  _d->pictures = a._d->pictures;
  _d->group = a._d->group;
  _d->desirability = a._d->desirability;
  _d->desc = a._d->desc;
  _d->options = a._d->options;

  return *this;
}

object::Group MetaData::group() const {  return _d->group; }

class MetaDataHolder::Impl
{
public:
  BuildingClassHelper classHelper;

  typedef std::map<object::Type, MetaData> ObjectsMap;
  typedef std::map<good::Product, object::Type> FactoryInMap;

  ObjectsMap objectsInfo;// key=building_type, value=data
  FactoryInMap mapBuildingByInGood;
};

MetaDataHolder& MetaDataHolder::instance()
{
  static MetaDataHolder inst;
  return inst;
}

object::Type MetaDataHolder::getConsumerType(const good::Product inGoodType) const
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

const MetaData& MetaDataHolder::getData(const object::Type buildingType)
{
  Impl::ObjectsMap::iterator mapIt;
  mapIt = instance()._d->objectsInfo.find(buildingType);
  if (mapIt == instance()._d->objectsInfo.end())
  {
    Logger::warning("MetaDataHolder::Unknown objects %d", buildingType.toInt() );
    return MetaData::invalid;
  }
  return mapIt->second;
}

bool MetaDataHolder::hasData(const object::Type buildingType) const
{
  bool res = true;
  Impl::ObjectsMap::iterator mapIt;
  mapIt = _d->objectsInfo.find(buildingType);
  if (mapIt == _d->objectsInfo.end())
  {
    res = false;
  }
  return res;
}

MetaDataHolder::OverlayTypes MetaDataHolder::availableTypes() const
{
  OverlayTypes ret;
  foreach( it, _d->objectsInfo ) { ret.push_back( it->first );  }
  return ret;
}

void MetaDataHolder::addData(const MetaData &data)
{
  object::Type buildingType = data.type();

  if (hasData(buildingType))
  {
    Logger::warning( "MetaDataHolder: Info is already set for " + data.name() );
    return;
  }

  _d->objectsInfo.insert(std::make_pair(buildingType, data));
}


MetaDataHolder::MetaDataHolder() : _d( new Impl )
{
}

void MetaDataHolder::initialize( vfs::Path filename )
{
  // populate _mapBuildingByInGood
  _d->mapBuildingByInGood[good::iron  ] = object::weapons_workshop;
  _d->mapBuildingByInGood[good::timber] = object::furniture_workshop;
  _d->mapBuildingByInGood[good::clay  ] = object::pottery_workshop;
  _d->mapBuildingByInGood[good::olive ] = object::oil_workshop;
  _d->mapBuildingByInGood[good::grape ] = object::wine_workshop;

  VariantMap constructions = config::load( filename );

  foreach( mapItem, constructions )
  {
    VariantMap options = mapItem->second.toMap();

    const object::Type btype = findType( mapItem->first );
    if( btype == object::unknown )
    {
      Logger::warning( "!!!Warning: can't associate type with %s", mapItem->first.c_str() );
      continue;
    }

    Impl::ObjectsMap::const_iterator bdataIt = _d->objectsInfo.find( btype );
    if( bdataIt != _d->objectsInfo.end() )
    {
      Logger::warning( "!!!Warning: type %s also initialized", mapItem->first.c_str() );
      continue;
    }

    MetaData bData( btype, mapItem->first );

    bData._d->options = options;
    VariantMap desMap = options[ "desirability" ].toMap();
    bData._d->desirability.VARIANT_LOAD_ANY(base, desMap );
    bData._d->desirability.VARIANT_LOAD_ANY(range, desMap);
    bData._d->desirability.VARIANT_LOAD_ANY(step, desMap );

    bData._d->desc = options.get( "desc" ).toStringArray();
    bData._d->prettyName = options.get( "prettyName", Variant( bData._d->prettyName ) ).toString();

    bData._d->group = findGroup( options[ "class" ].toString() );

    VariantList basePic = options[ "image" ].toList();
    if( !basePic.empty() )
    {
      std::string groupName = basePic.get( 0 ).toString();
      int imageIndex = basePic.get( 1 ).toInt();
      Variant vOffset = options[ "image.offset" ];
      if( vOffset.isValid() )
      {
        PictureInfoBank::instance().setOffset( groupName, imageIndex, vOffset.toPoint() );
      }

      Picture pic = Picture::load( groupName, imageIndex );
      bData._d->pictures[ 0 ] << pic.name();
    }

    VariantMap extPics = options[ "image.ext" ].toMap();
    foreach( it, extPics )
    {
      VariantMap info = it->second.toMap();
      VARIANT_INIT_ANY( int, size, info )
      VARIANT_INIT_ANY( int, start, info );
      VARIANT_INIT_ANY( int, count, info );
      VARIANT_INIT_STR( rc, info );
      for( int i=0; i < count; i++ )
      {
        Picture pic = Picture::load( rc, start + i );
        if( pic.isValid() )
        {
          bData._d->pictures[ size ] << pic.name();
        }
      }
    }

    VariantList soundVl = options[ "sound" ].toList();
    if( !soundVl.empty() )
    {
      bData._d->sound = utils::format( 0xff, "%s_%05d",
                                              soundVl.get( 0 ).toString().c_str(), soundVl.get( 1 ).toInt() );
    }

    addData( bData );
  }
}

MetaDataHolder::~MetaDataHolder() {}

std::string MetaDataHolder::findTypename(object::Type type)
{
  return instance()._d->typeHelper.findName( type );
}

object::Group MetaDataHolder::findGroup( const std::string& name )
{
  object::Group type = instance()._d->classHelper.findType( name );

  if( type == instance()._d->classHelper.getInvalid() )
  {
    Logger::warning( "MetaDataHolder: can't find object class for className %s", name.c_str() );
    return object::group::unknown;
  }

  return type;
}

std::string MetaDataHolder::findGroupname(object::Group group)
{
  return instance()._d->classHelper.findName( group );
}

std::string MetaDataHolder::findPrettyName(object::Type type)
{
  return instance().getData( type ).prettyName();
}

std::string MetaDataHolder::findDescription(object::Type type)
{
  return instance().getData( type ).description();
}

Picture MetaDataHolder::randomPicture(object::Type type, Size size)
{
  const MetaData& md = getData( type );
  return md.picture( size.width() );
}
