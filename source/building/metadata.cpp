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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "metadata.hpp"

#include <map>
#include "core/gettext.hpp"
#include "core/saveadapter.hpp"
#include "core/stringhelper.hpp"
#include "core/enumerator.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "constants.hpp"

using namespace constants;

MetaData MetaData::invalid = MetaData( building::unknown, "unknown" );

class BuildingTypeHelper : public EnumsHelper<TileOverlay::Type>
{
public:
  BuildingTypeHelper() : EnumsHelper<TileOverlay::Type>( building::unknown )
  {
    append( building::amphitheater,   "amphitheater");
    append( building::theater,        "theater" );
    append( building::hippodrome,     "hippodrome" );
    append( building::colloseum,     "colloseum" );
    append( building::actorColony,          "artist_colony" );
    append( building::gladiatorSchool,      "gladiator_pit" );
    append( building::lionHouse,           "lion_pit" );
    append( building::chariotSchool,        "chatioteer_school" );
    append( building::house,          "house" );
    append( construction::road,           "road" );
    append( construction::B_PLAZA,          "plaza" );
    append( construction::B_GARDEN,         "garden" );
    append( building::senate,         "senate_1" );
    append( building::forum,          "forum_1" );
    append( building::governorHouse, "governor_palace_1" );
    append( building::governorVilla, "governor_palace_2" );
    append( building::governorPalace,"governor_palace_3" );
    append( building::B_FORT_LEGIONNAIRE, "fort_legionaries" );
    append( building::B_FORT_JAVELIN,   "fort_javelin" );
    append( building::B_FORT_MOUNTED,   "fort_horse" );
    append( building::prefecture,        "prefecture" );
    append( building::B_BARRACKS,       "barracks" );
    append( building::B_MILITARY_ACADEMY, "military_academy" );
    append( building::B_DOCTOR,         "clinic" );
    append( building::B_HOSPITAL,       "hospital" );
    append( building::B_BATHS,          "baths" );
    append( building::B_BARBER,         "barber" );
    append( building::B_SCHOOL,         "school" );
    append( building::B_COLLEGE,        "academy" );
    append( building::B_LIBRARY,        "library" );
    append( building::B_MISSION_POST,   "mission post" );
    append( building::templeCeres,   "small_ceres_temple" );
    append( building::B_TEMPLE_NEPTUNE, "small_neptune_temple" );
    append( building::B_TEMPLE_MARS,    "small_mars_temple" );
    append( building::B_TEMPLE_MERCURE, "small_mercury_temple" );
    append( building::B_TEMPLE_VENUS,   "small_venus_temple" );
    append( building::B_BIG_TEMPLE_CERES,  "big_ceres_temple" );
    append( building::B_BIG_TEMPLE_NEPTUNE,"big_neptune_temple" );
    append( building::B_BIG_TEMPLE_MARS,   "big_mars_temple");
    append( building::B_BIG_TEMPLE_MERCURE,"big_mercury_temple");
    append( building::B_BIG_TEMPLE_VENUS,  "big_venus_temple");
    append( building::B_TEMPLE_ORACLE,     "oracle");
    append( building::B_MARKET,            "market");
    append( building::granary,        "granery");
    append( building::B_WAREHOUSE,      "warehouse");
    append( building::wheatFarm,          "wheat_farm");
    append( building::B_FRUIT_FARM,          "fig_farm");
    append( building::B_VEGETABLE_FARM,      "vegetable_farm");
    append( building::B_OLIVE_FARM,          "olive_farm");
    append( building::grapeFarm,          "vinard");
    append( building::B_PIG_FARM,           "meat_farm");
    append( building::marbleQuarry,         "quarry");
    append( building::ironMine,           "iron_mine");
    append( building::timberLogger,         "lumber_mill");
    append( building::clayPit,       "clay_pit");
    append( building::B_WINE_WORKSHOP,           "wine_workshop");
    append( building::B_OIL_WORKSHOP,            "oil_workshop");
    append( building::B_WEAPONS_WORKSHOP,         "weapons_workshop");
    append( building::B_FURNITURE,      "furniture_workshop");
    append( building::pottery,        "pottery_workshop");
    append( building::engineerPost,       "engineering_post");
    append( building::B_STATUE1,        "statue_small");
    append( building::B_STATUE2,        "statue_middle");
    append( building::B_STATUE3,        "statue_big");
    append( building::lowBridge,     "low_bridge");
    append( building::highBridge,    "high_bridge");
    append( building::B_DOCK,           "dock");
    append( building::B_SHIPYARD,       "shipyard");
    append( building::wharf,          "wharf");
    append( building::B_TRIUMPHAL_ARCH, "triumphal_arch");
    append( building::B_WELL,           "well");
    append( building::B_FOUNTAIN,       "fountain");
    append( building::B_AQUEDUCT,       "aqueduct");
    append( building::B_RESERVOIR,      "reservoir");
    append( building::B_NATIVE_HUT,     "native_hut");
    append( building::B_NATIVE_CENTER,  "native_center");
    append( building::B_NATIVE_FIELD,   "native_field");
    append( building::B_BURNING_RUINS,  "burning_ruins");
    append( building::B_BURNED_RUINS,   "burned_ruins");
    append( building::B_PLAGUE_RUINS,   "plague_ruins");
    append( building::B_COLLAPSED_RUINS,"collapsed_ruins");
    append( building::B_FORUM_2, "forum_2" );
    append( building::B_GATEHOUSE, "gatehouse" );
    append( building::B_SENATE_2, "senate_2" );
    append( building::B_TOWER, "tower" );
    append( building::B_WALL, "wall"  );
    append( building::unknown, "" );
 }
};

