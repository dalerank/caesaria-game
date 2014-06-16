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
// Copyright 2012-2013 dalerank, dalerankn8@gmail.com

#include "infoboxmanager.hpp"
#include "gui/info_box.hpp"
#include "gui/environment.hpp"
#include "objects/road.hpp"
#include "objects/prefecture.hpp"
#include "gfx/tile.hpp"
#include "objects/service.hpp"
#include "objects/engineer_post.hpp"
#include "core/stringhelper.hpp"
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
#include <map>

using namespace constants;
using namespace gui;
using namespace gfx;

template< class T >
class BaseInfoboxCreator : public InfoboxCreator
{
public:
  gui::InfoboxSimple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    return new T( parent, city->tilemap().at( pos ) );
  }
};

class InfoboxHouseCreator : public InfoboxCreator
{
public:
  gui::InfoboxSimple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    HousePtr house = ptr_cast<House>( city->getOverlay( pos ) );
    if( house->habitants().count() > 0 )
    {
      return new InfoboxHouse( parent, city->tilemap().at( pos ) );
    }
    else
    {
      return new InfoboxFreeHouse( parent, city->tilemap().at( pos ) );
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

  gui::InfoboxSimple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    Size  size = parent->size();
    WorkingBuildingPtr building = ptr_cast<WorkingBuilding>( city->getOverlay( pos ) );
    if( building.isValid() )
    {
      InfoboxWorkingBuilding* infoBox = new InfoboxWorkingBuilding( parent, building );
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

  gui::InfoboxSimple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    Size  size = parent->size();
    InfoboxSimple* infoBox = new InfoboxSimple( parent, Rect( 0, 0, 510, 300 ) );
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

class InfoboxManager::Impl
{
public:
    bool showDebugInfo;

    typedef std::map< TileOverlay::Type, InfoboxCreator* > InfoboxCreators;
    std::map< std::string, TileOverlay::Type > name2typeMap;

    InfoboxCreators constructors;
};

InfoboxManager::InfoboxManager() : _d( new Impl )
{
  _d->showDebugInfo = true;

  InfoboxCitizenManager::instance().loadInfoboxes( *this );

#define ADD_INFOBOX(typen, creator) addInfobox(typen, CAESARIA_STR_EXT(typen), new creator)
  ADD_INFOBOX( building::reservoir,    BaseInfoboxCreator<InfoboxReservoir>() );
  ADD_INFOBOX( building::house,        InfoboxHouseCreator() );
  ADD_INFOBOX( building::prefecture,   ServiceBaseInfoboxCreator( "", "" ) );
  ADD_INFOBOX( building::engineerPost, ServiceBaseInfoboxCreator( "", "" ) );
  ADD_INFOBOX( building::well,         BaseInfoboxCreator<InfoboxWell>() );
  ADD_INFOBOX( building::doctor,       ServiceBaseInfoboxCreator( "", "" ) );
  ADD_INFOBOX( building::baths,        ServiceBaseInfoboxCreator( "", "" ) );
  ADD_INFOBOX( building::barber,       ServiceBaseInfoboxCreator( "", "" ) );
  ADD_INFOBOX( building::hospital,     ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::fountain,         CAESARIA_STR_EXT(B_FOUNTAIN),  new BaseInfoboxCreator<InfoboxFontain>() );
  addInfobox( building::aqueduct,         CAESARIA_STR_EXT(B_AQUEDUCT),  new InfoboxBasicCreator( "", "##aqueduct_info##") );
  addInfobox( building::market,           CAESARIA_STR_EXT(B_MARKET),    new BaseInfoboxCreator<InfoboxMarket>() );
  addInfobox( building::granary,          CAESARIA_STR_EXT(B_GRANARY),   new BaseInfoboxCreator<InfoboxGranary>() );
  addInfobox( building::grapeFarm,        CAESARIA_STR_EXT(B_GRAPE_FARM),new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::wheatFarm,        CAESARIA_STR_EXT(B_WHEAT_FARM),new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::vegetableFarm,    CAESARIA_STR_EXT(B_VEGETABLE_FARM), new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::oliveFarm,        CAESARIA_STR_EXT(B_OLIVE_FARM),new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::fruitFarm,        CAESARIA_STR_EXT(B_FRUIT_FARM),new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::warehouse,        CAESARIA_STR_EXT(B_WAREHOUSE), new BaseInfoboxCreator<InfoboxWarehouse>() );
  addInfobox( building::pigFarm,          CAESARIA_STR_EXT(B_PIG_FARM),  new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::templeCeres,      CAESARIA_STR_EXT(B_TEMPLE_CERES), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::templeMars,       CAESARIA_STR_EXT(B_TEMPLE_MARS), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::templeNeptune,    CAESARIA_STR_EXT(B_TEMPLE_NEPTUNE), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::templeVenus,      CAESARIA_STR_EXT(B_TEMPLE_VENUS), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::templeMercury,    CAESARIA_STR_EXT(B_TEMPLE_MERCURE), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::cathedralCeres,   CAESARIA_STR_EXT(B_BIG_TEMPLE_CERES), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::cathedralMars,    CAESARIA_STR_EXT(B_BIG_TEMPLE_MARS), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::cathedralNeptune, CAESARIA_STR_EXT(B_BIG_TEMPLE_NEPTUNE), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::cathedralVenus,   CAESARIA_STR_EXT(B_BIG_TEMPLE_VENUS), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::cathedralMercury, CAESARIA_STR_EXT(B_BIG_TEMPLE_MERCURE), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::oracle,           CAESARIA_STR_EXT(B_TEMPLE_ORACLE), new BaseInfoboxCreator<InfoboxTemple>() );
  addInfobox( building::school,           CAESARIA_STR_EXT(B_SCHOOL),    new ServiceBaseInfoboxCreator( "", "" ));
  addInfobox( building::academy,          CAESARIA_STR_EXT(B_COLLEGE),   new ServiceBaseInfoboxCreator( "", "" ));
  addInfobox( building::library,          CAESARIA_STR_EXT(B_LIBRARY),   new ServiceBaseInfoboxCreator( "", "" ));
  addInfobox( construction::garden,       CAESARIA_STR_EXT(B_GARDEN),    new InfoboxBasicCreator( "", "##garden_info##" )  );
  addInfobox( building::smallStatue,      CAESARIA_STR_EXT(B_STATUE1),   new InfoboxBasicCreator( "", "##statue_info##") );
  addInfobox( building::middleStatue,     CAESARIA_STR_EXT(B_STATUE2),   new InfoboxBasicCreator( "", "##statue_info##") );
  addInfobox( building::bigStatue,        CAESARIA_STR_EXT(B_STATUE3),   new InfoboxBasicCreator( "", "##statue_info##") );
  addInfobox( building::pottery,          CAESARIA_STR_EXT(Pottery),   new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::shipyard,         CAESARIA_STR_EXT(Shipyard),   new BaseInfoboxCreator<InfoboxShipyard>() );
  addInfobox( building::weaponsWorkshop,  CAESARIA_STR_EXT(WeaponsWorkshop), new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::furnitureWorkshop,CAESARIA_STR_EXT(FurnitureWorkshop), new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::clayPit,          CAESARIA_STR_EXT(ClayPit),  new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::timberLogger,     CAESARIA_STR_EXT(TimberLogger), new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::marbleQuarry,     CAESARIA_STR_EXT(MarbleQuarry), new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::ironMine,         CAESARIA_STR_EXT(IronMine), new BaseInfoboxCreator<InfoboxRawMaterial>() );
  addInfobox( building::dock,             CAESARIA_STR_EXT(Dock), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::winery,           CAESARIA_STR_EXT(Winery), new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::creamery,         CAESARIA_STR_EXT(Creamery), new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::senate,           CAESARIA_STR_EXT(Senate),    new BaseInfoboxCreator<InfoboxSenate>() );
  addInfobox( building::theater,          CAESARIA_STR_EXT(Theater),     new BaseInfoboxCreator<InfoboxTheater>() );
  addInfobox( building::actorColony,      CAESARIA_STR_EXT(ActorColony), new ServiceBaseInfoboxCreator( "", "" ) );
  ADD_INFOBOX( building::amphitheater,    BaseInfoboxCreator<InfoboxAmphitheater>() );
  addInfobox( building::gladiatorSchool,  CAESARIA_STR_EXT(GladiatorSchool), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::colloseum,        CAESARIA_STR_EXT(Collosseum), new BaseInfoboxCreator<InfoboxColosseum>() );
  addInfobox( building::lionsNursery,     CAESARIA_STR_EXT(LionsNursery), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::hippodrome,       CAESARIA_STR_EXT(Hippodrome), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::chariotSchool,    CAESARIA_STR_EXT(chariotSchool),new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::forum,            CAESARIA_STR_EXT(Forum),        new ServiceBaseInfoboxCreator("", "" ) );
  addInfobox( building::governorHouse,    CAESARIA_STR_EXT(governorHouse),new ServiceBaseInfoboxCreator( "", "##governor_house_text##") );
  addInfobox( building::governorVilla,    CAESARIA_STR_EXT(governorVilla),new ServiceBaseInfoboxCreator( "", "##governor_villa_text##") );
  addInfobox( building::governorPalace,   CAESARIA_STR_EXT(governorPalace), new ServiceBaseInfoboxCreator( "", "##governor_palace_text##") );
  addInfobox( building::highBridge,       CAESARIA_STR_EXT(HighBridge),   new InfoboxBasicCreator( "", "##high_bridge_text##") );
  addInfobox( building::lowBridge,        CAESARIA_STR_EXT(LowBridge),    new InfoboxBasicCreator( "", "##low_bridge_text##") );
  addInfobox( building::wharf,            CAESARIA_STR_EXT(Wharf),        new BaseInfoboxCreator<InfoboxWharf>() );
  addInfobox( building::burningRuins,     CAESARIA_STR_EXT(BurningRuins), new BaseInfoboxCreator<InfoboxRuins>() );
  addInfobox( building::collapsedRuins,   CAESARIA_STR_EXT(CollapsedRuins), new BaseInfoboxCreator<InfoboxRuins>() );
  addInfobox( building::burnedRuins,      CAESARIA_STR_EXT(BurnedRuins), new BaseInfoboxCreator<InfoboxRuins>() );
  addInfobox( building::plagueRuins,      CAESARIA_STR_EXT(PlagueRuins), new BaseInfoboxCreator<InfoboxRuins>() );
}

InfoboxManager::~InfoboxManager() {}

InfoboxManager& InfoboxManager::getInstance()
{
  static InfoboxManager inst;
  return inst;
}

void InfoboxManager::showHelp( PlayerCityPtr city, GuiEnv* gui, TilePos pos )
{
  Tile& tile = city->tilemap().at( pos );
  TileOverlayPtr overlay = tile.overlay();
  TileOverlay::Type type;

  if( _d->showDebugInfo )
  {
    Logger::warning( "Tile debug info: dsrbl=%d", tile.getDesirability() );
  }

  type = overlay.isNull() ? building::unknown : overlay->type();

  Impl::InfoboxCreators::iterator findConstructor = _d->constructors.find( type );

  InfoboxSimple* infoBox = findConstructor != _d->constructors.end()
                                  ? findConstructor->second->create( city, gui->rootWidget(), pos )
                                  : 0;
  
  if( infoBox && infoBox->isAutoPosition() )
  {
    Size rSize = gui->rootWidget()->size();
    int y = ( gui->getCursorPos().y() < rSize.height() / 2 )
                ? rSize.height() - infoBox->height() - 5
                : 30;
    Point pos( ( rSize.width() - infoBox->width() ) / 2, y );

    infoBox->setPosition( pos );
    infoBox->setFocus();
  }
}

void InfoboxManager::setShowDebugInfo( const bool showInfo ) {  _d->showDebugInfo = showInfo; }

void InfoboxManager::addInfobox( const TileOverlay::Type type, const std::string& typeName, InfoboxCreator* ctor )
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

bool InfoboxManager::canCreate(const TileOverlay::Type type) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
