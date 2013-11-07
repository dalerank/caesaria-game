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

#include "tileoverlay_factory.hpp"
#include "building/service.hpp"
#include "building/training.hpp"
#include "building/watersupply.hpp"
#include "building/warehouse.hpp"
#include "building/ruins.hpp"
#include "building/engineer_post.hpp"
#include "building/factory.hpp"
#include "building/house.hpp"
#include "building/senate.hpp"
#include "building/prefecture.hpp"
#include "building/entertainment.hpp"
#include "building/religion.hpp"
#include "road.hpp"
#include "building/market.hpp"
#include "building/granary.hpp"
#include "building/well.hpp"
#include "building/native.hpp"
#include "building/farm.hpp"
#include "building/pottery.hpp"
#include "building/low_bridge.hpp"
#include "building/high_bridge.hpp"
#include "building/clay_pit.hpp"
#include "building/marble_quarry.hpp"
#include "building/goverment.hpp"
#include "building/military.hpp"
#include "building/military_academy.hpp"
#include "building/forum.hpp"
#include "building/garden.hpp"
#include "building/health.hpp"
#include "building/metadata.hpp"
#include "building/education.hpp"
#include "fish_place.hpp"
#include "building/wharf.hpp"
#include "building/constants.hpp"
#include "constants.hpp"
#include <map>

using namespace constants;

template< class T > class BaseCreator : public TileOverlayConstructor
{
public:
  virtual TileOverlayPtr create()
  {
    TileOverlayPtr ret( new T() );
    ret->drop();

    return ret;
  }
};

template< class T > class ConstructionCreator : public BaseCreator<T>
{
public: 
  virtual TileOverlayPtr create()
  {
    TileOverlayPtr ret = BaseCreator<T>::create();

    const MetaData& info = MetaDataHolder::instance().getData( ret->getType() );
    init( ret, info );

    return ret;
  }

  virtual void init( TileOverlayPtr a, const MetaData& info )
  {
    ConstructionPtr construction = a.as<Construction>();
    if( construction == 0 )
      return;

    if( info.getBasePicture().isValid() )
    {
      construction->setPicture( info.getBasePicture() );  // default picture for build tool
    }

    VariantMap anMap = info.getOption( "animation" ).toMap();
    if( !anMap.empty() )
    {
      Animation anim;

      anim.load( anMap.get( "rc" ).toString(), anMap.get( "start" ).toInt(),
                 anMap.get( "count" ).toInt(), anMap.get( "reverse", false ).toBool(),
                 anMap.get( "step", 1 ).toInt() );
      anim.setOffset( anMap.get( "offset" ).toPoint() );
      anim.setFrameDelay( anMap.get( "delay", 1 ).toInt() );

      construction->setAnimation( anim );
    }
  }
};

template< class T > class WorkingBuildingCreator : public ConstructionCreator<T>
{
public:
  void init( TileOverlayPtr a, const MetaData& info )
  {
    ConstructionCreator<T>::init( a, info );

    WorkingBuildingPtr wb = a.as<WorkingBuilding>();
    if( wb != 0 )
    {
      wb->setMaxWorkers( (int)info.getOption( "employers" ) );
    }
  }
};

template< class T > class FactoryCreator : public WorkingBuildingCreator<T>
{
public:
  void init( TileOverlayPtr a, const MetaData& info )
  {
    WorkingBuildingCreator<T>::init( a, info );

    FactoryPtr f = a.as<Factory>();
    if( f.isValid() )
    {
      f->setProductRate( (float)info.getOption( "productRate", 9.6 ) );
    }
  }
};

class TileOverlayFactory::Impl
{
public:
  typedef std::map< TileOverlay::Type, TileOverlayConstructor* > Constructors;
  std::map< std::string, TileOverlay::Type > name2typeMap;
  Constructors constructors;
};

TileOverlayPtr TileOverlayFactory::create(const TileOverlay::Type type) const
{
  Impl::Constructors::iterator findConstructor = _d->constructors.find( type );

  if( findConstructor != _d->constructors.end() )
  {
    return findConstructor->second->create();
  }

  return TileOverlayPtr();
}

TileOverlayPtr TileOverlayFactory::create( const std::string& typeName ) const
{
  return TileOverlayPtr();
}

TileOverlayFactory& TileOverlayFactory::getInstance()
{
  static TileOverlayFactory inst;
  return inst;
}

