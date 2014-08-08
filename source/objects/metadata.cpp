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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "metadata.hpp"

#include <map>
#include "core/gettext.hpp"
#include "core/saveadapter.hpp"
#include "core/stringhelper.hpp"
#include "core/enumerator.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "gfx/picture_info_bank.hpp"

using namespace constants;
using namespace gfx;

const char* MetaDataOptions::cost = "cost";
const char* MetaDataOptions::requestDestroy = "requestDestroy";
const char* MetaDataOptions::employers = "employers";

MetaData MetaData::invalid = MetaData( building::unknown, "unknown" );

class BuildingTypeHelper : public EnumsHelper<TileOverlay::Type>
{
public:
  BuildingTypeHelper() : EnumsHelper<TileOverlay::Type>( building::unknown )
  {
#define __REG_TOTYPE(a) append(building::a, CAESARIA_STR_EXT(a) );
    __REG_TOTYPE( amphitheater )
    __REG_TOTYPE( theater )
    __REG_TOTYPE( hippodrome )
    __REG_TOTYPE( colloseum )
    __REG_TOTYPE( actorColony )
    append( building::gladiatorSchool,"gladiator_pit" );
    append( building::lionsNursery,   "lion_pit" );
    append( building::chariotSchool,  "chatioteer_school" );
    __REG_TOTYPE( house )
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
    __REG_TOTYPE( prefecture )
    __REG_TOTYPE( barracks )
    append( building::militaryAcademy,"military_academy" );
    append( building::doctor,         "clinic" );
    __REG_TOTYPE( hospital )
    __REG_TOTYPE( baths )
    __REG_TOTYPE( barber )
    __REG_TOTYPE( school )
    __REG_TOTYPE( academy );
    __REG_TOTYPE( library )
    append( building::missionaryPost, "mission_post" );
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
    __REG_TOTYPE( oracle )
    __REG_TOTYPE( market )
    append( building::granary,        "granery");
    __REG_TOTYPE( warehouse )
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
    __REG_TOTYPE( dock )
    __REG_TOTYPE( shipyard )
    __REG_TOTYPE( wharf )
    append( building::triumphalArch,  "triumphal_arch");
    __REG_TOTYPE( well )
    __REG_TOTYPE( fountain )
    __REG_TOTYPE( aqueduct )
    __REG_TOTYPE( reservoir )
    append( building::nativeHut,      "native_hut");
    append( building::nativeCenter,   "native_center");
    append( building::nativeField,    "native_field");
    append( building::burningRuins,   "burning_ruins");
    append( building::burnedRuins,    "burned_ruins");
    append( building::plagueRuins,    "plague_ruins");
    append( building::collapsedRuins, "collapsed_ruins");
    append( building::forum2,         "forum_2" );
    __REG_TOTYPE( gatehouse )
    append( building::senate2,        "senate_2" );
    __REG_TOTYPE( tower )
    __REG_TOTYPE( wall )
    __REG_TOTYPE( fortification )
    __REG_TOTYPE( elevation )
    __REG_TOTYPE( rift )

    append( building::unknown,        "" );
#undef __REG_TOTYPE
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
  std::string prettyName;

  std::map< int, StringArray > pictures;
};

MetaData::MetaData(const gfx::TileOverlay::Type buildingType, const std::string& name )
  : _d( new Impl )
{
  _d->prettyName = "##" + name + "##";
  _d->tileovType = buildingType;
  _d->group = building::unknownGroup;
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

TileOverlay::Type MetaData::type() const {  return _d->tileovType;}
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

TileOverlay::Group MetaData::group() const {  return _d->group; }

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

const MetaData& MetaDataHolder::getData(const TileOverlay::Type buildingType)
{
  Impl::BuildingsMap::iterator mapIt;
  mapIt = instance()._d->buildings.find(buildingType);
  if (mapIt == instance()._d->buildings.end())
  {
    Logger::warning("MetaDataHolder::Unknown building %d", buildingType );
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

MetaDataHolder::OverlayTypes MetaDataHolder::availableTypes() const
{
  OverlayTypes ret;
  foreach( it, _d->buildings )  { ret.push_back( it->first );  }
  return ret;
}

void MetaDataHolder::addData(const MetaData &data)
{
  TileOverlay::Type buildingType = data.type();

  if (hasData(buildingType))
  {
    Logger::warning( "MetaDataHolder: Info is already set for " + data.name() );
    return;
  }

  _d->buildings.insert(std::make_pair(buildingType, data));
}


MetaDataHolder::MetaDataHolder() : _d( new Impl )
{
}

void MetaDataHolder::initialize( vfs::Path filename )
{
  // populate _mapBuildingByInGood
  _d->mapBuildingByInGood[Good::iron  ] = building::weaponsWorkshop;
  _d->mapBuildingByInGood[Good::timber] = building::furnitureWorkshop;
  _d->mapBuildingByInGood[Good::clay  ] = building::pottery;
  _d->mapBuildingByInGood[Good::olive ] = building::creamery;
  _d->mapBuildingByInGood[Good::grape ] = building::winery;

  VariantMap constructions = SaveAdapter::load( filename.toString() );

  foreach( mapItem, constructions )
  {
    VariantMap options = mapItem->second.toMap();

    const TileOverlay::Type btype = findType( mapItem->first );
    if( btype == building::unknown )
    {
      Logger::warning( "!!!Warning: can't associate type with %s", mapItem->first.c_str() );
      continue;
    }

    Impl::BuildingsMap::const_iterator bdataIt = _d->buildings.find( btype );
    if( bdataIt != _d->buildings.end() )
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
      bData._d->sound = StringHelper::format( 0xff, "%s_%05d",
                                              soundVl.get( 0 ).toString().c_str(), soundVl.get( 1 ).toInt() );
    }

    addData( bData );
  }
}

MetaDataHolder::~MetaDataHolder() {}

TileOverlay::Type MetaDataHolder::findType( const std::string& name )
{
  TileOverlay::Type type = instance()._d->typeHelper.findType( name );

  if( type == instance()._d->typeHelper.getInvalid() )
  {
    Logger::warning( "MetaDataHolder: can't find type for typeName " + ( name.empty() ? "null" : name) );
    return building::unknown;
  }

  return type;
}

std::string MetaDataHolder::findTypename(TileOverlay::Type type)
{
  return instance()._d->typeHelper.findName( type );
}

TileOverlay::Group MetaDataHolder::findGroup( const std::string& name )
{
  TileOverlay::Group type = instance()._d->classHelper.findType( name );

  if( type == instance()._d->classHelper.getInvalid() )
  {
    Logger::warning( "MetaDataHolder: can't find building class for building className %s", name.c_str() );
    return building::unknownGroup;
  }

  return type;
}

std::string MetaDataHolder::findPrettyName(TileOverlay::Type type)
{
  return instance().getData( type ).prettyName();
}

std::string MetaDataHolder::findDescription(TileOverlay::Type type)
{
  return instance().getData( type ).description();
}

Picture MetaDataHolder::randomPicture(TileOverlay::Type type, Size size)
{
  const MetaData& md = getData( type );
  return md.picture( size.width() );
}
