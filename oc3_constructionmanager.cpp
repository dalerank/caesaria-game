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
#include "oc3_service_building.hpp"
#include "oc3_training_building.hpp"
#include "oc3_water_buildings.hpp"
#include "oc3_warehouse.hpp"
#include "oc3_collapsedruins.hpp"
#include "oc3_buildingengineer.hpp"
#include "oc3_factory_building.hpp"
#include "oc3_burningruins.hpp"
#include "oc3_house.hpp"
#include "oc3_senate.hpp"
#include "oc3_prefecture.hpp"
#include "oc3_road.hpp"
#include "oc3_market.hpp"
#include "oc3_granary.hpp"
#include "oc3_well.hpp"
#include "oc3_nativebuildings.hpp"
#include "oc3_farm.hpp"
#include "oc3_factory_pottery.hpp"
#include "oc3_religion_building.hpp"
#include "oc3_low_bridge.hpp"
#include "oc3_high_bridge.hpp"

#include "oc3_clay_pit.hpp"
#include "oc3_factorymarble.hpp"
#include "oc3_goverment_building.hpp"
#include "oc3_military_building.hpp"
#include "oc3_academy.hpp"
#include "oc3_forum.hpp"
#include "oc3_garden.hpp"
#include "oc3_health_buildings.hpp"

#include <map>

class ConstructionManager::Impl
{
public:
  typedef std::map< BuildingType, AbstractBuildingCreator* > BuildingCreators;
  std::map< std::string, BuildingType > name2typeMap;
  BuildingCreators constructors;
};

