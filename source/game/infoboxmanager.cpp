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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "infoboxmanager.hpp"
#include "gui/info_box.hpp"
#include "gui/environment.hpp"
#include "objects/road.hpp"
#include "objects/prefecture.hpp"
#include "gfx/tile.hpp"
#include "objects/service.hpp"
#include "objects/engineer_post.hpp"
#include "core/utils.hpp"
#include "objects/house.hpp"
#include "core/gettext.hpp"
#include "city/helper.hpp"
#include "gui/infobox_warehouse.hpp"
#include "gui/infobox_market.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "walker/walker.hpp"
#include "gfx/tilemap.hpp"
#include "gui/infobox_house.hpp"
#include "gui/infobox_raw.hpp"
#include "objects/ruins.hpp"
#include "gui/infobox_ruins.hpp"
#include "gui/infobox_granary.hpp"
#include "gui/infobox_senate.hpp"
#include "gui/infobox_factory.hpp"
#include "gui/infobox_watersupply.hpp"
#include "gui/infobox_citizen_mgr.hpp"
#include "gui/infobox_land.hpp"
#include "gui/infobox_temple.hpp"
#include "gui/infobox_working.hpp"
#include "gui/infobox_colloseum.hpp"
#include "gui/infobox_theater.hpp"
#include "gui/infobox_amphitheater.hpp"
#include "gui/infobox_fort.hpp"
#include "gui/infobox_barracks.hpp"
#include <map>

using namespace constants;
using namespace gui;
using namespace gfx;

namespace gui
{

namespace infobox
{

template< class T >
class BaseInfoboxCreator : public InfoboxCreator
{
public:
  Simple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    return new T( parent, city, city->tilemap().at( pos ) );
  }
};

class InfoboxHouseCreator : public InfoboxCreator
{
public:
  Simple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    HousePtr house = ptr_cast<House>( city->getOverlay( pos ) );
    if( house->habitants().count() > 0 )
    {
      return new AboutHouse( parent, city, city->tilemap().at( pos ) );
    }
    else
    {
      return new AboutFreeHouse( parent, city, city->tilemap().at( pos ) );
    }
  }
};

class ServiceBaseInfoboxCreator : public InfoboxCreator
{
public:
  ServiceBaseInfoboxCreator( const std::string& caption,
                             const std::string& descr,
                             bool drawWorkers=false )
    : title( caption ), text( descr ), isDrawWorkers( drawWorkers )
  {
  }
  
  virtual ~ServiceBaseInfoboxCreator() {}

  Simple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    Size  size = parent->size();
    WorkingBuildingPtr building = ptr_cast<WorkingBuilding>( city->getOverlay( pos ) );
    if( building.isValid() )
    {
      AboutWorkingBuilding* infoBox = new AboutWorkingBuilding( parent, building );
      infoBox->setPosition( Point( (size.width() - infoBox->width()) / 2, size.height() - infoBox->height()) );

      if( !title.empty() ) { infoBox->setTitle( title ); }
      if( !text.empty() ) { infoBox->setText( text ); }
      return infoBox;
    }
    
    return 0;
  }

  std::string title, text;
  bool isDrawWorkers;
};

class InfoboxBasicCreator : public InfoboxCreator
{
public:
  InfoboxBasicCreator( const std::string& caption,
                       const std::string& desc ):
    title( caption ), text( desc )
  {}
  
  virtual ~InfoboxBasicCreator() {}

  Simple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    Size  size = parent->size();
    Simple* infoBox = new Simple( parent, Rect( 0, 0, 510, 300 ) );
    infoBox->setPosition( Point( (size.width() - infoBox->width()) / 2, 
                                  size.height() - infoBox->height()) );
    TileOverlayPtr overlay = city->getOverlay( pos );

    std::string caption = overlay.isValid()
                            ? MetaDataHolder::findPrettyName( overlay->type() )
                            : title;

    infoBox->setTitle( _( caption ) );
    infoBox->setText( _( text ) );
    return infoBox;
  }

  std::string title, text;
};

class Manager::Impl
{
public:
  bool showDebugInfo;

  typedef std::map< TileOverlay::Type, InfoboxCreator* > InfoboxCreators;
  std::map< std::string, TileOverlay::Type > name2typeMap;

