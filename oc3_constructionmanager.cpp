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

#include "oc3_constructionmanager.hpp"
#include "oc3_building_service.hpp"
#include "oc3_building_training.hpp"
#include "oc3_building_watersupply.hpp"
#include "oc3_building_warehouse.hpp"
#include "oc3_building_ruins.hpp"
#include "oc3_building_engineer_post.hpp"
#include "oc3_building_factory.hpp"
#include "oc3_building_house.hpp"
#include "oc3_building_senate.hpp"
#include "oc3_building_prefecture.hpp"
#include "oc3_building_entertainment.hpp"
#include "oc3_building_religion.hpp"
#include "oc3_road.hpp"
#include "oc3_building_market.hpp"
#include "oc3_building_granary.hpp"
#include "oc3_building_well.hpp"
#include "oc3_building_native.hpp"
#include "oc3_building_farm.hpp"
#include "oc3_building_pottery.hpp"

#include "oc3_building_low_bridge.hpp"
#include "oc3_building_high_bridge.hpp"

#include "oc3_building_clay_pit.hpp"
#include "oc3_building_marble_quarry.hpp"
#include "oc3_goverment_building.hpp"
#include "oc3_building_military.hpp"
#include "oc3_building_academy.hpp"
#include "oc3_building_forum.hpp"
#include "oc3_building_garden.hpp"
#include "oc3_building_health.hpp"
#include "oc3_building_data.hpp"
#include "oc3_building_education.hpp"

#include <map>

template< class T > class BaseBuildingCreator : public BuildingCreator
{
public:
  Construction* create( const BuildingData& info )
  {
    return new T();
  }
};

template< class T > class WorkingBuildingCreator : public BuildingCreator
{
public:
  Construction* create( const BuildingData& info )
  {
    WorkingBuilding* wb = new T();

    wb->setMaxWorkers( (int)info.getOption( "employers" ) );

    return wb;
  }
};

template< class T > class FactoryCreator : public BuildingCreator
{
  Construction* create( const BuildingData& info )
  {
    Factory* wb = new T();

    wb->setMaxWorkers( (int)info.getOption( "employers", 0 ) );
    wb->setProductRate( (float)info.getOption( "productRate", 9.6 ) );

    return wb;
  }
};

class ConstructionManager::Impl
{
public:
  typedef std::map< BuildingType, BuildingCreator* > BuildingCreators;
  std::map< std::string, BuildingType > name2typeMap;
  BuildingCreators constructors;
};

ConstructionPtr ConstructionManager::create(const BuildingType buildingType) const
{
  Impl::BuildingCreators::iterator findConstructor = _d->constructors.find( buildingType );

  if( findConstructor != _d->constructors.end() )
  {
    const BuildingData& info = BuildingDataHolder::instance().getData( buildingType );

    ConstructionPtr ret( findConstructor->second->create( info ) );

    if( info.getBasePicture().isValid() )
    {
      ret->setPicture( info.getBasePicture() );  // default picture for build tool
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

      ret->setAnimation( anim );
    }

    ret->drop();
    return ret;
  }

  return ConstructionPtr();
}

ConstructionPtr ConstructionManager::create( const std::string& typeName ) const
{
  return ConstructionPtr();
}

ConstructionManager& ConstructionManager::getInstance()
{
  static ConstructionManager inst;
  return inst;
}

