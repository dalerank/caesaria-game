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
#include "oc3_training_building.hpp"
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

#include <map>

template< class T > class BaseBuildingCreator : public BuildingCreator
{
public:
  Construction* create()
  {
    return new T();
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
    ConstructionPtr ret( findConstructor->second->create() );
    const BuildingData& info = BuildingDataHolder::instance().getData( buildingType );

    if( info.getBasePicture().isValid() )
    {
      ret->setPicture( info.getBasePicture() );  // default picture for build tool
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
  addCreator( B_THEATER, OC3_STR_EXT(B_THEATER), new BaseBuildingCreator<Theater>() );
  addCreator( B_AMPHITHEATER, OC3_STR_EXT(B_AMPHITHEATER), new BaseBuildingCreator<Amphitheater>() );
  addCreator( B_COLLOSSEUM, OC3_STR_EXT(B_COLLOSSEUM), new BaseBuildingCreator<Collosseum>() );
  addCreator( B_ACTOR_COLONY, OC3_STR_EXT(B_ACTOR_COLONY), new BaseBuildingCreator<ActorColony>() );
  addCreator( B_GLADIATOR_SCHOOL, OC3_STR_EXT(B_GLADIATOR_SCHOOL), new BaseBuildingCreator<GladiatorSchool>() );
  addCreator( B_LION_HOUSE, OC3_STR_EXT(B_LION_HOUSE), new BaseBuildingCreator<LionsNursery>() );
  addCreator( B_CHARIOT_MAKER, OC3_STR_EXT(B_CHARIOT_MAKER), new BaseBuildingCreator<WorkshopChariot>() );
  addCreator( B_HIPPODROME, OC3_STR_EXT(B_HIPPODROME),new BaseBuildingCreator<Hippodrome>() );
  // road&house
  addCreator(B_HOUSE, OC3_STR_EXT(B_HOUSE), new BaseBuildingCreator<House>() );
  addCreator(B_ROAD, OC3_STR_EXT(B_ROAD) , new BaseBuildingCreator<Road>() );
  // administration
  addCreator(B_FORUM, OC3_STR_EXT(B_FORUM) , new BaseBuildingCreator<Forum>() );
  addCreator(B_SENATE, OC3_STR_EXT(B_SENATE), new BaseBuildingCreator<Senate>() );
  addCreator(B_GOVERNOR_HOUSE, OC3_STR_EXT(B_GOVERNOR_HOUSE) , new BaseBuildingCreator<GovernorsHouse>() );
  addCreator(B_GOVERNOR_VILLA, OC3_STR_EXT(B_GOVERNOR_VILLA) , new BaseBuildingCreator<GovernorsVilla>() );
  addCreator(B_GOVERNOR_PALACE, OC3_STR_EXT(B_GOVERNOR_PALACE), new BaseBuildingCreator<GovernorsPalace>() );
  addCreator(B_STATUE1, OC3_STR_EXT(B_STATUE1), new BaseBuildingCreator<SmallStatue>() );
  addCreator(B_STATUE2, OC3_STR_EXT(B_STATUE2), new BaseBuildingCreator<MediumStatue>() );
  addCreator(B_STATUE3, OC3_STR_EXT(B_STATUE3), new BaseBuildingCreator<BigStatue>() );
  addCreator(B_GARDEN, OC3_STR_EXT(B_GARDEN) , new BaseBuildingCreator<Garden>() );
  addCreator(B_PLAZA, OC3_STR_EXT(B_PLAZA)  , new BaseBuildingCreator<Plaza>() );
  // water
  addCreator(B_WELL, OC3_STR_EXT(B_WELL)     , new BaseBuildingCreator<Well>() );
  addCreator(B_FOUNTAIN, OC3_STR_EXT(B_FOUNTAIN) , new BaseBuildingCreator<Fountain>() );
  addCreator(B_AQUEDUCT, OC3_STR_EXT(B_AQUEDUCT), new BaseBuildingCreator<Aqueduct>() );
  addCreator(B_RESERVOIR, OC3_STR_EXT(B_RESERVOIR), new BaseBuildingCreator<Reservoir>() );
  // security
  addCreator(B_PREFECTURE, OC3_STR_EXT(B_PREFECTURE)  , new BaseBuildingCreator<Prefecture>() );
  addCreator(B_FORT_LEGIONNAIRE, OC3_STR_EXT(B_FORT_LEGIONNAIRE), new BaseBuildingCreator<FortLegionnaire>() );
  addCreator(B_FORT_JAVELIN, OC3_STR_EXT(B_FORT_JAVELIN)   , new BaseBuildingCreator<FortJaveline>() );
  addCreator(B_FORT_MOUNTED, OC3_STR_EXT(B_FORT_MOUNTED)  , new BaseBuildingCreator<FortMounted>() );
  addCreator(B_MILITARY_ACADEMY, OC3_STR_EXT(B_MILITARY_ACADEMY), new BaseBuildingCreator<MilitaryAcademy>() );
  addCreator(B_BARRACKS, OC3_STR_EXT(B_BARRACKS)        , new BaseBuildingCreator<Barracks>() );
  // commerce
  addCreator(B_MARKET, OC3_STR_EXT(B_MARKET)  , new BaseBuildingCreator<Market>() );
  addCreator(B_WAREHOUSE, OC3_STR_EXT(B_WAREHOUSE), new BaseBuildingCreator<Warehouse>() );
  addCreator(B_GRANARY, OC3_STR_EXT(B_GRANARY)  , new BaseBuildingCreator<Granary>() );
  // farms
  addCreator(B_WHEAT_FARM, OC3_STR_EXT(B_WHEAT_FARM)    , new BaseBuildingCreator<FarmWheat>() );
  addCreator(B_OLIVE_FARM, OC3_STR_EXT(B_OLIVE_FARM)    , new BaseBuildingCreator<FarmOlive>() );
  addCreator(B_GRAPE_FARM, OC3_STR_EXT(B_GRAPE_FARM)    , new BaseBuildingCreator<FarmGrape>() );
  addCreator(B_PIG_FARM, OC3_STR_EXT(B_PIG_FARM)     , new BaseBuildingCreator<FarmMeat>() );
  addCreator(B_FRUIT_FARM, OC3_STR_EXT(B_FRUIT_FARM)    , new BaseBuildingCreator<FarmFruit>() );
  addCreator(B_VEGETABLE_FARM, OC3_STR_EXT(B_VEGETABLE_FARM), new BaseBuildingCreator<FarmVegetable>() );
  // raw materials
  addCreator(B_IRON_MINE, OC3_STR_EXT(B_IRON_MINE)  , new BaseBuildingCreator<IronMine>() );
  addCreator(B_TIMBER_YARD, OC3_STR_EXT(B_TIMBER_YARD), new BaseBuildingCreator<TimberLogger>() );
  addCreator(B_CLAY_PIT, OC3_STR_EXT(B_CLAY_PIT)  , new BaseBuildingCreator<ClayPit>() );
  addCreator(B_MARBLE_QUARRY, OC3_STR_EXT(B_MARBLE_QUARRY), new BaseBuildingCreator<MarbleQuarry>() );
  // factories
  addCreator(B_WEAPONS_WORKSHOP, OC3_STR_EXT(B_WEAPONS_WORKSHOP)   , new BaseBuildingCreator<WeaponsWorkshop>() );
  addCreator(B_FURNITURE, OC3_STR_EXT(B_FURNITURE), new BaseBuildingCreator<WorkshopFurniture>() );
  addCreator(B_WINE_WORKSHOP, OC3_STR_EXT(B_WINE_WORKSHOP)     , new BaseBuildingCreator<Winery>() );
  addCreator(B_OIL_WORKSHOP, OC3_STR_EXT(B_OIL_WORKSHOP)      , new BaseBuildingCreator<Creamery>() );
  addCreator(B_POTTERY, OC3_STR_EXT(B_POTTERY)  , new BaseBuildingCreator<Pottery>() );
  // utility
  addCreator(B_ENGINEER_POST, OC3_STR_EXT(B_ENGINEER_POST), new BaseBuildingCreator<BuildingEngineer>() );
  addCreator(B_LOW_BRIDGE, OC3_STR_EXT(B_LOW_BRIDGE), new BaseBuildingCreator<LowBridge>() );
  addCreator(B_HIGH_BRIDGE, OC3_STR_EXT(B_HIGH_BRIDGE), new BaseBuildingCreator<HighBridge>() );
  addCreator(B_DOCK, OC3_STR_EXT(B_DOCK)    , new BaseBuildingCreator<Dock>() );
  addCreator(B_SHIPYARD, OC3_STR_EXT(B_SHIPYARD), new BaseBuildingCreator<Shipyard>() );
  addCreator(B_WHARF, OC3_STR_EXT(B_WHARF)   , new BaseBuildingCreator<Wharf>() );
  addCreator(B_TRIUMPHAL_ARCH, OC3_STR_EXT(B_TRIUMPHAL_ARCH) , new BaseBuildingCreator<TriumphalArch>() );
  // religion
  addCreator(B_TEMPLE_CERES, OC3_STR_EXT(B_TEMPLE_CERES)  , new BaseBuildingCreator<TempleCeres>() );
  addCreator(B_TEMPLE_NEPTUNE, OC3_STR_EXT(B_TEMPLE_NEPTUNE), new BaseBuildingCreator<TempleNeptune>() );
  addCreator(B_TEMPLE_MARS, OC3_STR_EXT(B_TEMPLE_MARS)   , new BaseBuildingCreator<TempleMars>() );
  addCreator(B_TEMPLE_VENUS, OC3_STR_EXT(B_TEMPLE_VENUS)  , new BaseBuildingCreator<TempleVenus>() );
  addCreator(B_TEMPLE_MERCURE, OC3_STR_EXT(B_TEMPLE_MERCURE), new BaseBuildingCreator<TempleMercure>() );
  addCreator(B_BIG_TEMPLE_CERES, OC3_STR_EXT(B_BIG_TEMPLE_CERES)  , new BaseBuildingCreator<BigTempleCeres>() );
  addCreator(B_BIG_TEMPLE_NEPTUNE, OC3_STR_EXT(B_BIG_TEMPLE_NEPTUNE), new BaseBuildingCreator<BigTempleNeptune>() );
  addCreator(B_BIG_TEMPLE_MARS, OC3_STR_EXT(B_BIG_TEMPLE_MARS)   , new BaseBuildingCreator<BigTempleMars>() );
  addCreator(B_BIG_TEMPLE_VENUS, OC3_STR_EXT(B_BIG_TEMPLE_VENUS)  , new BaseBuildingCreator<BigTempleVenus>() );
  addCreator(B_BIG_TEMPLE_MERCURE, OC3_STR_EXT(B_BIG_TEMPLE_MERCURE), new BaseBuildingCreator<BigTempleMercure>() );
  addCreator(B_TEMPLE_ORACLE, OC3_STR_EXT(B_TEMPLE_ORACLE) , new BaseBuildingCreator<TempleOracle>() );
  // health
  addCreator(B_BATHS, OC3_STR_EXT(B_BATHS)   , new BaseBuildingCreator<Baths>() );
  addCreator(B_BARBER, OC3_STR_EXT(B_BARBER)  , new BaseBuildingCreator<Barber>() );
  addCreator(B_DOCTOR, OC3_STR_EXT(B_DOCTOR)  , new BaseBuildingCreator<Doctor>() );
  addCreator(B_HOSPITAL, OC3_STR_EXT(B_HOSPITAL), new BaseBuildingCreator<Hospital>() );
  // education
  addCreator(B_SCHOOL, OC3_STR_EXT(B_SCHOOL) , new BaseBuildingCreator<School>() );
  addCreator(B_LIBRARY, OC3_STR_EXT(B_LIBRARY), new BaseBuildingCreator<Library>() );
  addCreator(B_COLLEGE, OC3_STR_EXT(B_COLLEGE), new BaseBuildingCreator<College>() );
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