class BuildingClassHelper : public EnumsHelper<TileOverlay::Group>
{
public:
  BuildingClassHelper() : EnumsHelper<TileOverlay::Group>( building::unknownGroup )
  {
    append( building::industryGroup, "industry" );
    append( building::obtainGroup, "rawmaterial" );
    append( building::foodGroup, "food" );
    append( building::disasterGroup, "disaster" );
    append( building::religionGroup, "religion" );
    append( building::militaryGroup, "military" );
    append( building::nativeGroup, "native" );
    append( building::water, "water" );
    append( building::administration, "administration" );
    append( building::BC_BRIDGE, "bridge" );
    append( building::BC_ENGINEERING, "engineer" );
    append( building::BC_TRADE, "trade" );
    append( building::BC_TOWER, "tower" );
    append( building::BC_GATE, "gate" );
    append( building::BC_SECURITY, "security" );
    append( building::BC_EDUCATUION, "education" );
    append( building::BC_HEALTH, "health" );
    append( building::BC_SIGHT, "sight" );
    append( building::BC_GARDEN, "garden" );
    append( building::BC_ROAD, "road" );
    append( building::BC_ENTERTAINMENT, "entertainment" );
    append( building::BC_HOUSE, "house" );
    append( building::BC_WALL, "wall" );
    append( building::unknown, "" );
  }
};

class MetaData::Impl
{
public:
  MetaData::Desirability desirability;
  TileOverlay::Type buildingType;
  VariantMap options;
};

MetaData::MetaData(const TileOverlay::Type buildingType, const std::string& name )
  : _d( new Impl )
{
  _d->buildingType = buildingType;
  _group = building::unknownGroup;
  _name = name;
  _prettyName = _( ("##" + name + "##").c_str() );  // i18n translation
  _d->desirability.base = 0;
  _d->desirability.range = 0;
  _d->desirability.step = 0;
}

MetaData::MetaData(const MetaData &a) : _d( new Impl )
{
  *this = a;
}

MetaData::~MetaData()
{

}

std::string MetaData::getName() const
{
  return _name;
}

std::string MetaData::getPrettyName() const
{
  return _prettyName;
}

TileOverlay::Type MetaData::getType() const
{
  return _d->buildingType;
}

Picture MetaData::getBasePicture() const
{
  return _basePicture;
}

const MetaData::Desirability& MetaData::getDesirbilityInfo() const
{
  return _d->desirability;
}

Variant MetaData::getOption(const std::string &name, Variant defaultVal ) const
{
  VariantMap::iterator it = _d->options.find( name );
  return it != _d->options.end() ? it->second : defaultVal;
}

MetaData &MetaData::operator=(const MetaData &a)
{
  _d->buildingType = a._d->buildingType;
  _name = a._name;
  _prettyName = a._prettyName;
  _basePicture = a._basePicture;
  _group = a._group;
  _d->desirability = a._d->desirability;

  _d->options = a._d->options;

  return *this;
}

TileOverlay::Group MetaData::getClass() const
{
  return _group;
}

class MetaDataHolder::Impl
{
public:
  BuildingTypeHelper typeHelper;
  BuildingClassHelper classHelper;

  typedef std::map<TileOverlay::Type, MetaData> BuildingsMap;
  typedef std::map<Good::Type, TileOverlay::Type> FactoryInMap;

  BuildingsMap buildings;// key=building_type, value=data
  FactoryInMap mapBuildingByInGood;
};

MetaDataHolder& MetaDataHolder::instance()
{
  static MetaDataHolder inst;
  return inst;
}