ConstructionManager::ConstructionManager() : _d( new Impl )
{
  // entertainment
  addCreator(B_THEATER, OC3_STR_EXT(B_THEATER), new WorkingBuildingCreator<Theater>() );
  addCreator(B_AMPHITHEATER, OC3_STR_EXT(B_AMPHITHEATER), new WorkingBuildingCreator<Amphitheater>() );
  addCreator(B_COLLOSSEUM, OC3_STR_EXT(B_COLLOSSEUM), new WorkingBuildingCreator<Collosseum>() );
  addCreator(B_ACTOR_COLONY, OC3_STR_EXT(B_ACTOR_COLONY), new WorkingBuildingCreator<ActorColony>() );
  addCreator(B_GLADIATOR_SCHOOL, OC3_STR_EXT(B_GLADIATOR_SCHOOL), new WorkingBuildingCreator<GladiatorSchool>() );
  addCreator(B_LION_HOUSE, OC3_STR_EXT(B_LION_HOUSE), new WorkingBuildingCreator<LionsNursery>() );
  addCreator(B_CHARIOT_MAKER, OC3_STR_EXT(B_CHARIOT_MAKER), new WorkingBuildingCreator<WorkshopChariot>() );
  addCreator(B_HIPPODROME, OC3_STR_EXT(B_HIPPODROME),new WorkingBuildingCreator<Hippodrome>() );
  // road&house
  addCreator(B_HOUSE, OC3_STR_EXT(B_HOUSE), new BaseBuildingCreator<House>() );
  addCreator(B_ROAD, OC3_STR_EXT(B_ROAD) , new BaseBuildingCreator<Road>() );
  // administration
  addCreator(B_FORUM, OC3_STR_EXT(B_FORUM) , new WorkingBuildingCreator<Forum>() );
  addCreator(B_SENATE, OC3_STR_EXT(B_SENATE), new WorkingBuildingCreator<Senate>() );
  addCreator(B_GOVERNOR_HOUSE, OC3_STR_EXT(B_GOVERNOR_HOUSE) , new BaseBuildingCreator<GovernorsHouse>() );
  addCreator(B_GOVERNOR_VILLA, OC3_STR_EXT(B_GOVERNOR_VILLA) , new BaseBuildingCreator<GovernorsVilla>() );
  addCreator(B_GOVERNOR_PALACE, OC3_STR_EXT(B_GOVERNOR_PALACE), new BaseBuildingCreator<GovernorsPalace>() );
  addCreator(B_STATUE1, OC3_STR_EXT(B_STATUE1), new BaseBuildingCreator<SmallStatue>() );
  addCreator(B_STATUE2, OC3_STR_EXT(B_STATUE2), new BaseBuildingCreator<MediumStatue>() );
  addCreator(B_STATUE3, OC3_STR_EXT(B_STATUE3), new BaseBuildingCreator<BigStatue>() );
  addCreator(B_GARDEN, OC3_STR_EXT(B_GARDEN) , new BaseBuildingCreator<Garden>() );
  addCreator(B_PLAZA, OC3_STR_EXT(B_PLAZA)  , new BaseBuildingCreator<Plaza>() );
  // water
  addCreator(B_WELL, OC3_STR_EXT(B_WELL)     , new WorkingBuildingCreator<Well>() );
  addCreator(B_FOUNTAIN, OC3_STR_EXT(B_FOUNTAIN) , new WorkingBuildingCreator<Fountain>() );
  addCreator(B_AQUEDUCT, OC3_STR_EXT(B_AQUEDUCT), new BaseBuildingCreator<Aqueduct>() );
  addCreator(B_RESERVOIR, OC3_STR_EXT(B_RESERVOIR), new BaseBuildingCreator<Reservoir>() );
  // security
  addCreator(B_PREFECTURE, OC3_STR_EXT(B_PREFECTURE)  , new WorkingBuildingCreator<Prefecture>() );
  addCreator(B_FORT_LEGIONNAIRE, OC3_STR_EXT(B_FORT_LEGIONNAIRE), new BaseBuildingCreator<FortLegionnaire>() );
  addCreator(B_FORT_JAVELIN, OC3_STR_EXT(B_FORT_JAVELIN)   , new BaseBuildingCreator<FortJaveline>() );
  addCreator(B_FORT_MOUNTED, OC3_STR_EXT(B_FORT_MOUNTED)  , new BaseBuildingCreator<FortMounted>() );
  addCreator(B_MILITARY_ACADEMY, OC3_STR_EXT(B_MILITARY_ACADEMY), new WorkingBuildingCreator<MilitaryAcademy>() );
  addCreator(B_BARRACKS, OC3_STR_EXT(B_BARRACKS)        , new BaseBuildingCreator<Barracks>() );
  // commerce
  addCreator(B_MARKET, OC3_STR_EXT(B_MARKET)  , new WorkingBuildingCreator<Market>() );
  addCreator(B_WAREHOUSE, OC3_STR_EXT(B_WAREHOUSE), new WorkingBuildingCreator<Warehouse>() );
  addCreator(B_GRANARY, OC3_STR_EXT(B_GRANARY)  , new WorkingBuildingCreator<Granary>() );
  // farms
  addCreator(B_WHEAT_FARM, OC3_STR_EXT(B_WHEAT_FARM) , new FactoryCreator<FarmWheat>() );
  addCreator(B_OLIVE_FARM, OC3_STR_EXT(B_OLIVE_FARM) , new FactoryCreator<FarmOlive>() );
  addCreator(B_GRAPE_FARM, OC3_STR_EXT(B_GRAPE_FARM) , new FactoryCreator<FarmGrape>() );
  addCreator(B_PIG_FARM, OC3_STR_EXT(B_PIG_FARM)     , new FactoryCreator<FarmMeat>() );
  addCreator(B_FRUIT_FARM, OC3_STR_EXT(B_FRUIT_FARM)    , new FactoryCreator<FarmFruit>() );
  addCreator(B_VEGETABLE_FARM, OC3_STR_EXT(B_VEGETABLE_FARM), new FactoryCreator<FarmVegetable>() );
  // raw materials
  addCreator(B_IRON_MINE, OC3_STR_EXT(B_IRON_MINE)  , new FactoryCreator<IronMine>() );
  addCreator(B_TIMBER_YARD, OC3_STR_EXT(B_TIMBER_YARD), new FactoryCreator<TimberLogger>() );
  addCreator(B_CLAY_PIT, OC3_STR_EXT(B_CLAY_PIT)  , new FactoryCreator<ClayPit>() );
  addCreator(B_MARBLE_QUARRY, OC3_STR_EXT(B_MARBLE_QUARRY), new FactoryCreator<MarbleQuarry>() );
  // factories
  addCreator(B_WEAPONS_WORKSHOP, OC3_STR_EXT(B_WEAPONS_WORKSHOP)   , new FactoryCreator<WeaponsWorkshop>() );
  addCreator(B_FURNITURE, OC3_STR_EXT(B_FURNITURE), new FactoryCreator<WorkshopFurniture>() );
  addCreator(B_WINE_WORKSHOP, OC3_STR_EXT(B_WINE_WORKSHOP)     , new FactoryCreator<Winery>() );
  addCreator(B_OIL_WORKSHOP, OC3_STR_EXT(B_OIL_WORKSHOP)      , new FactoryCreator<Creamery>() );
  addCreator(B_POTTERY, OC3_STR_EXT(B_POTTERY)  , new FactoryCreator<Pottery>() );
  // utility
  addCreator(B_ENGINEER_POST, OC3_STR_EXT(B_ENGINEER_POST), new WorkingBuildingCreator<BuildingEngineer>() );
  addCreator(B_LOW_BRIDGE, OC3_STR_EXT(B_LOW_BRIDGE), new BaseBuildingCreator<LowBridge>() );
  addCreator(B_HIGH_BRIDGE, OC3_STR_EXT(B_HIGH_BRIDGE), new BaseBuildingCreator<HighBridge>() );
  addCreator(B_DOCK, OC3_STR_EXT(B_DOCK)    , new BaseBuildingCreator<Dock>() );
  addCreator(B_SHIPYARD, OC3_STR_EXT(B_SHIPYARD), new BaseBuildingCreator<Shipyard>() );
  addCreator(B_WHARF, OC3_STR_EXT(B_WHARF)   , new WorkingBuildingCreator<Wharf>() );
  addCreator(B_TRIUMPHAL_ARCH, OC3_STR_EXT(B_TRIUMPHAL_ARCH) , new BaseBuildingCreator<TriumphalArch>() );
  // religion
  addCreator(B_TEMPLE_CERES, OC3_STR_EXT(B_TEMPLE_CERES)  , new WorkingBuildingCreator<TempleCeres>() );
  addCreator(B_TEMPLE_NEPTUNE, OC3_STR_EXT(B_TEMPLE_NEPTUNE), new WorkingBuildingCreator<TempleNeptune>() );
  addCreator(B_TEMPLE_MARS, OC3_STR_EXT(B_TEMPLE_MARS)   , new WorkingBuildingCreator<TempleMars>() );
  addCreator(B_TEMPLE_VENUS, OC3_STR_EXT(B_TEMPLE_VENUS)  , new WorkingBuildingCreator<TempleVenus>() );
  addCreator(B_TEMPLE_MERCURE, OC3_STR_EXT(B_TEMPLE_MERCURE), new WorkingBuildingCreator<TempleMercure>() );
  addCreator(B_BIG_TEMPLE_CERES, OC3_STR_EXT(B_BIG_TEMPLE_CERES)  , new WorkingBuildingCreator<BigTempleCeres>() );
  addCreator(B_BIG_TEMPLE_NEPTUNE, OC3_STR_EXT(B_BIG_TEMPLE_NEPTUNE), new WorkingBuildingCreator<BigTempleNeptune>() );
  addCreator(B_BIG_TEMPLE_MARS, OC3_STR_EXT(B_BIG_TEMPLE_MARS)   , new WorkingBuildingCreator<BigTempleMars>() );
  addCreator(B_BIG_TEMPLE_VENUS, OC3_STR_EXT(B_BIG_TEMPLE_VENUS)  , new WorkingBuildingCreator<BigTempleVenus>() );
  addCreator(B_BIG_TEMPLE_MERCURE, OC3_STR_EXT(B_BIG_TEMPLE_MERCURE), new WorkingBuildingCreator<BigTempleMercure>() );
  addCreator(B_TEMPLE_ORACLE, OC3_STR_EXT(B_TEMPLE_ORACLE) , new WorkingBuildingCreator<TempleOracle>() );
  // health
  addCreator(B_BATHS, OC3_STR_EXT(B_BATHS)   , new WorkingBuildingCreator<Baths>() );
  addCreator(B_BARBER, OC3_STR_EXT(B_BARBER)  , new WorkingBuildingCreator<Barber>() );
  addCreator(B_DOCTOR, OC3_STR_EXT(B_DOCTOR)  , new WorkingBuildingCreator<Doctor>() );
  addCreator(B_HOSPITAL, OC3_STR_EXT(B_HOSPITAL), new WorkingBuildingCreator<Hospital>() );
  // education
  addCreator(B_SCHOOL, OC3_STR_EXT(B_SCHOOL) , new WorkingBuildingCreator<School>() );
  addCreator(B_LIBRARY, OC3_STR_EXT(B_LIBRARY), new WorkingBuildingCreator<Library>() );
  addCreator(B_COLLEGE, OC3_STR_EXT(B_COLLEGE), new WorkingBuildingCreator<College>() );
  addCreator(B_MISSION_POST, OC3_STR_EXT(B_MISSION_POST), new BaseBuildingCreator<MissionPost>() );
  // natives
  addCreator(B_NATIVE_HUT, OC3_STR_EXT(B_NATIVE_HUT)   , new BaseBuildingCreator<NativeHut>() );
  addCreator(B_NATIVE_CENTER, OC3_STR_EXT(B_NATIVE_CENTER), new BaseBuildingCreator<NativeCenter>() );
  addCreator(B_NATIVE_FIELD, OC3_STR_EXT(B_NATIVE_FIELD) , new BaseBuildingCreator<NativeField>() );

  //damages
  addCreator(B_BURNING_RUINS , OC3_STR_EXT(B_BURNING_RUINS), new BaseBuildingCreator<BurningRuins>() );
  addCreator(B_BURNED_RUINS , OC3_STR_EXT(B_BURNED_RUINS), new BaseBuildingCreator<BurnedRuins>() );
  addCreator(B_COLLAPSED_RUINS , OC3_STR_EXT(B_COLLAPSED_RUINS), new BaseBuildingCreator<CollapsedRuins>() );
  addCreator(B_PLAGUE_RUINS , OC3_STR_EXT(B_PLAGUE_RUINS), new BaseBuildingCreator<PlagueRuins>() );
}

void ConstructionManager::addCreator( const BuildingType type, const std::string& typeName, BuildingCreator* ctor )
{
  bool alreadyHaveConstructor = _d->name2typeMap.find( typeName ) != _d->name2typeMap.end();
  _OC3_DEBUG_BREAK_IF( alreadyHaveConstructor && "already have constructor for this type");

  if( !alreadyHaveConstructor )
  {
    _d->name2typeMap[ typeName ] = type;
    _d->constructors[ type ] = ctor;
  }
}

bool ConstructionManager::canCreate( const BuildingType type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
