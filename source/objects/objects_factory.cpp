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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "objects_factory.hpp"
#include "service.hpp"
#include "training.hpp"
#include "core/variant_map.hpp"
#include "watersupply.hpp"
#include "warehouse.hpp"
#include "ruins.hpp"
#include "engineer_post.hpp"
#include "factory.hpp"
#include "house.hpp"
#include "senate.hpp"
#include "prefecture.hpp"
#include "entertainment.hpp"
#include "religion.hpp"
#include "aqueduct.hpp"
#include "barracks.hpp"
#include "road.hpp"
#include "market.hpp"
#include "granary.hpp"
#include "well.hpp"
#include "native.hpp"
#include "farm.hpp"
#include "pottery.hpp"
#include "low_bridge.hpp"
#include "high_bridge.hpp"
#include "clay_pit.hpp"
#include "marble_quarry.hpp"
#include "goverment.hpp"
#include "military.hpp"
#include "military_academy.hpp"
#include "forum.hpp"
#include "garden.hpp"
#include "health.hpp"
#include "tree.hpp"
#include "metadata.hpp"
#include "education.hpp"
#include "amphitheater.hpp"
#include "walker/fish_place.hpp"
#include "wharf.hpp"
#include "constants.hpp"
#include "constants.hpp"
#include "core/logger.hpp"
#include "wall.hpp"
#include "fortification.hpp"
#include "gatehouse.hpp"
#include "shipyard.hpp"
#include "tower.hpp"
#include "elevation.hpp"
#include "dock.hpp"
#include "fountain.hpp"
#include "colosseum.hpp"
#include "hippodrome.hpp"
#include "theater.hpp"
#include "workshop_chariot.hpp"
#include "furniture_workshop.hpp"
#include "actor_colony.hpp"
#include "sight.hpp"
#include "rift.hpp"
#include "weaponsworkshop.hpp"
#include "missionarypost.hpp"
#include "ironmine.hpp"
#include "river.hpp"
#include "timber_logger.hpp"
#include "waymark.hpp"
#include <map>

using namespace constants;
using namespace gfx;

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

    const MetaData& info = MetaDataHolder::instance().getData( ret->type() );
    init( ret, info );

    return ret;
  }

  virtual void init( TileOverlayPtr a, const MetaData& info )
  {
    ConstructionPtr construction = ptr_cast<Construction>( a );
    if( construction == 0 )
      return;

    if( info.picture().isValid() )
    {
      construction->setPicture( info.picture() );  // default picture for build tool
    }

    VariantMap anMap = info.getOption( "animation" ).toMap();
    if( !anMap.empty() )
    {
      Animation anim;

      anim.load( anMap.get( "rc" ).toString(), anMap.get( "start" ).toInt(),
                 anMap.get( "count" ).toInt(), anMap.get( "reverse", false ).toBool(),
                 anMap.get( "step", 1 ).toInt() );

      Variant v_offset = anMap.get( "offset" );
      if( v_offset.isValid() )
      {
        anim.setOffset( v_offset.toPoint() );
      }

      anim.setDelay( (unsigned int)anMap.get( "delay", 1u ) );

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

    WorkingBuildingPtr wb = ptr_cast<WorkingBuilding>( a );
    if( wb != 0 )
    {
      wb->setMaximumWorkers( (unsigned int)info.getOption( "employers" ) );
    }
  }
};