ConstructionPtr ConstructionManager::create(const BuildingType buildingType) const
{
  Impl::BuildingCreators::iterator findConstructor = _d->constructors.find( buildingType );

  if( findConstructor != _d->constructors.end() )
  {
    ConstructionPtr ret( findConstructor->second->create() );
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
  addCreator( B_THEATER, OC3_STR_EXT(B_THEATER), new BuildingCreator<Theater>() );
  addCreator( B_AMPHITHEATER, OC3_STR_EXT(B_AMPHITHEATER), new BuildingCreator<BuildingAmphiTheater>() );
  addCreator( B_COLLOSSEUM, OC3_STR_EXT(B_COLLOSSEUM), new BuildingCreator<BuildingCollosseum>() );
  addCreator( B_ACTOR_COLONY, OC3_STR_EXT(B_ACTOR_COLONY), new BuildingCreator<BuildingActor>() );
  addCreator( B_GLADIATOR_SCHOOL, OC3_STR_EXT(B_GLADIATOR_SCHOOL), new BuildingCreator<BuildingGladiator>() );
  addCreator( B_LION_HOUSE, OC3_STR_EXT(B_LION_HOUSE), new BuildingCreator<BuildingLion>() );
  addCreator( B_CHARIOT_MAKER, OC3_STR_EXT(B_CHARIOT_MAKER), new BuildingCreator<BuildingChariot>() );
  addCreator( B_HIPPODROME, OC3_STR_EXT(B_HIPPODROME),new BuildingCreator<BuildingHippodrome>() );
  // road&house
  addCreator(B_HOUSE, OC3_STR_EXT(B_HOUSE), new BuildingCreator<House>() );
  addCreator(B_ROAD, OC3_STR_EXT(B_ROAD) , new BuildingCreator<Road>() );
  // administration
  addCreator(B_FORUM, OC3_STR_EXT(B_FORUM) , new BuildingCreator<Forum>() );
  addCreator(B_SENATE, OC3_STR_EXT(B_SENATE), new BuildingCreator<Senate>() );
  addCreator(B_GOVERNOR_HOUSE, OC3_STR_EXT(B_GOVERNOR_HOUSE) , new BuildingCreator<GovernorsHouse>() );
  addCreator(B_GOVERNOR_VILLA, OC3_STR_EXT(B_GOVERNOR_VILLA) , new BuildingCreator<GovernorsVilla>() );
  addCreator(B_GOVERNOR_PALACE, OC3_STR_EXT(B_GOVERNOR_PALACE), new BuildingCreator<GovernorsPalace>() ); 
  addCreator(B_STATUE1, OC3_STR_EXT(B_STATUE1), new BuildingCreator<SmallStatue>() ); 
  addCreator(B_STATUE2, OC3_STR_EXT(B_STATUE2), new BuildingCreator<MediumStatue>() ); 
  addCreator(B_STATUE3, OC3_STR_EXT(B_STATUE3), new BuildingCreator<BigStatue>() );
  addCreator(B_GARDEN, OC3_STR_EXT(B_GARDEN) , new BuildingCreator<Garden>() );
  addCreator(B_PLAZA, OC3_STR_EXT(B_PLAZA)  , new BuildingCreator<Plaza>() );
  // water
  addCreator(B_WELL, OC3_STR_EXT(B_WELL)     , new BuildingCreator<BuildingWell>() );
  addCreator(B_FOUNTAIN, OC3_STR_EXT(B_FOUNTAIN) , new BuildingCreator<BuildingFountain>() );
  addCreator(B_AQUEDUCT, OC3_STR_EXT(B_AQUEDUCT), new BuildingCreator<Aqueduct>() );
  addCreator(B_RESERVOIR, OC3_STR_EXT(B_RESERVOIR), new BuildingCreator<Reservoir>() );
  // security
  addCreator(B_PREFECTURE, OC3_STR_EXT(B_PREFECTURE)  , new BuildingCreator<BuildingPrefecture>() );
  addCreator(B_FORT_LEGIONNAIRE, OC3_STR_EXT(B_FORT_LEGIONNAIRE), new BuildingCreator<FortLegionnaire>() );
  addCreator(B_FORT_JAVELIN, OC3_STR_EXT(B_FORT_JAVELIN)   , new BuildingCreator<FortJaveline>() );
  addCreator(B_FORT_MOUNTED, OC3_STR_EXT(B_FORT_MOUNTED)  , new BuildingCreator<FortMounted>() );
  addCreator(B_MILITARY_ACADEMY, OC3_STR_EXT(B_MILITARY_ACADEMY), new BuildingCreator<Academy>() );
  addCreator(B_BARRACKS, OC3_STR_EXT(B_BARRACKS)        , new BuildingCreator<Barracks>() );
  // commerce
  addCreator(B_MARKET, OC3_STR_EXT(B_MARKET)  , new BuildingCreator<Market>() );
  addCreator(B_WAREHOUSE, OC3_STR_EXT(B_WAREHOUSE), new BuildingCreator<Warehouse>() );
  addCreator(B_GRANARY, OC3_STR_EXT(B_GRANARY)  , new BuildingCreator<Granary>() );
  // farms
  addCreator(B_WHEAT_FARM, OC3_STR_EXT(B_WHEAT_FARM)    , new BuildingCreator<FarmWheat>() );
  addCreator(B_OLIVE, OC3_STR_EXT(B_OLIVE)    , new BuildingCreator<FarmOlive>() );
  addCreator(B_GRAPE_FARM, OC3_STR_EXT(B_GRAPE_FARM)    , new BuildingCreator<FarmGrape>() );
  addCreator(B_MEAT, OC3_STR_EXT(B_MEAT)     , new BuildingCreator<FarmMeat>() );
  addCreator(B_FRUIT, OC3_STR_EXT(B_FRUIT)    , new BuildingCreator<FarmFruit>() );
  addCreator(B_VEGETABLE, OC3_STR_EXT(B_VEGETABLE), new BuildingCreator<FarmVegetable>() );
  // raw materials
  addCreator(B_IRON_MINE, OC3_STR_EXT(B_IRON_MINE)  , new BuildingCreator<IronMine>() );
  addCreator(B_TIMBER, OC3_STR_EXT(B_TIMBER), new BuildingCreator<TimberLogger>() );
  addCreator(B_CLAY_PIT, OC3_STR_EXT(B_CLAY_PIT)  , new BuildingCreator<ClayPit>() );
  addCreator(B_MARBLE, OC3_STR_EXT(B_MARBLE), new BuildingCreator<FactoryMarble>() );
  // factories
  addCreator(B_WEAPON, OC3_STR_EXT(B_WEAPON)   , new BuildingCreator<WeaponsWorkshop>() );
  addCreator(B_FURNITURE, OC3_STR_EXT(B_FURNITURE), new BuildingCreator<FactoryFurniture>() );
  addCreator(B_WINE, OC3_STR_EXT(B_WINE)     , new BuildingCreator<Winery>() );
  addCreator(B_OIL, OC3_STR_EXT(B_OIL)      , new BuildingCreator<FactoryOil>() );
  addCreator(B_POTTERY, OC3_STR_EXT(B_POTTERY)  , new BuildingCreator<FactoryPottery>() );
  // utility
  addCreator(B_ENGINEER_POST, OC3_STR_EXT(B_ENGINEER_POST), new BuildingCreator<BuildingEngineer>() );
  addCreator(B_LOW_BRIDGE, OC3_STR_EXT(B_LOW_BRIDGE), new BuildingCreator<LowBridge>() );
  addCreator(B_HIGH_BRIDGE, OC3_STR_EXT(B_HIGH_BRIDGE), new BuildingCreator<HighBridge>() );
  addCreator(B_DOCK, OC3_STR_EXT(B_DOCK)    , new BuildingCreator<Dock>() );
  addCreator(B_SHIPYARD, OC3_STR_EXT(B_SHIPYARD), new BuildingCreator<Shipyard>() );
  addCreator(B_WHARF, OC3_STR_EXT(B_WHARF)   , new BuildingCreator<Wharf>() );
  addCreator(B_TRIUMPHAL_ARCH, OC3_STR_EXT(B_TRIUMPHAL_ARCH) , new BuildingCreator<TriumphalArch>() );
  // religion
  addCreator(B_TEMPLE_CERES, OC3_STR_EXT(B_TEMPLE_CERES)  , new BuildingCreator<TempleCeres>() );
  addCreator(B_TEMPLE_NEPTUNE, OC3_STR_EXT(B_TEMPLE_NEPTUNE), new BuildingCreator<TempleNeptune>() );
  addCreator(B_TEMPLE_MARS, OC3_STR_EXT(B_TEMPLE_MARS)   , new BuildingCreator<TempleMars>() );
  addCreator(B_TEMPLE_VENUS, OC3_STR_EXT(B_TEMPLE_VENUS)  , new BuildingCreator<TempleVenus>() );
  addCreator(B_TEMPLE_MERCURE, OC3_STR_EXT(B_TEMPLE_MERCURE), new BuildingCreator<TempleMercure>() );
  addCreator(B_BIG_TEMPLE_CERES, OC3_STR_EXT(B_BIG_TEMPLE_CERES)  , new BuildingCreator<BigTempleCeres>() );
  addCreator(B_BIG_TEMPLE_NEPTUNE, OC3_STR_EXT(B_BIG_TEMPLE_NEPTUNE), new BuildingCreator<BigTempleNeptune>() );
  addCreator(B_BIG_TEMPLE_MARS, OC3_STR_EXT(B_BIG_TEMPLE_MARS)   , new BuildingCreator<BigTempleMars>() );
  addCreator(B_BIG_TEMPLE_VENUS, OC3_STR_EXT(B_BIG_TEMPLE_VENUS)  , new BuildingCreator<BigTempleVenus>() );
  addCreator(B_BIG_TEMPLE_MERCURE, OC3_STR_EXT(B_BIG_TEMPLE_MERCURE), new BuildingCreator<BigTempleMercure>() );
  addCreator(B_TEMPLE_ORACLE, OC3_STR_EXT(B_TEMPLE_ORACLE) , new BuildingCreator<TempleOracle>() );
  // health
  addCreator(B_BATHS, OC3_STR_EXT(B_BATHS)   , new BuildingCreator<Baths>() );
  addCreator(B_BARBER, OC3_STR_EXT(B_BARBER)  , new BuildingCreator<Barber>() );
  addCreator(B_DOCTOR, OC3_STR_EXT(B_DOCTOR)  , new BuildingCreator<Doctor>() );
  addCreator(B_HOSPITAL, OC3_STR_EXT(B_HOSPITAL), new BuildingCreator<Hospital>() );
  // education
  addCreator(B_SCHOOL, OC3_STR_EXT(B_SCHOOL) , new BuildingCreator<School>() );
  addCreator(B_LIBRARY, OC3_STR_EXT(B_LIBRARY), new BuildingCreator<Library>() );
  addCreator(B_COLLEGE, OC3_STR_EXT(B_COLLEGE), new BuildingCreator<College>() );
  addCreator(B_MISSION_POST, OC3_STR_EXT(B_MISSION_POST), new BuildingCreator<MissionPost>() );
  // natives
  addCreator(B_NATIVE_HUT, OC3_STR_EXT(B_NATIVE_HUT)   , new BuildingCreator<NativeHut>() );
  addCreator(B_NATIVE_CENTER, OC3_STR_EXT(B_NATIVE_CENTER), new BuildingCreator<NativeCenter>() );
  addCreator(B_NATIVE_FIELD, OC3_STR_EXT(B_NATIVE_FIELD) , new BuildingCreator<NativeField>() );

  //damages
  addCreator(B_BURNING_RUINS , OC3_STR_EXT(B_BURNING_RUINS), new BuildingCreator<BurningRuins>() );
  addCreator(B_BURNED_RUINS , OC3_STR_EXT(B_BURNED_RUINS), new BuildingCreator<BurnedRuins>() );
  addCreator(B_COLLAPSED_RUINS , OC3_STR_EXT(B_COLLAPSED_RUINS), new BuildingCreator<CollapsedRuins>() );
}

void ConstructionManager::addCreator( const BuildingType type, const std::string& typeName, AbstractBuildingCreator* ctor )
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