TileOverlay::Type MetaDataHolder::getConsumerType(const Good::Type inGoodType) const
{
  TileOverlay::Type res = building::unknown;

  Impl::FactoryInMap::iterator mapIt;
  mapIt = _d->mapBuildingByInGood.find(inGoodType);
  if (mapIt != _d->mapBuildingByInGood.end())
  {
    res = mapIt->second;
  }
  return res;
}

const MetaData& MetaDataHolder::getData(const TileOverlay::Type buildingType) const
{
  Impl::BuildingsMap::iterator mapIt;
  mapIt = _d->buildings.find(buildingType);
  if (mapIt == _d->buildings.end())
  {
    Logger::warning("Unknown building %d", buildingType );
    return MetaData::invalid;
  }
  return mapIt->second;
}

bool MetaDataHolder::hasData(const TileOverlay::Type buildingType) const
{
  bool res = true;
  Impl::BuildingsMap::iterator mapIt;
  mapIt = _d->buildings.find(buildingType);
  if (mapIt == _d->buildings.end())
  {
    res = false;
  }
  return res;
}

void MetaDataHolder::addData(const MetaData &data)
{
  TileOverlay::Type buildingType = data.getType();

  if (hasData(buildingType))
  {
    Logger::warning( "Building is already set %s", data.getName().c_str() );
    return;
  }

  _d->buildings.insert(std::make_pair(buildingType, data));
}


MetaDataHolder::MetaDataHolder() : _d( new Impl )
{
}

void MetaDataHolder::initialize( const io::FilePath& filename )
{
  // populate _mapBuildingByInGood
  _d->mapBuildingByInGood[Good::iron]   = building::B_WEAPONS_WORKSHOP;
  _d->mapBuildingByInGood[Good::timber] = building::B_FURNITURE;
  _d->mapBuildingByInGood[Good::clay]   = building::pottery;
  _d->mapBuildingByInGood[Good::olive]  = building::B_OIL_WORKSHOP;
  _d->mapBuildingByInGood[Good::grape]  = building::B_WINE_WORKSHOP;

  VariantMap constructions = SaveAdapter::load( filename.toString() );

  foreach( VariantMap::value_type& mapItem, constructions )
  {
    VariantMap options = mapItem.second.toMap();

    const TileOverlay::Type btype = getType( mapItem.first );
    if( btype == building::unknown )
    {
      Logger::warning( "!!!Warning: can't associate type with %s", mapItem.first.c_str() );
      continue;
    }

    Impl::BuildingsMap::const_iterator bdataIt = _d->buildings.find( btype );
    if( bdataIt != _d->buildings.end() )
    {
      Logger::warning( "!!!Warning: type %s also initialized", mapItem.first.c_str() );
      continue;
    }

    MetaData bData( btype, mapItem.first );
    const std::string pretty = options[ "pretty" ].toString();
    if( !pretty.empty() )
    {
      bData._prettyName = pretty;
    }

    bData._d->options = options;
    VariantMap desMap = options[ "desirability" ].toMap();
    bData._d->desirability.base = (int)desMap[ "base" ];
    bData._d->desirability.range = (int)desMap[ "range" ];
    bData._d->desirability.step  = (int)desMap[ "step" ];

    Variant prettyName = options[ "prettyName" ];
    if( prettyName.isValid() )
    {
      bData._prettyName = prettyName.toString();
    }

    bData._group = getClass( options[ "class" ].toString() );

    VariantList basePic = options[ "image" ].toList();
    if( !basePic.empty() )
    {
      bData._basePicture = Picture::load( basePic.get( 0 ).toString(), basePic.get( 1 ).toInt() );
    }

    addData( bData );
  }
}

TileOverlay::Type MetaDataHolder::getType( const std::string& name )
{
  TileOverlay::Type type = instance()._d->typeHelper.findType( name );

  if( type == instance()._d->typeHelper.getInvalid() )
  {
    Logger::warning( "Can't find type for typeName %s", name.c_str() );
    _OC3_DEBUG_BREAK_IF( "Can't find type for typeName" );
  }

  return type;
}

TileOverlay::Group MetaDataHolder::getClass( const std::string& name )
{
  TileOverlay::Group type = instance()._d->classHelper.findType( name );

  if( type == instance()._d->classHelper.getInvalid() )
  {
    Logger::warning( "Can't find building class for building className %s", name.c_str() );
    _OC3_DEBUG_BREAK_IF( "Can't find building class for building className" );
  }

  return type;
}

std::string MetaDataHolder::getPrettyName(TileOverlay::Type bType)
{
  return instance().getData( bType ).getPrettyName();
}