template< class T > class FactoryCreator : public WorkingBuildingCreator<T>
{
public:
  void init( TileOverlayPtr a, const MetaData& info )
  {
    WorkingBuildingCreator<T>::init( a, info );

    FactoryPtr f = ptr_cast<Factory>( a );
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

TileOverlayFactory& TileOverlayFactory::instance()
{
  static TileOverlayFactory inst;
  return inst;
}

TileOverlayFactory::TileOverlayFactory() : _d( new Impl )
{
#define ADD_CREATOR(type,classObject,creator) addCreator(type,CAESARIA_STR_EXT(classObject), new creator<classObject>() )
  // entertainment
  ADD_CREATOR(objects::theater,      Theater,      WorkingBuildingCreator );
  ADD_CREATOR(objects::amphitheater, Amphitheater, WorkingBuildingCreator );
  ADD_CREATOR(objects::colloseum,    Colosseum,   WorkingBuildingCreator );
  ADD_CREATOR(objects::actorColony,  ActorColony,  WorkingBuildingCreator );
  ADD_CREATOR(objects::gladiatorSchool, GladiatorSchool, WorkingBuildingCreator );
  ADD_CREATOR(objects::lionsNursery, LionsNursery, WorkingBuildingCreator );
  ADD_CREATOR(objects::chariotSchool,WorkshopChariot, WorkingBuildingCreator );
  ADD_CREATOR(objects::hippodrome,   Hippodrome, WorkingBuildingCreator );

  // road&house
  ADD_CREATOR(objects::house,        House, ConstructionCreator );
  ADD_CREATOR(objects::road,     Road, ConstructionCreator );

  // administration
  ADD_CREATOR(objects::forum_1,        Forum, WorkingBuildingCreator );
  ADD_CREATOR(objects::senate_1,       Senate, WorkingBuildingCreator );
  ADD_CREATOR(objects::governorHouse,GovernorsHouse, ConstructionCreator );
  ADD_CREATOR(objects::governorVilla,GovernorsVilla, ConstructionCreator );
  addCreator(objects::governorPalace, CAESARIA_STR_EXT(GovernorsPalace), new ConstructionCreator<GovernorsPalace>() );
  addCreator(objects::statue_small,    CAESARIA_STR_EXT(SmallStatue), new ConstructionCreator<SmallStatue>() );
  addCreator(objects::statue_middle,    CAESARIA_STR_EXT(MediumStatue), new ConstructionCreator<MediumStatue>() );
  addCreator(objects::statue_big,    CAESARIA_STR_EXT(BigStatue), new ConstructionCreator<BigStatue>() );
  addCreator(objects::garden, CAESARIA_STR_EXT(Garden) , new ConstructionCreator<Garden>() );
  ADD_CREATOR(objects::plaza,  Plaza, ConstructionCreator );

  // water
  ADD_CREATOR(objects::well,       Well, WorkingBuildingCreator );
  ADD_CREATOR(objects::fountain,   Fountain, WorkingBuildingCreator );
  addCreator(objects::aqueduct,   CAESARIA_STR_EXT(Aqueduct), new ConstructionCreator<Aqueduct>() );
  addCreator(objects::reservoir,  CAESARIA_STR_EXT(Reservoir), new ConstructionCreator<Reservoir>() );

  // security
  addCreator(objects::prefecture,   CAESARIA_STR_EXT(Prefecture)  , new WorkingBuildingCreator<Prefecture>() );
  addCreator(objects::fort_legionaries, CAESARIA_STR_EXT(FortLegionary), new WorkingBuildingCreator<FortLegionary>() );
  addCreator(objects::fort_javelin, CAESARIA_STR_EXT(FortJaveline)   , new WorkingBuildingCreator<FortJaveline>() );
  addCreator(objects::fort_horse, CAESARIA_STR_EXT(FortMounted)  , new WorkingBuildingCreator<FortMounted>() );
  ADD_CREATOR(objects::military_academy, MilitaryAcademy, WorkingBuildingCreator );
  addCreator(objects::barracks,   CAESARIA_STR_EXT(Barracks)        , new WorkingBuildingCreator<Barracks>() );
  ADD_CREATOR( objects::wall,          Wall, ConstructionCreator );
  ADD_CREATOR( objects::fortification, Fortification, ConstructionCreator );
  ADD_CREATOR( objects::gatehouse, Gatehouse, ConstructionCreator );
  ADD_CREATOR( objects::tower,     Tower, WorkingBuildingCreator );


  // commerce
  ADD_CREATOR(objects::market,     Market, WorkingBuildingCreator );
  ADD_CREATOR(objects::warehouse,  Warehouse, WorkingBuildingCreator );
  addCreator(objects::granery,      CAESARIA_STR_EXT(Granary)  , new WorkingBuildingCreator<Granary>() );

  // farms
  addCreator(objects::wheat_farm,    CAESARIA_STR_EXT(FarmWheat) , new FactoryCreator<FarmWheat>() );
  addCreator(objects::olive_farm, CAESARIA_STR_EXT(FarmOlive) , new FactoryCreator<FarmOlive>() );
  addCreator(objects::vinard,    CAESARIA_STR_EXT(FarmGrape) , new FactoryCreator<FarmGrape>() );
  addCreator(objects::meat_farm,   CAESARIA_STR_EXT(FarmMeat)     , new FactoryCreator<FarmMeat>() );
  addCreator(objects::fig_farm, CAESARIA_STR_EXT(FarmFruit)    , new FactoryCreator<FarmFruit>() );
  addCreator(objects::vegetable_farm, CAESARIA_STR_EXT(FarmVegetable), new FactoryCreator<FarmVegetable>() );

  // raw materials
  addCreator(objects::iron_mine,     CAESARIA_STR_EXT(IronMine)  , new FactoryCreator<IronMine>() );
  addCreator(objects::lumber_mill, CAESARIA_STR_EXT(TimberLogger), new FactoryCreator<TimberLogger>() );
  addCreator(objects::clay_pit,      CAESARIA_STR_EXT(ClayPit)  , new FactoryCreator<ClayPit>() );
  addCreator(objects::quarry, CAESARIA_STR_EXT(MarbleQuarry), new FactoryCreator<MarbleQuarry>() );

  // factories
  ADD_CREATOR(objects::weapons_workshop, WeaponsWorkshop, FactoryCreator );
  ADD_CREATOR(objects::furniture_workshop,  FurnitureWorkshop, FactoryCreator );
  ADD_CREATOR(objects::wine_workshop, Winery, FactoryCreator );
  ADD_CREATOR(objects::oil_workshop, Creamery, FactoryCreator );
  ADD_CREATOR(objects::pottery_workshop,  Pottery, FactoryCreator );

  // utility
  ADD_CREATOR(objects::engineering_post, EngineerPost, WorkingBuildingCreator );
  ADD_CREATOR(objects::low_bridge,  LowBridge, ConstructionCreator );
  ADD_CREATOR(objects::high_bridge, HighBridge, ConstructionCreator );
  ADD_CREATOR(objects::dock,       Dock    , WorkingBuildingCreator );
  ADD_CREATOR(objects::shipyard,   Shipyard, FactoryCreator );
  ADD_CREATOR(objects::wharf,      Wharf   , FactoryCreator );
  ADD_CREATOR(objects::triumphal_arch, TriumphalArch, ConstructionCreator );

  // religion
  ADD_CREATOR(objects::small_ceres_temple,  TempleCeres, WorkingBuildingCreator );
  ADD_CREATOR(objects::small_neptune_temple, TempleNeptune, WorkingBuildingCreator );
  ADD_CREATOR(objects::small_mars_temple, TempleMars, WorkingBuildingCreator );
  ADD_CREATOR(objects::small_venus_temple, TempleVenus, WorkingBuildingCreator );
  ADD_CREATOR(objects::small_mercury_temple, TempleMercury, WorkingBuildingCreator );
  ADD_CREATOR(objects::big_ceres_temple, BigTempleCeres, WorkingBuildingCreator );
  ADD_CREATOR(objects::big_neptune_temple, BigTempleNeptune, WorkingBuildingCreator );
  ADD_CREATOR(objects::big_mars_temple, BigTempleMars, WorkingBuildingCreator );
  ADD_CREATOR(objects::big_venus_temple, BigTempleVenus, WorkingBuildingCreator );
  ADD_CREATOR(objects::big_mercury_temple, BigTempleMercury, WorkingBuildingCreator );
  ADD_CREATOR(objects::oracle, TempleOracle, WorkingBuildingCreator );

  // health
  addCreator(objects::baths,      CAESARIA_STR_EXT(Baths)   , new WorkingBuildingCreator<Baths>() );
  addCreator(objects::barber,     CAESARIA_STR_EXT(Barber)  , new WorkingBuildingCreator<Barber>() );
  addCreator(objects::clinic,     CAESARIA_STR_EXT(Doctor)  , new WorkingBuildingCreator<Doctor>() );
  addCreator(objects::hospital,   CAESARIA_STR_EXT(Hospital), new WorkingBuildingCreator<Hospital>() );

  // education
  ADD_CREATOR(objects::school,   School, WorkingBuildingCreator );
  ADD_CREATOR(objects::library,  Library, WorkingBuildingCreator );
  ADD_CREATOR(objects::academy,    Academy, WorkingBuildingCreator );
  ADD_CREATOR(objects::missionaryPost, MissionaryPost, ConstructionCreator );

  // natives
  ADD_CREATOR(objects::native_hut, NativeHut, ConstructionCreator );
  ADD_CREATOR(objects::native_center, NativeCenter, ConstructionCreator );
  ADD_CREATOR(objects::native_field, NativeField, ConstructionCreator );

  //damages
  ADD_CREATOR(objects::burning_ruins, BurningRuins, ConstructionCreator );
  ADD_CREATOR(objects::burned_ruins, BurnedRuins, ConstructionCreator );
  ADD_CREATOR(objects::collapsed_ruins, CollapsedRuins, ConstructionCreator );
  ADD_CREATOR(objects::plague_ruins, PlagueRuins, ConstructionCreator);

  //places
  ADD_CREATOR( objects::elevation, Elevation, BaseCreator );
  ADD_CREATOR( objects::rift, Rift, BaseCreator );
  ADD_CREATOR( objects::tree, Tree, BaseCreator );
  ADD_CREATOR( objects::waymark, Waymark, BaseCreator );
  ADD_CREATOR( objects::river, River, BaseCreator );
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
