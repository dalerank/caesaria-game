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
    append( building::colloseum,      "colloseum" );
    append( building::actorColony,    "artist_colony" );
    append( building::gladiatorSchool,"gladiator_pit" );
    append( building::lionsNursery,   "lion_pit" );
    append( building::chariotSchool,  "chatioteer_school" );
    append( building::house,          "house" );
    append( construction::road,       "road" );
    append( construction::plaza,      "plaza" );
    append( construction::garden,     "garden" );
    append( building::senate,         "senate_1" );
    append( building::forum,          "forum_1" );
    append( building::governorHouse,  "governor_palace_1" );
    append( building::governorVilla,  "governor_palace_2" );
    append( building::governorPalace, "governor_palace_3" );
    append( building::fortLegionaire, "fort_legionaries" );
    append( building::fortJavelin,    "fort_javelin" );
    append( building::fortMounted,    "fort_horse" );
    append( building::prefecture,     "prefecture" );
    append( building::barracks,       "barracks" );
    append( building::militaryAcademy,"military_academy" );
    append( building::doctor,         "clinic" );
    append( building::hospital,       "hospital" );
    append( building::baths,          "baths" );
    append( building::barber,         "barber" );
    append( building::school,         "school" );
    append( building::academy,        "academy" );
    append( building::library,        "library" );
    append( building::missionaryPost, "mission post" );
    append( building::templeCeres,    "small_ceres_temple" );
    append( building::templeNeptune,  "small_neptune_temple" );
    append( building::templeMars,     "small_mars_temple" );
    append( building::templeMercury,  "small_mercury_temple" );
    append( building::templeVenus,    "small_venus_temple" );
    append( building::cathedralCeres,  "big_ceres_temple" );
    append( building::cathedralNeptune,"big_neptune_temple" );
    append( building::cathedralMars,   "big_mars_temple");
    append( building::cathedralMercury,"big_mercury_temple");
    append( building::cathedralVenus,  "big_venus_temple");
    append( building::oracle,         "oracle");
    append( building::market,         "market");
    append( building::granary,        "granery");
    append( building::warehouse,      "warehouse");
    append( building::wheatFarm,      "wheat_farm");
    append( building::fruitFarm,      "fig_farm");
    append( building::vegetableFarm,  "vegetable_farm");
    append( building::oliveFarm,      "olive_farm");
    append( building::grapeFarm,      "vinard");
    append( building::pigFarm,        "meat_farm");
    append( building::marbleQuarry,   "quarry");
    append( building::ironMine,       "iron_mine");
    append( building::timberLogger,   "lumber_mill");
    append( building::clayPit,        "clay_pit");
    append( building::winery,         "wine_workshop");
    append( building::creamery,       "oil_workshop");
    append( building::weaponsWorkshop,"weapons_workshop");
    append( building::furnitureWorkshop, "furniture_workshop");
    append( building::pottery,        "pottery_workshop");
    append( building::engineerPost,   "engineering_post");
    append( building::smallStatue,    "statue_small");
    append( building::middleStatue,   "statue_middle");
    append( building::bigStatue,      "statue_big");
    append( building::lowBridge,      "low_bridge");
    append( building::highBridge,     "high_bridge");
    append( building::dock,           "dock");
    append( building::shipyard,       "shipyard");
    append( building::wharf,          "wharf");
    append( building::triumphalArch,  "triumphal_arch");
    append( building::well,           "well");
    append( building::fountain,       "fountain");
    append( building::aqueduct,       "aqueduct");
    append( building::reservoir,      "reservoir");
    append( building::nativeHut,      "native_hut");
    append( building::nativeCenter,   "native_center");
    append( building::nativeField,    "native_field");
    append( building::burningRuins,   "burning_ruins");
    append( building::burnedRuins,    "burned_ruins");
    append( building::plagueRuins,    "plague_ruins");
    append( building::collapsedRuins, "collapsed_ruins");
    append( building::forum2,         "forum_2" );
    append( building::gatehouse,      "gatehouse" );
    append( building::senate2,        "senate_2" );
    append( building::tower,          "tower" );
    append( building::wall,           "wall"  );
    append( building::fortification,  "fortification" );
    append( building::elevation, "elevation" );
    append( building::unknown,        "" );
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
    append( building::waterGroup, "water" );
    append( building::administrationGroup, "administration" );
    append( building::bridgeGroup, "bridge" );
    append( building::engineeringGroup, "engineer" );
    append( building::tradeGroup, "trade" );
    append( building::tower, "tower" );
    append( building::gateGroup, "gate" );
    append( building::securityGroup, "security" );
    append( building::educationGroup, "education" );
    append( building::healthGroup, "health" );
    append( building::sightGroup, "sight" );
    append( building::gardenGroup, "garden" );
    append( building::roadGroup, "road" );
    append( building::entertainmentGroup, "entertainment" );
    append( building::houseGroup, "house" );
    append( building::wallGroup, "wall" );
    append( building::unknown, "" );
  }
};

