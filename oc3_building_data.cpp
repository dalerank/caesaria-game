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

#include "oc3_building_data.hpp"

#include <map>
#include "oc3_gettext.hpp"
#include "oc3_saveadapter.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_enums_helper.hpp"
#include "core/foreach.hpp"

BuildingData BuildingData::invalid = BuildingData( unknown, "unknown", 0 );

class BuildingTypeHelper : public EnumsHelper<TileOverlayType>
{
public:
  BuildingTypeHelper() : EnumsHelper<TileOverlayType>( unknown )
  {
    append( buildingAmphitheater,   "amphitheater");
    append( B_THEATER,        "theater" );
    append( B_HIPPODROME,     "hippodrome" );
    append( B_COLLOSSEUM,     "colloseum" );
    append( B_ACTOR_COLONY,          "artist_colony" );
    append( B_GLADIATOR_SCHOOL,      "gladiator_pit" );
    append( B_LION_HOUSE,           "lion_pit" );
    append( B_CHARIOT_MAKER,        "chatioteer_school" );
    append( B_HOUSE,          "house" );
    append( B_ROAD,           "road" );
    append( B_PLAZA,          "plaza" );
    append( B_GARDEN,         "garden" );
    append( B_SENATE,         "senate_1" );
    append( B_FORUM,          "forum_1" );
    append( B_GOVERNOR_HOUSE, "governor_palace_1" );
    append( B_GOVERNOR_VILLA, "governor_palace_2" );
    append( B_GOVERNOR_PALACE,"governor_palace_3" );
    append( B_FORT_LEGIONNAIRE, "fort_legionaries" );
    append( B_FORT_JAVELIN,   "fort_javelin" );
    append( B_FORT_MOUNTED,   "fort_horse" );
    append( B_PREFECTURE,        "prefecture" );
    append( B_BARRACKS,       "barracks" );
    append( B_MILITARY_ACADEMY, "military_academy" );
    append( B_DOCTOR,         "clinic" );
    append( B_HOSPITAL,       "hospital" );
    append( B_BATHS,          "baths" );
    append( B_BARBER,         "barber" );
    append( B_SCHOOL,         "school" );
    append( B_COLLEGE,        "academy" );
    append( B_LIBRARY,        "library" );
    append( B_MISSION_POST,   "mission post" );
    append( B_TEMPLE_CERES,   "small_ceres_temple" );
    append( B_TEMPLE_NEPTUNE, "small_neptune_temple" );
    append( B_TEMPLE_MARS,    "small_mars_temple" );
    append( B_TEMPLE_MERCURE, "small_mercury_temple" );
    append( B_TEMPLE_VENUS,   "small_venus_temple" );
    append( B_BIG_TEMPLE_CERES,  "big_ceres_temple" );
    append( B_BIG_TEMPLE_NEPTUNE,"big_neptune_temple" );
    append( B_BIG_TEMPLE_MARS,   "big_mars_temple");
    append( B_BIG_TEMPLE_MERCURE,"big_mercury_temple");
    append( B_BIG_TEMPLE_VENUS,  "big_venus_temple");
    append( B_TEMPLE_ORACLE,     "oracle");
    append( B_MARKET,            "market");
    append( B_GRANARY,        "granery");
    append( B_WAREHOUSE,      "warehouse");
    append( B_WHEAT_FARM,          "wheat_farm");
    append( B_FRUIT_FARM,          "fig_farm");
    append( B_VEGETABLE_FARM,      "vegetable_farm");
    append( B_OLIVE_FARM,          "olive_farm");
    append( B_GRAPE_FARM,          "vinard");
    append( B_PIG_FARM,           "meat_farm");
    append( B_MARBLE_QUARRY,         "quarry");
    append( B_IRON_MINE,           "iron_mine");
    append( B_TIMBER_YARD,         "lumber_mill");
    append( B_CLAY_PIT,       "clay_pit");
    append( B_WINE_WORKSHOP,           "wine_workshop");
    append( B_OIL_WORKSHOP,            "oil_workshop");
    append( B_WEAPONS_WORKSHOP,         "weapons_workshop");
    append( B_FURNITURE,      "furniture_workshop");
    append( B_POTTERY,        "pottery_workshop");
    append( B_ENGINEER_POST,       "engineering_post");
    append( B_STATUE1,        "statue_small");
    append( B_STATUE2,        "statue_middle");
    append( B_STATUE3,        "statue_big");
    append( B_LOW_BRIDGE,     "low_bridge");
    append( B_HIGH_BRIDGE,    "high_bridge");
    append( B_DOCK,           "dock");
    append( B_SHIPYARD,       "shipyard");
    append( B_WHARF,          "wharf");
    append( B_TRIUMPHAL_ARCH, "triumphal_arch");
    append( B_WELL,           "well");
    append( B_FOUNTAIN,       "fountain");
    append( B_AQUEDUCT,       "aqueduct");
    append( B_RESERVOIR,      "reservoir");
    append( B_NATIVE_HUT,     "native_hut");
    append( B_NATIVE_CENTER,  "native_center");
    append( B_NATIVE_FIELD,   "native_field");
    append( B_BURNING_RUINS,  "burning_ruins");
    append( B_BURNED_RUINS,   "burned_ruins");
    append( B_PLAGUE_RUINS,   "plague_ruins");
    append( B_COLLAPSED_RUINS,"collapsed_ruins");
    append( B_FORUM_2, "forum_2" );
    append( B_GATEHOUSE, "gatehouse" );
    append( B_SENATE_2, "senate_2" );
    append( B_TOWER, "tower" );
    append( B_WALL, "wall"  );
    append( unknown, "" );
 }
};

