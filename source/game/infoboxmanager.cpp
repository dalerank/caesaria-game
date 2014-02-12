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
#include <map>

using namespace constants;
using namespace gui;

template< class T >
class BaseInfoboxCreator : public InfoboxCreator
{
public:
  gui::InfoboxSimple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    return new T( parent, city->getTilemap().at( pos ) );
  }
};

class InfoboxHouseCreator : public InfoboxCreator
{
public:
  gui::InfoboxSimple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    HousePtr house = ptr_cast<House>( city->getOverlay( pos ) );
    if( house->getHabitants().count() > 0 )
    {
      return new InfoBoxHouse( parent, city->getTilemap().at( pos ) );
    }
    else
    {
      return new InfoBoxFreeHouse( parent, city->getTilemap().at( pos ) );
    }
  }
};

class ServiceBaseInfoboxCreator : public InfoboxCreator
{
public:
  ServiceBaseInfoboxCreator( const std::string& caption,
                             const std::string& descr,
                             bool drawWorkers=false )
  {
    title = caption;
    text = descr;
    isDrawWorkers = drawWorkers;
  }
  
  virtual ~ServiceBaseInfoboxCreator() {}

  gui::InfoboxSimple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    Size  size = parent->getSize();
    WorkingBuildingPtr building = ptr_cast<WorkingBuilding>( city->getOverlay( pos ) );
    if( building.isValid() )
    {
      InfoboxWorkingBuilding* infoBox = new InfoboxWorkingBuilding( parent, building );
      infoBox->setPosition( Point( (size.getWidth() - infoBox->getWidth()) / 2, size.getHeight() - infoBox->getHeight()) );

      if( !title.empty() ) { infoBox->setTitle( title ); }
      if( !text.empty() ) { infoBox->setText( text ); }
      return infoBox;
    }
    
    return 0;
  }

  std::string title, text;
  bool isDrawWorkers;
};

class InfoBoxBasicCreator : public InfoboxCreator
{
public:
  InfoBoxBasicCreator( const std::string& caption,
                       const std::string& desc )
  {
    title = caption;
    text = desc;
  }
  
  virtual ~InfoBoxBasicCreator() {}

  gui::InfoboxSimple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    Size  size = parent->getSize();
    InfoboxSimple* infoBox = new InfoboxSimple( parent, Rect( 0, 0, 510, 300 ) );
    infoBox->setPosition( Point( (size.getWidth() - infoBox->getWidth()) / 2, 
                                  size.getHeight() - infoBox->getHeight()) );
    TileOverlayPtr overlay = city->getOverlay( pos );

    std::string caption = overlay.isValid()
                            ? MetaDataHolder::getPrettyName( overlay->getType() )
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