class MetaData::Impl
{
public:
  Desirability desirability;
  TileOverlay::Type tileovType;
  TileOverlay::Group group;
  std::string name;  // debug name  (english, ex:"iron")
  std::string sound;
  StringArray desc;
  VariantMap options;
};

MetaData::MetaData(const TileOverlay::Type buildingType, const std::string& name )
  : _d( new Impl )
{
  _d->tileovType = buildingType;
  _d->group = building::unknownGroup;
  _d->name = name;
  _prettyName = "##" + name + "##";  // i18n translation
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
  return _d->name;
}

std::string MetaData::getSound() const
{
  return _d->sound;
}

std::string MetaData::getPrettyName() const
{
  return _prettyName;
}

std::string MetaData::getDescription() const
{
  if( _d->desc.empty() )
    return "##" + _d->name + "_info##";

  return _d->desc.at( rand() % _d->desc.size() );
}

TileOverlay::Type MetaData::getType() const
{
  return _d->tileovType;
}

Picture MetaData::getBasePicture() const
{
  return _basePicture;
}

Desirability MetaData::getDesirbility() const
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
  _d->tileovType = a._d->tileovType;
  _d->name = a._d->name;
  _prettyName = a._prettyName;
  _d->sound = a._d->sound;
  _basePicture = a._basePicture;
  _d->group = a._d->group;
  _d->desirability = a._d->desirability;
  _d->desc = a._d->desc;
  _d->options = a._d->options;

  return *this;
}

TileOverlay::Group MetaData::getGroup() const
{
  return _d->group;
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

void MetaDataHolder::initialize( const vfs::Path& filename )
{
  // populate _mapBuildingByInGood
  _d->mapBuildingByInGood[Good::iron  ] = building::weaponsWorkshop;
  _d->mapBuildingByInGood[Good::timber] = building::furnitureWorkshop;
  _d->mapBuildingByInGood[Good::clay  ] = building::pottery;
  _d->mapBuildingByInGood[Good::olive ] = building::creamery;
  _d->mapBuildingByInGood[Good::grape ] = building::winery;

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

    bData._d->options = options;
    VariantMap desMap = options[ "desirability" ].toMap();
    bData._d->desirability.base = (int)desMap[ "base" ];
    bData._d->desirability.range = (int)desMap[ "range" ];
    bData._d->desirability.step  = (int)desMap[ "step" ];

    bData._d->desc = options.get( "desc" ).toStringArray();

    Variant prettyName = options.get( "prettyName" );
    if( prettyName.isValid() )
    {
      bData._prettyName = prettyName.toString();
    }

    bData._d->group = getClass( options[ "class" ].toString() );

    VariantList basePic = options[ "image" ].toList();
    if( !basePic.empty() )
    {
      bData._basePicture = Picture::load( basePic.get( 0 ).toString(), basePic.get( 1 ).toInt() );
    }

    VariantList soundVl = options[ "sound" ].toList();
    if( !soundVl.empty() )
    {
      bData._d->sound = StringHelper::format( 0xff, "%s_%05d.wav",
                                              soundVl.get( 0 ).toString().c_str(), soundVl.get( 1 ).toInt() );
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
    return building::unknown;
  }

  return type;
}

std::string MetaDataHolder::getTypename(TileOverlay::Type type)
{
  return instance()._d->typeHelper.findName( type );
}

TileOverlay::Group MetaDataHolder::getClass( const std::string& name )
{
  TileOverlay::Group type = instance()._d->classHelper.findType( name );

  if( type == instance()._d->classHelper.getInvalid() )
  {
    Logger::warning( "Can't find building class for building className %s", name.c_str() );
    return building::unknownGroup;
  }

  return type;
}

std::string MetaDataHolder::getPrettyName(TileOverlay::Type type)
{
  return instance().getData( type ).getPrettyName();
}

std::string MetaDataHolder::getDescription(TileOverlay::Type type)
{
  return instance().getData( type ).getDescription();
}