class BuildingClassHelper : public EnumsHelper<TileOverlayGroup>
{
public:
  BuildingClassHelper() : EnumsHelper<TileOverlayGroup>( lndcUnknown )
  {
    append( BC_INDUSTRY, "industry" );
    append( BC_RAWMATERIAL, "rawmaterial" );
    append( BC_FOOD, "food" );
    append( BC_DISASTER, "disaster" );
    append( BC_RELIGION, "religion" );
    append( BC_MILITARY, "military" );
    append( BC_NATIVE, "native" );
    append( BC_WATER, "water" );
    append( BC_ADMINISTRATION, "administration" );
    append( BC_BRIDGE, "bridge" );
    append( BC_ENGINEERING, "engineer" );
    append( BC_TRADE, "trade" );
    append( BC_TOWER, "tower" );
    append( BC_GATE, "gate" );
    append( BC_SECURITY, "security" );
    append( BC_EDUCATUION, "education" );
    append( BC_HEALTH, "health" );
    append( BC_SIGHT, "sight" );
    append( BC_GARDEN, "garden" );
    append( BC_ROAD, "road" );
    append( BC_ENTERTAINMENT, "entertainment" );
    append( BC_HOUSE, "house" );
    append( BC_WALL, "wall" );
    append( lndcUnknown, "" );
  }
};

class BuildingData::Impl
{
public:
  BuildingData::Desirability desirability;
  TileOverlayType buildingType;
  VariantMap options;
};

BuildingData::BuildingData(const TileOverlayType buildingType, const std::string &name, const int cost)
  : _d( new Impl )
{
  _d->buildingType = buildingType;
  _buildingClass = lndcUnknown;
  _name = name;
  _prettyName = _( ("##" + name + "##").c_str() );  // i18n translation
  _cost = cost;
  _d->desirability.base = 0;
  _d->desirability.range = 0;
  _d->desirability.step = 0;
}

BuildingData::BuildingData(const BuildingData &a) : _d( new Impl )
{
  *this = a;
}

BuildingData::~BuildingData()
{

}

std::string BuildingData::getName() const
{
  return _name;
}

std::string BuildingData::getPrettyName() const
{
  return _prettyName;
}

TileOverlayType BuildingData::getType() const
{
  return _d->buildingType;
}

int BuildingData::getCost() const
{
  return _cost;
}

const Picture &BuildingData::getBasePicture() const
{
  return _basePicture;
}

const BuildingData::Desirability& BuildingData::getDesirbilityInfo() const
{
  return _d->desirability;
}

Variant BuildingData::getOption(const std::string &name, Variant defaultVal ) const
{
  VariantMap::iterator it = _d->options.find( name );
  return it != _d->options.end() ? it->second : defaultVal;
}

BuildingData &BuildingData::operator=(const BuildingData &a)
{
  _d->buildingType = a._d->buildingType;
  _name = a._name;
  _prettyName = a._prettyName;
  _basePicture = a._basePicture;
  _buildingClass = a._buildingClass;
  _d->desirability = a._d->desirability;
  _cost = a._cost;

  _d->options = a._d->options;

  return *this;
}

TileOverlayGroup BuildingData::getClass() const
{
  return _buildingClass;
}