  InfoboxCreators constructors;
};

Manager::Manager() : _d( new Impl )
{
  _d->showDebugInfo = true;

  citizen::PManager::instance().loadInfoboxes( *this );

#define ADD_INFOBOX(typen, creator) addInfobox(typen, CAESARIA_STR_EXT(typen), new creator);
  ADD_INFOBOX( objects::reservoir,    BaseInfoboxCreator<AboutReservoir>() )
  ADD_INFOBOX( objects::house,        InfoboxHouseCreator() )
  ADD_INFOBOX( objects::prefecture,   ServiceBaseInfoboxCreator( "", "" ) )
  ADD_INFOBOX( objects::engineering_post, ServiceBaseInfoboxCreator( "", "" ) )
  ADD_INFOBOX( objects::well,         BaseInfoboxCreator<AboutWell>() )
  ADD_INFOBOX( objects::clinic,       ServiceBaseInfoboxCreator( "", "" ) )
  ADD_INFOBOX( objects::baths,        ServiceBaseInfoboxCreator( "", "" ) )
  ADD_INFOBOX( objects::barber,       ServiceBaseInfoboxCreator( "", "" ) )
  ADD_INFOBOX( objects::hospital,     ServiceBaseInfoboxCreator( "", "" ) )
  ADD_INFOBOX( objects::fountain,     BaseInfoboxCreator<AboutFontain> )
  ADD_INFOBOX( objects::missionaryPost, ServiceBaseInfoboxCreator( "", "") )
  ADD_INFOBOX( objects::elevation,    InfoboxBasicCreator( "", "##elevation_info##" ) )
  addInfobox( objects::aqueduct,         CAESARIA_STR_EXT(aqueduct),  new InfoboxBasicCreator( "", "##aqueduct_info##") );
  addInfobox( objects::market,           CAESARIA_STR_EXT(market),    new BaseInfoboxCreator<AboutMarket>() );
  addInfobox( objects::granery,          CAESARIA_STR_EXT(granary),   new BaseInfoboxCreator<AboutGranary>() );
  addInfobox( objects::vinard,        CAESARIA_STR_EXT(grapeFarm),new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::wheat_farm,        CAESARIA_STR_EXT(wheatFarm),new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::vegetable_farm,    CAESARIA_STR_EXT(vegetableFarm), new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::olive_farm,        CAESARIA_STR_EXT(oliveFarm),new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::fig_farm,        CAESARIA_STR_EXT(fruitFarm),new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::warehouse,        CAESARIA_STR_EXT(B_WAREHOUSE), new BaseInfoboxCreator<AboutWarehouse>() );
  addInfobox( objects::meat_farm,          CAESARIA_STR_EXT(B_PIG_FARM),  new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::small_ceres_temple,      CAESARIA_STR_EXT(B_TEMPLE_CERES), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::small_mars_temple,       CAESARIA_STR_EXT(B_TEMPLE_MARS), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::small_neptune_temple,    CAESARIA_STR_EXT(B_TEMPLE_NEPTUNE), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::small_venus_temple,      CAESARIA_STR_EXT(B_TEMPLE_VENUS), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::small_mercury_temple,    CAESARIA_STR_EXT(B_TEMPLE_MERCURY), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::big_ceres_temple,   CAESARIA_STR_EXT(B_BIG_TEMPLE_CERES), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::big_mars_temple,    CAESARIA_STR_EXT(B_BIG_TEMPLE_MARS), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::big_neptune_temple, CAESARIA_STR_EXT(B_BIG_TEMPLE_NEPTUNE), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::big_venus_temple,   CAESARIA_STR_EXT(B_BIG_TEMPLE_VENUS), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::big_mercury_temple, CAESARIA_STR_EXT(B_BIG_TEMPLE_MERCURY), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::oracle,           CAESARIA_STR_EXT(B_TEMPLE_ORACLE), new BaseInfoboxCreator<AboutTemple>() );
  addInfobox( objects::school,           CAESARIA_STR_EXT(B_SCHOOL),    new ServiceBaseInfoboxCreator( "", "" ));
  addInfobox( objects::academy,          CAESARIA_STR_EXT(B_COLLEGE),   new ServiceBaseInfoboxCreator( "", "" ));
  addInfobox( objects::library,          CAESARIA_STR_EXT(B_LIBRARY),   new ServiceBaseInfoboxCreator( "", "" ));
  addInfobox( objects::garden,       CAESARIA_STR_EXT(B_GARDEN),    new InfoboxBasicCreator( "", "##garden_info##" )  );
  addInfobox( objects::statue_small,      CAESARIA_STR_EXT(B_STATUE1),   new InfoboxBasicCreator( "", "##statue_small_info##") );
  addInfobox( objects::statue_middle,     CAESARIA_STR_EXT(B_STATUE2),   new InfoboxBasicCreator( "", "##statue_middle_info##") );
  addInfobox( objects::statue_big,        CAESARIA_STR_EXT(B_STATUE3),   new InfoboxBasicCreator( "", "##statue_big_info##") );
  addInfobox( objects::native_hut,        CAESARIA_STR_EXT(NativeHut),   new InfoboxBasicCreator( "", "##nativeHut_info##") );
  ADD_INFOBOX( objects::gatehouse,   InfoboxBasicCreator( "", "##gatehouse_info##") );
  ADD_INFOBOX( objects::native_field, InfoboxBasicCreator( "", "##nativeField_info##") );
  addInfobox( objects::native_center,     CAESARIA_STR_EXT(NativeCenter),   new InfoboxBasicCreator( "", "##nativeCenter_info##") );
  addInfobox( objects::pottery_workshop,          CAESARIA_STR_EXT(Pottery),   new BaseInfoboxCreator<AboutFactory>() );
  addInfobox( objects::shipyard,         CAESARIA_STR_EXT(Shipyard),   new BaseInfoboxCreator<AboutShipyard>() );
  addInfobox( objects::weapons_workshop,  CAESARIA_STR_EXT(WeaponsWorkshop), new BaseInfoboxCreator<AboutFactory>() );
  addInfobox( objects::furniture_workshop,CAESARIA_STR_EXT(FurnitureWorkshop), new BaseInfoboxCreator<AboutFactory>() );
  addInfobox( objects::clay_pit,          CAESARIA_STR_EXT(ClayPit),  new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::lumber_mill,     CAESARIA_STR_EXT(TimberLogger), new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::quarry,     CAESARIA_STR_EXT(MarbleQuarry), new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::iron_mine,         CAESARIA_STR_EXT(IronMine), new BaseInfoboxCreator<AboutRawMaterial>() );
  addInfobox( objects::dock,             CAESARIA_STR_EXT(Dock), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( objects::wine_workshop,           CAESARIA_STR_EXT(Winery), new BaseInfoboxCreator<AboutFactory>() );
  addInfobox( objects::oil_workshop,         CAESARIA_STR_EXT(Creamery), new BaseInfoboxCreator<AboutFactory>() );
  addInfobox( objects::senate,           CAESARIA_STR_EXT(Senate),    new BaseInfoboxCreator<AboutSenate>() );
  addInfobox( objects::theater,          CAESARIA_STR_EXT(Theater),     new BaseInfoboxCreator<AboutTheater>() );
  addInfobox( objects::actorColony,      CAESARIA_STR_EXT(ActorColony), new ServiceBaseInfoboxCreator( "", "" ) );
  ADD_INFOBOX( objects::amphitheater,    BaseInfoboxCreator<AboutAmphitheater>() )
  addInfobox( objects::gladiatorSchool,  CAESARIA_STR_EXT(GladiatorSchool), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( objects::colloseum,        CAESARIA_STR_EXT(Collosseum), new BaseInfoboxCreator<AboutColosseum>() );
  addInfobox( objects::lionsNursery,     CAESARIA_STR_EXT(LionsNursery), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( objects::hippodrome,       CAESARIA_STR_EXT(Hippodrome), new ServiceBaseInfoboxCreator( "", "" ) );
  ADD_INFOBOX( objects::chariotSchool, ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( objects::forum,            CAESARIA_STR_EXT(Forum),        new ServiceBaseInfoboxCreator("", "" ) );
  addInfobox( objects::governorHouse,    CAESARIA_STR_EXT(governorHouse),new ServiceBaseInfoboxCreator( "", "##governor_house_text##") );
  ADD_INFOBOX( objects::governorVilla, ServiceBaseInfoboxCreator( "", "##governor_villa_text##") )
  addInfobox( objects::governorPalace,   CAESARIA_STR_EXT(governorPalace), new ServiceBaseInfoboxCreator( "", "##governor_palace_text##") );
  addInfobox( objects::high_bridge,       CAESARIA_STR_EXT(HighBridge),   new InfoboxBasicCreator( "", "##high_bridge_info##") );
  addInfobox( objects::low_bridge,        CAESARIA_STR_EXT(LowBridge),    new InfoboxBasicCreator( "", "##bridge_extends_city_area##") );
  addInfobox( objects::wharf,            CAESARIA_STR_EXT(Wharf),        new BaseInfoboxCreator<AboutWharf>() );
  ADD_INFOBOX( objects::burning_ruins,    InfoboxBasicCreator( "", "##this_fire_can_spread##" ) );
  ADD_INFOBOX( objects::rift,    InfoboxBasicCreator( "", "##these_rift_info##" ) );
  addInfobox( objects::collapsed_ruins,   CAESARIA_STR_EXT(CollapsedRuins), new BaseInfoboxCreator<AboutRuins>() );
  addInfobox( objects::burned_ruins,      CAESARIA_STR_EXT(BurnedRuins), new BaseInfoboxCreator<AboutRuins>() );
  addInfobox( objects::plague_ruins,      CAESARIA_STR_EXT(PlagueRuins), new BaseInfoboxCreator<AboutRuins>() );
  ADD_INFOBOX( objects::fort_legionaries,   BaseInfoboxCreator<AboutFort>() )
  ADD_INFOBOX( objects::fort_javelin,   BaseInfoboxCreator<AboutFort>() )
  ADD_INFOBOX( objects::fort_horse,   BaseInfoboxCreator<AboutFort>() )
  ADD_INFOBOX( objects::fortArea, BaseInfoboxCreator<AboutFort>() );
  ADD_INFOBOX( objects::barracks, BaseInfoboxCreator<AboutBarracks>() );
  ADD_INFOBOX( objects::tree, BaseInfoboxCreator<AboutLand>() );
}

Manager::~Manager() {}

Manager& Manager::getInstance()
{
  static Manager inst;
  return inst;
}

void Manager::showHelp( PlayerCityPtr city, Ui* gui, TilePos pos )
{
  Tile& tile = city->tilemap().at( pos );
  TileOverlayPtr overlay = tile.overlay();
  TileOverlay::Type type;

  if( _d->showDebugInfo )
  {
    Logger::warning( "Tile debug info: dsrbl=%d", tile.param( Tile::pDesirability ) );
  }

  type = overlay.isNull() ? objects::unknown : overlay->type();

  Impl::InfoboxCreators::iterator findConstructor = _d->constructors.find( type );

  Simple* infoBox = findConstructor != _d->constructors.end()
                                  ? findConstructor->second->create( city, gui->rootWidget(), pos )
                                  : 0;
  
  if( infoBox && infoBox->isAutoPosition() )
  {
    Size rSize = gui->rootWidget()->size();
    int y = ( gui->cursorPos().y() < rSize.height() / 2 )
                ? rSize.height() - infoBox->height() - 5
                : 30;
    Point pos( ( rSize.width() - infoBox->width() ) / 2, y );

    infoBox->setPosition( pos );
    infoBox->setFocus();
  }
}

void Manager::setShowDebugInfo( const bool showInfo ) {  _d->showDebugInfo = showInfo; }

void Manager::addInfobox( const TileOverlay::Type type, const std::string& typeName, InfoboxCreator* ctor )
{
  bool alreadyHaveConstructor = _d->name2typeMap.find( typeName ) != _d->name2typeMap.end();  

  if( !alreadyHaveConstructor )
  {
    _d->name2typeMap[ typeName ] = type;
    _d->constructors[ type ] = ctor;
  }
  else
  {
    Logger::warning( "InfoboxManager: already have constructor for type " + typeName );
  }
}

bool Manager::canCreate(const TileOverlay::Type type) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}


}

}