  InfoboxCitizenManager::loadInfoboxes( *this );

#define ADD_INFOBOX(typen, creator) addInfobox(typen, CAESARIA_STR_EXT(typen), new creator)
  ADD_INFOBOX( building::reservoir,    BaseInfoboxCreator<InfoboxReservoir>() );
  ADD_INFOBOX( building::house,        InfoboxHouseCreator() );
  ADD_INFOBOX( building::prefecture,   ServiceBaseInfoboxCreator( "", "" ) );
  ADD_INFOBOX( building::engineerPost, ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::well,             CAESARIA_STR_EXT(Well),        new BaseInfoboxCreator<InfoboxWell>() );
  addInfobox( building::doctor,           CAESARIA_STR_EXT(Doctor),      new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::baths,            CAESARIA_STR_EXT(Baths),       new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::barber,           CAESARIA_STR_EXT(Barber),      new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::hospital,         CAESARIA_STR_EXT(Hospital),    new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::fountain,         CAESARIA_STR_EXT(B_FOUNTAIN),  new BaseInfoboxCreator<InfoboxFontain>() );
  addInfobox( building::aqueduct,         CAESARIA_STR_EXT(B_AQUEDUCT),  new InfoBoxBasicCreator( "", "##aqueduct_info##") );
  addInfobox( building::market,           CAESARIA_STR_EXT(B_MARKET),    new BaseInfoboxCreator<InfoBoxMarket>() );
  addInfobox( building::granary,          CAESARIA_STR_EXT(B_GRANARY),   new BaseInfoboxCreator<InfoBoxGranary>() );
  addInfobox( building::grapeFarm,        CAESARIA_STR_EXT(B_GRAPE_FARM),new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::wheatFarm,        CAESARIA_STR_EXT(B_WHEAT_FARM),new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::vegetableFarm,    CAESARIA_STR_EXT(B_VEGETABLE_FARM), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::oliveFarm,        CAESARIA_STR_EXT(B_OLIVE_FARM),new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::fruitFarm,        CAESARIA_STR_EXT(B_FRUIT_FARM),new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::warehouse,        CAESARIA_STR_EXT(B_WAREHOUSE), new BaseInfoboxCreator<InfoBoxWarehouse>() );
  addInfobox( building::pigFarm,          CAESARIA_STR_EXT(B_PIG_FARM),  new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::templeCeres,      CAESARIA_STR_EXT(B_TEMPLE_CERES), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::templeMars,       CAESARIA_STR_EXT(B_TEMPLE_MARS), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::templeNeptune,    CAESARIA_STR_EXT(B_TEMPLE_NEPTUNE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::templeVenus,      CAESARIA_STR_EXT(B_TEMPLE_VENUS), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::templeMercury,    CAESARIA_STR_EXT(B_TEMPLE_MERCURE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::cathedralCeres,   CAESARIA_STR_EXT(B_BIG_TEMPLE_CERES), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::cathedralMars,    CAESARIA_STR_EXT(B_BIG_TEMPLE_MARS), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::cathedralNeptune, CAESARIA_STR_EXT(B_BIG_TEMPLE_NEPTUNE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::cathedralVenus,   CAESARIA_STR_EXT(B_BIG_TEMPLE_VENUS), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::cathedralMercury, CAESARIA_STR_EXT(B_BIG_TEMPLE_MERCURE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::oracle,           CAESARIA_STR_EXT(B_TEMPLE_ORACLE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::school,           CAESARIA_STR_EXT(B_SCHOOL),    new ServiceBaseInfoboxCreator( "", "" ));
  addInfobox( building::academy,          CAESARIA_STR_EXT(B_COLLEGE),   new ServiceBaseInfoboxCreator( "", "" ));
  addInfobox( building::library,          CAESARIA_STR_EXT(B_LIBRARY),   new ServiceBaseInfoboxCreator( "", "" ));
  addInfobox( construction::garden,       CAESARIA_STR_EXT(B_GARDEN),    new InfoBoxBasicCreator( "", "##garden_info##" )  );
  addInfobox( building::smallStatue,      CAESARIA_STR_EXT(B_STATUE1),   new InfoBoxBasicCreator( "", "##statue_info##") );
  addInfobox( building::middleStatue,     CAESARIA_STR_EXT(B_STATUE2),   new InfoBoxBasicCreator( "", "##statue_info##") );
  addInfobox( building::bigStatue,        CAESARIA_STR_EXT(B_STATUE3),   new InfoBoxBasicCreator( "", "##statue_info##") );
  addInfobox( building::pottery,          CAESARIA_STR_EXT(Pottery),   new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::shipyard,         CAESARIA_STR_EXT(Shipyard),   new BaseInfoboxCreator<InfoboxShipyard>() );
  addInfobox( building::weaponsWorkshop,  CAESARIA_STR_EXT(WeaponsWorkshop), new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::furnitureWorkshop,CAESARIA_STR_EXT(FurnitureWorkshop), new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::clayPit,          CAESARIA_STR_EXT(ClayPit),  new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::timberLogger,     CAESARIA_STR_EXT(TimberLogger), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::marbleQuarry,     CAESARIA_STR_EXT(MarbleQuarry), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::ironMine,         CAESARIA_STR_EXT(IronMine), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::dock,             CAESARIA_STR_EXT(Dock), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::winery,           CAESARIA_STR_EXT(Winery), new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::creamery,         CAESARIA_STR_EXT(Creamery), new BaseInfoboxCreator<InfoboxFactory>() );
  addInfobox( building::senate,           CAESARIA_STR_EXT(Senate),    new BaseInfoboxCreator<InfoBoxSenate>() );
  addInfobox( building::theater,          CAESARIA_STR_EXT(Theater),     new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::actorColony,      CAESARIA_STR_EXT(ActorColony), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::amphitheater,     CAESARIA_STR_EXT(Amphitheater), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::gladiatorSchool,  CAESARIA_STR_EXT(GladiatorSchool), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::colloseum,        CAESARIA_STR_EXT(Collosseum), new BaseInfoboxCreator<InfoboxColosseum>() );
  addInfobox( building::lionsNursery,     CAESARIA_STR_EXT(LionsNursery), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::hippodrome,       CAESARIA_STR_EXT(Hippodrome), new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::chariotSchool,    CAESARIA_STR_EXT(chariotSchool),new ServiceBaseInfoboxCreator( "", "" ) );
  addInfobox( building::forum,            CAESARIA_STR_EXT(Forum),        new ServiceBaseInfoboxCreator("", "" ) );
  addInfobox( building::governorHouse,    CAESARIA_STR_EXT(governorHouse),new ServiceBaseInfoboxCreator( "", "##governor_house_text##") );
  addInfobox( building::governorVilla,    CAESARIA_STR_EXT(governorVilla),new ServiceBaseInfoboxCreator( "", "##governor_villa_text##") );
  addInfobox( building::governorPalace,   CAESARIA_STR_EXT(governorPalace), new ServiceBaseInfoboxCreator( "", "##governor_palace_text##") );
  addInfobox( building::highBridge,       CAESARIA_STR_EXT(HighBridge),   new InfoBoxBasicCreator( "", "##high_bridge_text##") );
  addInfobox( building::lowBridge,        CAESARIA_STR_EXT(LowBridge),    new InfoBoxBasicCreator( "", "##low_bridge_text##") );
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
  Tile& tile = city->getTilemap().at( pos );
  TileOverlayPtr overlay = tile.getOverlay();
  TileOverlay::Type type;

  if( _d->showDebugInfo )
  {
    Logger::warning( "Tile debug info: dsrbl=%d", tile.getDesirability() );
  }

  type = overlay.isNull() ? building::unknown : overlay->getType();

  Impl::InfoboxCreators::iterator findConstructor = _d->constructors.find( type );

  InfoboxSimple* infoBox = findConstructor != _d->constructors.end()
                                  ? findConstructor->second->create( city, gui->getRootWidget(), pos )
                                  : 0;
  
  if( infoBox && infoBox->isAutoPosition() )
  {
    Size rSize = gui->getRootWidget()->getSize();
    int y = ( gui->getCursorPos().y() < rSize.getHeight() / 2 )
                ? rSize.getHeight() - infoBox->getHeight() - 5
                : 30;
    Point pos( ( rSize.getWidth() - infoBox->getWidth() ) / 2, y );

    infoBox->setPosition( pos );
  }
}

void InfoboxManager::setShowDebugInfo( const bool showInfo )
{
  _d->showDebugInfo = showInfo;
} 

void InfoboxManager::addInfobox( const TileOverlay::Type type, const std::string& typeName, InfoboxCreator* ctor )
{
  bool alreadyHaveConstructor = _d->name2typeMap.find( typeName ) != _d->name2typeMap.end();
  _CAESARIA_DEBUG_BREAK_IF( alreadyHaveConstructor && "already have constructor for this type");

  if( !alreadyHaveConstructor )
  {
    _d->name2typeMap[ typeName ] = type;
    _d->constructors[ type ] = ctor;
  }
}

bool InfoboxManager::canCreate(const TileOverlay::Type type) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