class BuildingDataHolder::Impl
{
public:
  BuildingTypeHelper typeHelper;
  BuildingClassHelper classHelper;

  typedef std::map<TileOverlayType, BuildingData> BuildingsMap; 
  typedef std::map<Good::Type, TileOverlayType> FactoryInMap;

  BuildingsMap buildings;// key=building_type, value=data
  FactoryInMap mapBuildingByInGood;
};

BuildingDataHolder& BuildingDataHolder::instance()
{
  static BuildingDataHolder inst;
  return inst;
}

TileOverlayType BuildingDataHolder::getConsumerType(const Good::Type inGoodType) const
{
  TileOverlayType res = unknown;

  Impl::FactoryInMap::iterator mapIt;
  mapIt = _d->mapBuildingByInGood.find(inGoodType);
  if (mapIt != _d->mapBuildingByInGood.end())
  {
    res = mapIt->second;
  }
  return res;
}

const BuildingData& BuildingDataHolder::getData(const TileOverlayType buildingType) const
{
  Impl::BuildingsMap::iterator mapIt;
  mapIt = _d->buildings.find(buildingType);
  if (mapIt == _d->buildings.end())
  {
    StringHelper::debug( 0xff, "Unknown building %d", buildingType );
    return BuildingData::invalid;
  }
  return mapIt->second;
}

bool BuildingDataHolder::hasData(const TileOverlayType buildingType) const
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

void BuildingDataHolder::addData(const BuildingData &data)
{
  TileOverlayType buildingType = data.getType();

  if (hasData(buildingType))
  {
    StringHelper::debug( 0xff, "Building is already set %s", data.getName().c_str() );
    return;
  }

  _d->buildings.insert(std::make_pair(buildingType, data));
}


BuildingDataHolder::BuildingDataHolder() : _d( new Impl )
{
}

void BuildingDataHolder::initialize( const io::FilePath& filename )
{
  // populate _mapBuildingByInGood
  _d->mapBuildingByInGood[Good::iron]   = B_WEAPONS_WORKSHOP;
  _d->mapBuildingByInGood[Good::timber] = B_FURNITURE;
  _d->mapBuildingByInGood[Good::clay]   = B_POTTERY;
  _d->mapBuildingByInGood[Good::olive]  = B_OIL_WORKSHOP;
  _d->mapBuildingByInGood[Good::grape]  = B_WINE_WORKSHOP;

  VariantMap constructions = SaveAdapter::load( filename.toString() );

  foreach( VariantMap::value_type& mapItem, constructions )
  {
    VariantMap options = mapItem.second.toMap();

    const TileOverlayType btype = getType( mapItem.first );
    if( btype == unknown )
    {
      StringHelper::debug( 0xff, "!!!Warning: can't associate type with %s", mapItem.first.c_str() );
      continue;
    }

    Impl::BuildingsMap::const_iterator bdataIt = _d->buildings.find( btype );
    if( bdataIt != _d->buildings.end() )
    {
      StringHelper::debug( 0xff, "!!!Warning: type %s also initialized", mapItem.first.c_str() );
      continue;
    }

    BuildingData bData( btype, mapItem.first, (int)options[ "cost" ] );
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

    bData._buildingClass = getClass( options[ "class" ].toString() );

    VariantList basePic = options[ "image" ].toList();
    if( !basePic.empty() )
    {
      bData._basePicture = Picture::load( basePic.get( 0 ).toString(), basePic.get( 1 ).toInt() );
    }

    addData( bData );
  }
}

TileOverlayType BuildingDataHolder::getType( const std::string& name )
{
  TileOverlayType type = instance()._d->typeHelper.findType( name );

  if( type == instance()._d->typeHelper.getInvalid() )
  {
    StringHelper::debug( 0xff, "Can't find type for typeName %s", name.c_str() );
    _OC3_DEBUG_BREAK_IF( "Can't find type for typeName" );
  }

  return type;
}

TileOverlayGroup BuildingDataHolder::getClass( const std::string& name )
{
  TileOverlayGroup type = instance()._d->classHelper.findType( name );

  if( type == instance()._d->classHelper.getInvalid() )
  {
    StringHelper::debug( 0xff, "Can't find building class for building className %s", name.c_str() );
    _OC3_DEBUG_BREAK_IF( "Can't find building class for building className" );
  }

  return type;
}

std::string BuildingDataHolder::getPrettyName(TileOverlayType bType)
{
  return instance().getData( bType ).getPrettyName();
}
