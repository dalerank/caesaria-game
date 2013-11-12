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
#include "core/logger.hpp"
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
      anim.setDelay( anMap.get( "delay", 1 ).toInt() );

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
  addCreator(building::theater,      OC3_STR_EXT(Theater), new WorkingBuildingCreator<Theater>() );
  addCreator(building::amphitheater, OC3_STR_EXT(Amphitheater), new WorkingBuildingCreator<Amphitheater>() );
  addCreator(building::colloseum,    OC3_STR_EXT(Collosseum), new WorkingBuildingCreator<Collosseum>() );
  addCreator(building::actorColony,  OC3_STR_EXT(ActorColony), new WorkingBuildingCreator<ActorColony>() );
  addCreator(building::gladiatorSchool, OC3_STR_EXT(GladiatorSchool), new WorkingBuildingCreator<GladiatorSchool>() );
  addCreator(building::lionHouse,    OC3_STR_EXT(LionsNursery), new WorkingBuildingCreator<LionsNursery>() );
  addCreator(building::chariotSchool,OC3_STR_EXT(WorkshopChariot), new WorkingBuildingCreator<WorkshopChariot>() );
  addCreator(building::hippodrome,   OC3_STR_EXT(Hippodrome),new WorkingBuildingCreator<Hippodrome>() );
  // road&house
  addCreator(building::house,        OC3_STR_EXT(House), new ConstructionCreator<House>() );
  addCreator(construction::road,     OC3_STR_EXT(Road) , new ConstructionCreator<Road>() );
  // administration
  addCreator(building::forum,        OC3_STR_EXT(Forum) , new WorkingBuildingCreator<Forum>() );
  addCreator(building::senate,       OC3_STR_EXT(Senate), new WorkingBuildingCreator<Senate>() );
  addCreator(building::governorHouse,OC3_STR_EXT(GovernorsHouse) , new ConstructionCreator<GovernorsHouse>() );
  addCreator(building::governorVilla,OC3_STR_EXT(GovernorsVilla) , new ConstructionCreator<GovernorsVilla>() );
  addCreator(building::governorPalace, OC3_STR_EXT(GovernorsPalace), new ConstructionCreator<GovernorsPalace>() );
  addCreator(building::B_STATUE1,    OC3_STR_EXT(SmallStatue), new ConstructionCreator<SmallStatue>() );
  addCreator(building::B_STATUE2,    OC3_STR_EXT(MediumStatue), new ConstructionCreator<MediumStatue>() );
  addCreator(building::B_STATUE3,    OC3_STR_EXT(BigStatue), new ConstructionCreator<BigStatue>() );
  addCreator(construction::B_GARDEN, OC3_STR_EXT(Garden) , new ConstructionCreator<Garden>() );
  addCreator(construction::B_PLAZA,  OC3_STR_EXT(Plaza)  , new ConstructionCreator<Plaza>() );
  // water
  addCreator(building::well,       OC3_STR_EXT(Well)     , new WorkingBuildingCreator<Well>() );
  addCreator(building::fountain,   OC3_STR_EXT(Fountain) , new WorkingBuildingCreator<Fountain>() );
  addCreator(building::aqueduct,   OC3_STR_EXT(Aqueduct), new ConstructionCreator<Aqueduct>() );
  addCreator(building::reservoir,  OC3_STR_EXT(Reservoir), new ConstructionCreator<Reservoir>() );
  // security
  addCreator(building::prefecture,   OC3_STR_EXT(Prefecture)  , new WorkingBuildingCreator<Prefecture>() );
  addCreator(building::B_FORT_LEGIONNAIRE, OC3_STR_EXT(FortLegionnaire), new ConstructionCreator<FortLegionnaire>() );
  addCreator(building::B_FORT_JAVELIN, OC3_STR_EXT(FortJaveline)   , new ConstructionCreator<FortJaveline>() );
  addCreator(building::B_FORT_MOUNTED, OC3_STR_EXT(FortMounted)  , new ConstructionCreator<FortMounted>() );
  addCreator(building::militaryAcademy, OC3_STR_EXT(MilitaryAcademy), new WorkingBuildingCreator<MilitaryAcademy>() );
  addCreator(building::barracks,   OC3_STR_EXT(Barracks)        , new ConstructionCreator<Barracks>() );
  // commerce
  addCreator(building::market,     OC3_STR_EXT(Market)  , new WorkingBuildingCreator<Market>() );
  addCreator(building::warehouse,  OC3_STR_EXT(Warehouse), new WorkingBuildingCreator<Warehouse>() );
  addCreator(building::granary,      OC3_STR_EXT(Granary)  , new WorkingBuildingCreator<Granary>() );
  // farms
  addCreator(building::wheatFarm,    OC3_STR_EXT(FarmWheat) , new FactoryCreator<FarmWheat>() );
  addCreator(building::oliveFarm, OC3_STR_EXT(FarmOlive) , new FactoryCreator<FarmOlive>() );
  addCreator(building::grapeFarm,    OC3_STR_EXT(FarmGrape) , new FactoryCreator<FarmGrape>() );
  addCreator(building::pigFarm,   OC3_STR_EXT(FarmMeat)     , new FactoryCreator<FarmMeat>() );
  addCreator(building::fruitFarm, OC3_STR_EXT(FarmFruit)    , new FactoryCreator<FarmFruit>() );
  addCreator(building::vegetableFarm, OC3_STR_EXT(FarmVegetable), new FactoryCreator<FarmVegetable>() );
  // raw materials
  addCreator(building::ironMine,     OC3_STR_EXT(IronMine)  , new FactoryCreator<IronMine>() );
  addCreator(building::timberLogger, OC3_STR_EXT(TimberLogger), new FactoryCreator<TimberLogger>() );
  addCreator(building::clayPit,      OC3_STR_EXT(ClayPit)  , new FactoryCreator<ClayPit>() );
  addCreator(building::marbleQuarry, OC3_STR_EXT(MarbleQuarry), new FactoryCreator<MarbleQuarry>() );
  // factories
  addCreator(building::B_WEAPONS_WORKSHOP, OC3_STR_EXT(WeaponsWorkshop)   , new FactoryCreator<WeaponsWorkshop>() );
  addCreator(building::furniture,  OC3_STR_EXT(WorkshopFurniture), new FactoryCreator<WorkshopFurniture>() );
  addCreator(building::winery, OC3_STR_EXT(Winery)     , new FactoryCreator<Winery>() );
  addCreator(building::creamery, OC3_STR_EXT(Creamery)      , new FactoryCreator<Creamery>() );
  addCreator(building::pottery,      OC3_STR_EXT(Pottery)  , new FactoryCreator<Pottery>() );
  // utility
  addCreator(building::engineerPost, OC3_STR_EXT(EngineerPost), new WorkingBuildingCreator<EngineerPost>() );
  addCreator(building::lowBridge,    OC3_STR_EXT(LowBridge), new ConstructionCreator<LowBridge>() );
  addCreator(building::highBridge,   OC3_STR_EXT(HighBridge), new ConstructionCreator<HighBridge>() );
  addCreator(building::dock,       OC3_STR_EXT(Dock)    , new ConstructionCreator<Dock>() );
  addCreator(building::shipyard,   OC3_STR_EXT(Shipyard), new ConstructionCreator<Shipyard>() );
  addCreator(building::wharf,        OC3_STR_EXT(Wharf)   , new WorkingBuildingCreator<Wharf>() );
  addCreator(building::triumphalArch, OC3_STR_EXT(TriumphalArch) , new ConstructionCreator<TriumphalArch>() );
  // religion
  addCreator(building::templeCeres,  OC3_STR_EXT(TempleCeres)  , new WorkingBuildingCreator<TempleCeres>() );
  addCreator(building::templeNeptune, OC3_STR_EXT(TempleNeptune), new WorkingBuildingCreator<TempleNeptune>() );
  addCreator(building::templeMars,OC3_STR_EXT(TempleMars)   , new WorkingBuildingCreator<TempleMars>() );
  addCreator(building::templeVenus, OC3_STR_EXT(TempleVenus)  , new WorkingBuildingCreator<TempleVenus>() );
  addCreator(building::templeMercury, OC3_STR_EXT(TempleMercure), new WorkingBuildingCreator<TempleMercure>() );
  addCreator(building::B_BIG_TEMPLE_CERES, OC3_STR_EXT(BigTempleCeres)  , new WorkingBuildingCreator<BigTempleCeres>() );
  addCreator(building::B_BIG_TEMPLE_NEPTUNE, OC3_STR_EXT(BigTempleNeptune), new WorkingBuildingCreator<BigTempleNeptune>() );
  addCreator(building::B_BIG_TEMPLE_MARS, OC3_STR_EXT(BigTempleMars)   , new WorkingBuildingCreator<BigTempleMars>() );
  addCreator(building::B_BIG_TEMPLE_VENUS, OC3_STR_EXT(BigTempleVenus)  , new WorkingBuildingCreator<BigTempleVenus>() );
  addCreator(building::B_BIG_TEMPLE_MERCURE, OC3_STR_EXT(BigTempleMercure), new WorkingBuildingCreator<BigTempleMercure>() );
  addCreator(building::oracle, OC3_STR_EXT(TempleOracle) , new WorkingBuildingCreator<TempleOracle>() );
  // health
  addCreator(building::B_BATHS,      OC3_STR_EXT(Baths)   , new WorkingBuildingCreator<Baths>() );
  addCreator(building::B_BARBER,     OC3_STR_EXT(Barber)  , new WorkingBuildingCreator<Barber>() );
  addCreator(building::B_DOCTOR,     OC3_STR_EXT(Doctor)  , new WorkingBuildingCreator<Doctor>() );
  addCreator(building::B_HOSPITAL,   OC3_STR_EXT(Hospital), new WorkingBuildingCreator<Hospital>() );
  // education
  addCreator(building::B_SCHOOL,     OC3_STR_EXT(School) , new WorkingBuildingCreator<School>() );
  addCreator(building::B_LIBRARY,    OC3_STR_EXT(Library), new WorkingBuildingCreator<Library>() );
  addCreator(building::B_COLLEGE,    OC3_STR_EXT(College), new WorkingBuildingCreator<College>() );
  addCreator(building::B_MISSION_POST, OC3_STR_EXT(MissionPost), new ConstructionCreator<MissionPost>() );
  // natives
  addCreator(building::B_NATIVE_HUT, OC3_STR_EXT(NativeHut)   , new ConstructionCreator<NativeHut>() );
  addCreator(building::B_NATIVE_CENTER, OC3_STR_EXT(NativeCenter), new ConstructionCreator<NativeCenter>() );
  addCreator(building::B_NATIVE_FIELD, OC3_STR_EXT(NativeField) , new ConstructionCreator<NativeField>() );

  //damages
  addCreator(building::burningRuins , OC3_STR_EXT(BurningRuins), new ConstructionCreator<BurningRuins>() );
  addCreator(building::B_BURNED_RUINS , OC3_STR_EXT(BurnedRuins), new ConstructionCreator<BurnedRuins>() );
  addCreator(building::B_COLLAPSED_RUINS , OC3_STR_EXT(CollapsedRuins), new ConstructionCreator<CollapsedRuins>() );
  addCreator(building::B_PLAGUE_RUINS , OC3_STR_EXT(PlagueRuins), new ConstructionCreator<PlagueRuins>() );
  addCreator(place::fishPlace,      OC3_STR_EXT(FishPlace), new BaseCreator<FishPlace>() );
}

void TileOverlayFactory::addCreator( const TileOverlay::Type type, const std::string& typeName, TileOverlayConstructor* ctor )
{
  bool alreadyHaveConstructor = _d->name2typeMap.find( typeName ) != _d->name2typeMap.end();

  if( !alreadyHaveConstructor )
  {
    _d->name2typeMap[ typeName ] = type;
    _d->constructors[ type ] = ctor;
  }
  else
  {
    Logger::warning( "TileOverlayFactory already have constructor for %s", typeName.c_str() );
  }
}

bool TileOverlayFactory::canCreate( const TileOverlay::Type type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
