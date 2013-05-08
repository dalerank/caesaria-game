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
#include "oc3_buildingprefect.hpp"
#include "oc3_road.hpp"
#include "oc3_market.hpp"
#include "oc3_granary.hpp"
#include "oc3_nativebuildings.hpp"

#include "oc3_factoryclay.hpp"
#include "oc3_factorymarble.hpp"

#include <map>

typedef std::map< BuildingType, AbstractConstructor* > ConstructorsMap;

class ConstructionManager::Impl
{
public:
  std::map< std::string, BuildingType > name2typeMap;
  ConstructorsMap constructors;
};

ConstructionPtr ConstructionManager::create(const BuildingType buildingType) const
{
  ConstructorsMap::iterator findConstructor = _d->constructors.find( buildingType );

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
  addConstructor( B_THEATER, OC3_STR_EXT(B_THEATER), new BaseConstructor<BuildingTheater>() );
  addConstructor( B_AMPHITHEATER, OC3_STR_EXT(B_AMPHITHEATER), new BaseConstructor<BuildingAmphiTheater>() );
  addConstructor( B_COLLOSSEUM, OC3_STR_EXT(B_COLLOSSEUM), new BaseConstructor<BuildingCollosseum>() );
  addConstructor( B_ACTOR, OC3_STR_EXT(B_ACTOR), new BaseConstructor<BuildingActor>() );
  addConstructor( B_GLADIATOR, OC3_STR_EXT(B_GLADIATOR), new BaseConstructor<BuildingGladiator>() );
  addConstructor( B_LION, OC3_STR_EXT(B_LION), new BaseConstructor<BuildingLion>() );
  addConstructor( B_CHARIOT, OC3_STR_EXT(B_CHARIOT), new BaseConstructor<BuildingChariot>() );
  addConstructor( B_HIPPODROME, OC3_STR_EXT(B_HIPPODROME),new BaseConstructor<BuildingHippodrome>() );
  // road&house
  addConstructor(B_HOUSE, OC3_STR_EXT(B_HOUSE), new BaseConstructor<House>() );
  addConstructor(B_ROAD, OC3_STR_EXT(B_ROAD) , new BaseConstructor<Road>() );
  // administration
  addConstructor(B_FORUM, OC3_STR_EXT(B_FORUM) , new BaseConstructor<Forum>() );
  addConstructor(B_SENATE, OC3_STR_EXT(B_SENATE), new BaseConstructor<Senate>() );
  addConstructor(B_GOVERNOR_HOUSE, OC3_STR_EXT(B_GOVERNOR_HOUSE) , new BaseConstructor<GovernorsHouse>() );
  addConstructor(B_GOVERNOR_VILLA, OC3_STR_EXT(B_GOVERNOR_VILLA) , new BaseConstructor<GovernorsVilla>() );
  addConstructor(B_GOVERNOR_PALACE, OC3_STR_EXT(B_GOVERNOR_PALACE), new BaseConstructor<GovernorsPalace>() ); 
  addConstructor(B_STATUE1, OC3_STR_EXT(B_STATUE1), new BaseConstructor<SmallStatue>() ); 
  addConstructor(B_STATUE2, OC3_STR_EXT(B_STATUE2), new BaseConstructor<MediumStatue>() ); 
  addConstructor(B_STATUE3, OC3_STR_EXT(B_STATUE3), new BaseConstructor<BigStatue>() );
  addConstructor(B_GARDEN, OC3_STR_EXT(B_GARDEN) , new BaseConstructor<Garden>() );
  addConstructor(B_PLAZA, OC3_STR_EXT(B_PLAZA)  , new BaseConstructor<Plaza>() );
  // water
  addConstructor(B_WELL, OC3_STR_EXT(B_WELL)     , new BaseConstructor<BuildingWell>() );
  addConstructor(B_FOUNTAIN, OC3_STR_EXT(B_FOUNTAIN) , new BaseConstructor<BuildingFountain>() );
  addConstructor(B_AQUEDUCT, OC3_STR_EXT(B_AQUEDUCT), new BaseConstructor<Aqueduct>() );
  addConstructor(B_RESERVOIR, OC3_STR_EXT(B_RESERVOIR), new BaseConstructor<Reservoir>() );
  // security
  addConstructor(B_PREFECT, OC3_STR_EXT(B_PREFECT)  , new BaseConstructor<BuildingPrefect>() );
  addConstructor(B_FORT_LEGIONNAIRE, OC3_STR_EXT(B_FORT_LEGIONNAIRE), new BaseConstructor<FortLegionnaire>() );
  addConstructor(B_FORT_JAVELIN, OC3_STR_EXT(B_FORT_JAVELIN)   , new BaseConstructor<FortJaveline>() );
  addConstructor(B_FORT_MOUNTED, OC3_STR_EXT(B_FORT_MOUNTED)  , new BaseConstructor<FortMounted>() );
  addConstructor(B_MILITARY_ACADEMY, OC3_STR_EXT(B_MILITARY_ACADEMY), new BaseConstructor<Academy>() );
  addConstructor(B_BARRACKS, OC3_STR_EXT(B_BARRACKS)        , new BaseConstructor<Barracks>() );
  // commerce
  addConstructor(B_MARKET, OC3_STR_EXT(B_MARKET)  , new BaseConstructor<Market>() );
  addConstructor(B_WAREHOUSE, OC3_STR_EXT(B_WAREHOUSE), new BaseConstructor<Warehouse>() );
  addConstructor(B_GRANARY, OC3_STR_EXT(B_GRANARY)  , new BaseConstructor<Granary>() );
  // farms
  addConstructor(B_WHEAT, OC3_STR_EXT(B_WHEAT)    , new BaseConstructor<FarmWheat>() );
  addConstructor(B_OLIVE, OC3_STR_EXT(B_OLIVE)    , new BaseConstructor<FarmOlive>() );
  addConstructor(B_GRAPE, OC3_STR_EXT(B_GRAPE)    , new BaseConstructor<FarmGrape>() );
  addConstructor(B_MEAT, OC3_STR_EXT(B_MEAT)     , new BaseConstructor<FarmMeat>() );
  addConstructor(B_FRUIT, OC3_STR_EXT(B_FRUIT)    , new BaseConstructor<FarmFruit>() );
  addConstructor(B_VEGETABLE, OC3_STR_EXT(B_VEGETABLE), new BaseConstructor<FarmVegetable>() );
  // raw materials
  addConstructor(B_IRON, OC3_STR_EXT(B_IRON)  , new BaseConstructor<FactoryIron>() );
  addConstructor(B_TIMBER, OC3_STR_EXT(B_TIMBER), new BaseConstructor<FactoryTimber>() );
  addConstructor(B_CLAY_PIT, OC3_STR_EXT(B_CLAY_PIT)  , new BaseConstructor<FactoryClay>() );
  addConstructor(B_MARBLE, OC3_STR_EXT(B_MARBLE), new BaseConstructor<FactoryMarble>() );
  // factories
  addConstructor(B_WEAPON, OC3_STR_EXT(B_WEAPON)   , new BaseConstructor<FactoryWeapon>() );
  addConstructor(B_FURNITURE, OC3_STR_EXT(B_FURNITURE), new BaseConstructor<FactoryFurniture>() );
  addConstructor(B_WINE, OC3_STR_EXT(B_WINE)     , new BaseConstructor<FactoryWine>() );
  addConstructor(B_OIL, OC3_STR_EXT(B_OIL)      , new BaseConstructor<FactoryOil>() );
  addConstructor(B_POTTERY, OC3_STR_EXT(B_POTTERY)  , new BaseConstructor<FactoryPottery>() );
  // utility
  addConstructor(B_ENGINEER, OC3_STR_EXT(B_ENGINEER), new BaseConstructor<BuildingEngineer>() );
  addConstructor(B_DOCK, OC3_STR_EXT(B_DOCK)    , new BaseConstructor<Dock>() );
  addConstructor(B_SHIPYARD, OC3_STR_EXT(B_SHIPYARD), new BaseConstructor<Shipyard>() );
  addConstructor(B_WHARF, OC3_STR_EXT(B_WHARF)   , new BaseConstructor<Wharf>() );
  addConstructor(B_TRIUMPHAL_ARCH, OC3_STR_EXT(B_TRIUMPHAL_ARCH) , new BaseConstructor<TriumphalArch>() );
  // religion
  addConstructor(B_TEMPLE_CERES, OC3_STR_EXT(B_TEMPLE_CERES)  , new BaseConstructor<TempleCeres>() );
  addConstructor(B_TEMPLE_NEPTUNE, OC3_STR_EXT(B_TEMPLE_NEPTUNE), new BaseConstructor<TempleNeptune>() );
  addConstructor(B_TEMPLE_MARS, OC3_STR_EXT(B_TEMPLE_MARS)   , new BaseConstructor<TempleMars>() );
  addConstructor(B_TEMPLE_VENUS, OC3_STR_EXT(B_TEMPLE_VENUS)  , new BaseConstructor<TempleVenus>() );
  addConstructor(B_TEMPLE_MERCURE, OC3_STR_EXT(B_TEMPLE_MERCURE), new BaseConstructor<TempleMercure>() );
  addConstructor(B_BIG_TEMPLE_CERES, OC3_STR_EXT(B_BIG_TEMPLE_CERES)  , new BaseConstructor<BigTempleCeres>() );
  addConstructor(B_BIG_TEMPLE_NEPTUNE, OC3_STR_EXT(B_BIG_TEMPLE_NEPTUNE), new BaseConstructor<BigTempleNeptune>() );
  addConstructor(B_BIG_TEMPLE_MARS, OC3_STR_EXT(B_BIG_TEMPLE_MARS)   , new BaseConstructor<BigTempleMars>() );
  addConstructor(B_BIG_TEMPLE_VENUS, OC3_STR_EXT(B_BIG_TEMPLE_VENUS)  , new BaseConstructor<BigTempleVenus>() );
  addConstructor(B_BIG_TEMPLE_MERCURE, OC3_STR_EXT(B_BIG_TEMPLE_MERCURE), new BaseConstructor<BigTempleMercure>() );
  addConstructor(B_TEMPLE_ORACLE, OC3_STR_EXT(B_TEMPLE_ORACLE) , new BaseConstructor<TempleOracle>() );
  // health
  addConstructor(B_BATHS, OC3_STR_EXT(B_BATHS)   , new BaseConstructor<Baths>() );
  addConstructor(B_BARBER, OC3_STR_EXT(B_BARBER)  , new BaseConstructor<Barber>() );
  addConstructor(B_DOCTOR, OC3_STR_EXT(B_DOCTOR)  , new BaseConstructor<Doctor>() );
  addConstructor(B_HOSPITAL, OC3_STR_EXT(B_HOSPITAL), new BaseConstructor<Hospital>() );
  // education
  addConstructor(B_SCHOOL, OC3_STR_EXT(B_SCHOOL) , new BaseConstructor<School>() );
  addConstructor(B_LIBRARY, OC3_STR_EXT(B_LIBRARY), new BaseConstructor<Library>() );
  addConstructor(B_COLLEGE, OC3_STR_EXT(B_COLLEGE), new BaseConstructor<College>() );
  addConstructor(B_MISSION_POST, OC3_STR_EXT(B_MISSION_POST), new BaseConstructor<MissionPost>() );
  // natives
  addConstructor(B_NATIVE_HUT, OC3_STR_EXT(B_NATIVE_HUT)   , new BaseConstructor<NativeHut>() );
  addConstructor(B_NATIVE_CENTER, OC3_STR_EXT(B_NATIVE_CENTER), new BaseConstructor<NativeCenter>() );
  addConstructor(B_NATIVE_FIELD, OC3_STR_EXT(B_NATIVE_FIELD) , new BaseConstructor<NativeField>() );

  //damages
  addConstructor(B_BURNING_RUINS , OC3_STR_EXT(B_BURNING_RUINS), new BaseConstructor<BurningRuins>() );
  addConstructor(B_COLLAPSED_RUINS , OC3_STR_EXT(B_COLLAPSED_RUINS), new BaseConstructor<CollapsedRuins>() );
}

void ConstructionManager::addConstructor( const BuildingType type, const std::string& typeName, AbstractConstructor* ctor )
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