TileOverlayFactory::TileOverlayFactory() : _d( new Impl )
{
  // entertainment
  addCreator(building::theater, OC3_STR_EXT(buildingTheater), new WorkingBuildingCreator<Theater>() );
  addCreator(building::amphitheater, OC3_STR_EXT(buildingAmphitheater), new WorkingBuildingCreator<Amphitheater>() );
  addCreator(building::colloseum, OC3_STR_EXT(B_COLLOSSEUM), new WorkingBuildingCreator<Collosseum>() );
  addCreator(building::actorColony, OC3_STR_EXT(B_ACTOR_COLONY), new WorkingBuildingCreator<ActorColony>() );
  addCreator(building::gladiatorSchool, OC3_STR_EXT(B_GLADIATOR_SCHOOL), new WorkingBuildingCreator<GladiatorSchool>() );
  addCreator(building::lionHouse, OC3_STR_EXT(B_LION_HOUSE), new WorkingBuildingCreator<LionsNursery>() );
  addCreator(building::chariotSchool, OC3_STR_EXT(B_CHARIOT_MAKER), new WorkingBuildingCreator<WorkshopChariot>() );
  addCreator(building::hippodrome, OC3_STR_EXT(B_HIPPODROME),new WorkingBuildingCreator<Hippodrome>() );
  // road&house
  addCreator(building::house, OC3_STR_EXT(B_HOUSE), new ConstructionCreator<House>() );
  addCreator(construction::B_ROAD, OC3_STR_EXT(B_ROAD) , new ConstructionCreator<Road>() );
  // administration
  addCreator(building::forum, OC3_STR_EXT(B_FORUM) , new WorkingBuildingCreator<Forum>() );
  addCreator(building::senate, OC3_STR_EXT(B_SENATE), new WorkingBuildingCreator<Senate>() );
  addCreator(building::governorHouse, OC3_STR_EXT(B_GOVERNOR_HOUSE) , new ConstructionCreator<GovernorsHouse>() );
  addCreator(building::governorVilla, OC3_STR_EXT(B_GOVERNOR_VILLA) , new ConstructionCreator<GovernorsVilla>() );
  addCreator(building::governorPalace, OC3_STR_EXT(B_GOVERNOR_PALACE), new ConstructionCreator<GovernorsPalace>() );
  addCreator(building::B_STATUE1, OC3_STR_EXT(B_STATUE1), new ConstructionCreator<SmallStatue>() );
  addCreator(building::B_STATUE2, OC3_STR_EXT(B_STATUE2), new ConstructionCreator<MediumStatue>() );
  addCreator(building::B_STATUE3, OC3_STR_EXT(B_STATUE3), new ConstructionCreator<BigStatue>() );
  addCreator(construction::B_GARDEN, OC3_STR_EXT(B_GARDEN) , new ConstructionCreator<Garden>() );
  addCreator(construction::B_PLAZA, OC3_STR_EXT(B_PLAZA)  , new ConstructionCreator<Plaza>() );
  // water
  addCreator(building::B_WELL, OC3_STR_EXT(B_WELL)     , new WorkingBuildingCreator<Well>() );
  addCreator(building::B_FOUNTAIN, OC3_STR_EXT(B_FOUNTAIN) , new WorkingBuildingCreator<Fountain>() );
  addCreator(building::B_AQUEDUCT, OC3_STR_EXT(B_AQUEDUCT), new ConstructionCreator<Aqueduct>() );
  addCreator(building::B_RESERVOIR, OC3_STR_EXT(B_RESERVOIR), new ConstructionCreator<Reservoir>() );
  // security
  addCreator(building::prefecture, OC3_STR_EXT(B_PREFECTURE)  , new WorkingBuildingCreator<Prefecture>() );
  addCreator(building::B_FORT_LEGIONNAIRE, OC3_STR_EXT(B_FORT_LEGIONNAIRE), new ConstructionCreator<FortLegionnaire>() );
  addCreator(building::B_FORT_JAVELIN, OC3_STR_EXT(B_FORT_JAVELIN)   , new ConstructionCreator<FortJaveline>() );
  addCreator(building::B_FORT_MOUNTED, OC3_STR_EXT(B_FORT_MOUNTED)  , new ConstructionCreator<FortMounted>() );
  addCreator(building::B_MILITARY_ACADEMY, OC3_STR_EXT(B_MILITARY_ACADEMY), new WorkingBuildingCreator<MilitaryAcademy>() );
  addCreator(building::B_BARRACKS, OC3_STR_EXT(B_BARRACKS)        , new ConstructionCreator<Barracks>() );
  // commerce
  addCreator(building::B_MARKET, OC3_STR_EXT(B_MARKET)  , new WorkingBuildingCreator<Market>() );
  addCreator(building::B_WAREHOUSE, OC3_STR_EXT(B_WAREHOUSE), new WorkingBuildingCreator<Warehouse>() );
  addCreator(building::granary, OC3_STR_EXT(B_GRANARY)  , new WorkingBuildingCreator<Granary>() );
  // farms
  addCreator(building::wheatFarm, OC3_STR_EXT(B_WHEAT_FARM) , new FactoryCreator<FarmWheat>() );
  addCreator(building::B_OLIVE_FARM, OC3_STR_EXT(B_OLIVE_FARM) , new FactoryCreator<FarmOlive>() );
  addCreator(building::grapeFarm, OC3_STR_EXT(B_GRAPE_FARM) , new FactoryCreator<FarmGrape>() );
  addCreator(building::B_PIG_FARM, OC3_STR_EXT(B_PIG_FARM)     , new FactoryCreator<FarmMeat>() );
  addCreator(building::B_FRUIT_FARM, OC3_STR_EXT(B_FRUIT_FARM)    , new FactoryCreator<FarmFruit>() );
  addCreator(building::B_VEGETABLE_FARM, OC3_STR_EXT(B_VEGETABLE_FARM), new FactoryCreator<FarmVegetable>() );
  // raw materials
  addCreator(building::ironMine, OC3_STR_EXT(B_IRON_MINE)  , new FactoryCreator<IronMine>() );
  addCreator(building::B_TIMBER_YARD, OC3_STR_EXT(B_TIMBER_YARD), new FactoryCreator<TimberLogger>() );
  addCreator(building::clayPit, OC3_STR_EXT(B_CLAY_PIT)  , new FactoryCreator<ClayPit>() );
  addCreator(building::B_MARBLE_QUARRY, OC3_STR_EXT(B_MARBLE_QUARRY), new FactoryCreator<MarbleQuarry>() );
  // factories
  addCreator(building::B_WEAPONS_WORKSHOP, OC3_STR_EXT(B_WEAPONS_WORKSHOP)   , new FactoryCreator<WeaponsWorkshop>() );
  addCreator(building::B_FURNITURE, OC3_STR_EXT(B_FURNITURE), new FactoryCreator<WorkshopFurniture>() );
  addCreator(building::B_WINE_WORKSHOP, OC3_STR_EXT(B_WINE_WORKSHOP)     , new FactoryCreator<Winery>() );
  addCreator(building::B_OIL_WORKSHOP, OC3_STR_EXT(B_OIL_WORKSHOP)      , new FactoryCreator<Creamery>() );
  addCreator(building::pottery, OC3_STR_EXT(B_POTTERY)  , new FactoryCreator<Pottery>() );
  // utility
  addCreator(building::engineerPost, OC3_STR_EXT(B_ENGINEER_POST), new WorkingBuildingCreator<BuildingEngineer>() );
  addCreator(building::B_LOW_BRIDGE, OC3_STR_EXT(B_LOW_BRIDGE), new ConstructionCreator<LowBridge>() );
  addCreator(building::B_HIGH_BRIDGE, OC3_STR_EXT(B_HIGH_BRIDGE), new ConstructionCreator<HighBridge>() );
  addCreator(building::B_DOCK, OC3_STR_EXT(B_DOCK)    , new ConstructionCreator<Dock>() );
  addCreator(building::B_SHIPYARD, OC3_STR_EXT(B_SHIPYARD), new ConstructionCreator<Shipyard>() );
  addCreator(building::B_WHARF, OC3_STR_EXT(B_WHARF)   , new WorkingBuildingCreator<Wharf>() );
  addCreator(building::B_TRIUMPHAL_ARCH, OC3_STR_EXT(B_TRIUMPHAL_ARCH) , new ConstructionCreator<TriumphalArch>() );
  // religion
  addCreator(building::templeCeres, OC3_STR_EXT(B_TEMPLE_CERES)  , new WorkingBuildingCreator<TempleCeres>() );
  addCreator(building::B_TEMPLE_NEPTUNE, OC3_STR_EXT(B_TEMPLE_NEPTUNE), new WorkingBuildingCreator<TempleNeptune>() );
  addCreator(building::B_TEMPLE_MARS, OC3_STR_EXT(B_TEMPLE_MARS)   , new WorkingBuildingCreator<TempleMars>() );
  addCreator(building::B_TEMPLE_VENUS, OC3_STR_EXT(B_TEMPLE_VENUS)  , new WorkingBuildingCreator<TempleVenus>() );
  addCreator(building::B_TEMPLE_MERCURE, OC3_STR_EXT(B_TEMPLE_MERCURE), new WorkingBuildingCreator<TempleMercure>() );
  addCreator(building::B_BIG_TEMPLE_CERES, OC3_STR_EXT(B_BIG_TEMPLE_CERES)  , new WorkingBuildingCreator<BigTempleCeres>() );
  addCreator(building::B_BIG_TEMPLE_NEPTUNE, OC3_STR_EXT(B_BIG_TEMPLE_NEPTUNE), new WorkingBuildingCreator<BigTempleNeptune>() );
  addCreator(building::B_BIG_TEMPLE_MARS, OC3_STR_EXT(B_BIG_TEMPLE_MARS)   , new WorkingBuildingCreator<BigTempleMars>() );
  addCreator(building::B_BIG_TEMPLE_VENUS, OC3_STR_EXT(B_BIG_TEMPLE_VENUS)  , new WorkingBuildingCreator<BigTempleVenus>() );
  addCreator(building::B_BIG_TEMPLE_MERCURE, OC3_STR_EXT(B_BIG_TEMPLE_MERCURE), new WorkingBuildingCreator<BigTempleMercure>() );
  addCreator(building::B_TEMPLE_ORACLE, OC3_STR_EXT(B_TEMPLE_ORACLE) , new WorkingBuildingCreator<TempleOracle>() );
  // health
  addCreator(building::B_BATHS, OC3_STR_EXT(B_BATHS)   , new WorkingBuildingCreator<Baths>() );
  addCreator(building::B_BARBER, OC3_STR_EXT(B_BARBER)  , new WorkingBuildingCreator<Barber>() );
  addCreator(building::B_DOCTOR, OC3_STR_EXT(B_DOCTOR)  , new WorkingBuildingCreator<Doctor>() );
  addCreator(building::B_HOSPITAL, OC3_STR_EXT(B_HOSPITAL), new WorkingBuildingCreator<Hospital>() );
  // education
  addCreator(building::B_SCHOOL, OC3_STR_EXT(B_SCHOOL) , new WorkingBuildingCreator<School>() );
  addCreator(building::B_LIBRARY, OC3_STR_EXT(B_LIBRARY), new WorkingBuildingCreator<Library>() );
  addCreator(building::B_COLLEGE, OC3_STR_EXT(B_COLLEGE), new WorkingBuildingCreator<College>() );
  addCreator(building::B_MISSION_POST, OC3_STR_EXT(B_MISSION_POST), new ConstructionCreator<MissionPost>() );
  // natives
  addCreator(building::B_NATIVE_HUT, OC3_STR_EXT(B_NATIVE_HUT)   , new ConstructionCreator<NativeHut>() );
  addCreator(building::B_NATIVE_CENTER, OC3_STR_EXT(B_NATIVE_CENTER), new ConstructionCreator<NativeCenter>() );
  addCreator(building::B_NATIVE_FIELD, OC3_STR_EXT(B_NATIVE_FIELD) , new ConstructionCreator<NativeField>() );

  //damages
  addCreator(building::B_BURNING_RUINS , OC3_STR_EXT(B_BURNING_RUINS), new ConstructionCreator<BurningRuins>() );
  addCreator(building::B_BURNED_RUINS , OC3_STR_EXT(B_BURNED_RUINS), new ConstructionCreator<BurnedRuins>() );
  addCreator(building::B_COLLAPSED_RUINS , OC3_STR_EXT(B_COLLAPSED_RUINS), new ConstructionCreator<CollapsedRuins>() );
  addCreator(building::B_PLAGUE_RUINS , OC3_STR_EXT(B_PLAGUE_RUINS), new ConstructionCreator<PlagueRuins>() );
  addCreator(place::fishPlace, OC3_STR_EXT(wtrFishPlace), new BaseCreator<FishPlace>() );
}

void TileOverlayFactory::addCreator( const TileOverlay::Type type, const std::string& typeName, TileOverlayConstructor* ctor )
{
  bool alreadyHaveConstructor = _d->name2typeMap.find( typeName ) != _d->name2typeMap.end();
  _OC3_DEBUG_BREAK_IF( alreadyHaveConstructor && "already have constructor for this type");

  if( !alreadyHaveConstructor )
  {
    _d->name2typeMap[ typeName ] = type;
    _d->constructors[ type ] = ctor;
  }
}

bool TileOverlayFactory::canCreate( const TileOverlay::Type type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
