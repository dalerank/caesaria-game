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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "infoboxmanager.hpp"
#include "gui/info_box.hpp"
#include "gui/environment.hpp"
#include "road.hpp"
#include "building/prefecture.hpp"
#include "gfx/tile.hpp"
#include "building/service.hpp"
#include "building/engineer_post.hpp"
#include "core/stringhelper.hpp"
#include "building/house.hpp"
#include "core/gettext.hpp"
#include "game/city.hpp"
#include "gui/warehouse_infobox.hpp"
#include "core/logger.hpp"
#include "building/constants.hpp"
#include <map>

using namespace constants;
using namespace gui;

template< class T >
class BaseInfoboxCreator : public InfoboxCreator
{
public:
  gui::InfoBoxSimple* create( gui::Widget* parent, const Tile& tile )
  {
    return new T( parent, tile );
  }
};


class InfoBoxHouseCreator : public InfoboxCreator
{
public:
  gui::InfoBoxSimple* create( gui::Widget* parent, const Tile& tile )
  {
    HousePtr house = tile.getOverlay().as<House>();
    if( house->getHabitants().count() > 0 )
    {
      return new InfoBoxHouse( parent, tile );
    }
    else
    {
      return new InfoBoxFreeHouse( parent, tile );
    }
  }
};


template< class T >
class CitizenInfoboxCreator : public InfoboxCreator
{
public:
  CitizenInfoboxCreator( CityPtr c ) : city(c) {}

  gui::InfoBoxSimple* create( gui::Widget* parent, const Tile& tile )
  {
    CityHelper helper( city );
    WalkerList walkers = helper.find<Walker>( walker::any, tile.getIJ() );

    if( walkers.empty() )
    {
      return new T( parent, tile );
    }
    else
    {
      return new InfoBoxCitizen( parent, walkers );
    }
  }

  CityPtr city;
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

  gui::InfoBoxSimple* create( gui::Widget* parent, const Tile& tile )
  {
    Size  size = parent->getSize();
    WorkingBuildingPtr building = tile.getOverlay().as<WorkingBuilding>();
    if( building.isValid() )
    {
      InfoBoxWorkingBuilding* infoBox = new InfoBoxWorkingBuilding( parent, building );
      infoBox->setPosition( Point( (size.getWidth() - infoBox->getWidth()) / 2, size.getHeight() - infoBox->getHeight()) );

      infoBox->setTitle( title );
      infoBox->setText( text );
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

  gui::InfoBoxSimple* create( gui::Widget* parent, const Tile& tile )
  {
    Size  size = parent->getSize();
    InfoBoxSimple* infoBox = new InfoBoxSimple( parent, Rect( 0, 0, 510, 300 ) );
    infoBox->setPosition( Point( (size.getWidth() - infoBox->getWidth()) / 2, 
                                  size.getHeight() - infoBox->getHeight()) );

    infoBox->setTitle( title );
    infoBox->setText( text );
    return infoBox;
  }

  std::string title, text;
};

class InfoBoxManager::Impl
{
public:
    GuiEnv* gui;
    CityPtr city;
    bool showDebugInfo;

    typedef std::map< TileOverlay::Type, InfoboxCreator* > InfoboxCreators;
    std::map< std::string, TileOverlay::Type > name2typeMap;

    InfoboxCreators constructors;
};

InfoBoxManagerPtr InfoBoxManager::create( CityPtr city, GuiEnv* gui )
{
  InfoBoxManagerPtr ret( new InfoBoxManager( city, gui ) );
  ret->drop();

  return ret;
}

InfoBoxManager::InfoBoxManager( CityPtr city, GuiEnv* gui ) : _d( new Impl )
{
  _d->city = city;
  _d->showDebugInfo = true;
  _d->gui = gui;

  addInfobox( construction::road,             OC3_STR_EXT(B_ROAD), new CitizenInfoboxCreator<InfoBoxLand>( _d->city ) );
  addInfobox( building::B_RESERVOIR,        OC3_STR_EXT(B_RESERVOIR), new InfoBoxBasicCreator( _("##reservoir_title##"), _("##reservoir_text##") ) );
  addInfobox( building::house,            OC3_STR_EXT(B_HOUSE), new InfoBoxHouseCreator() );
  addInfobox( building::prefecture,       OC3_STR_EXT(B_PREFECTURE), new ServiceBaseInfoboxCreator( "##prefecture_title##", "##prefecture_text##") );
  addInfobox( building::engineerPost,    OC3_STR_EXT(B_ENGINEER_POST), new ServiceBaseInfoboxCreator( "##engineering_post_title##", "##engineering_post_text##" ) );
  addInfobox( building::B_WELL,             OC3_STR_EXT(B_WELL), new ServiceBaseInfoboxCreator( "##well_title##", "##well_text##" ) );
  addInfobox( building::B_DOCTOR,           OC3_STR_EXT(B_DOCTOR), new ServiceBaseInfoboxCreator( "##doctor_title##", "##doctor_text##" ) );
  addInfobox( building::B_BATHS,            OC3_STR_EXT(B_BATHS), new ServiceBaseInfoboxCreator( "##baths_title##", "##baths_text##" ) );
  addInfobox( building::B_BARBER,           OC3_STR_EXT(B_BARBER), new ServiceBaseInfoboxCreator( "##barber_title##", "##barber_text##" ) );
  addInfobox( building::B_HOSPITAL,         OC3_STR_EXT(B_HOSPITAL), new ServiceBaseInfoboxCreator( "##hospital_title##", "##hospital_text##" ) );
  addInfobox( building::B_FOUNTAIN,         OC3_STR_EXT(B_FOUNTAIN), new BaseInfoboxCreator<InfoBoxFontain>() );
  addInfobox( building::B_AQUEDUCT,         OC3_STR_EXT(B_AQUEDUCT), new InfoBoxBasicCreator( "##aqueduct_title##", "##aqueduct_text##") );
  addInfobox( building::market,           OC3_STR_EXT(B_MARKET), new BaseInfoboxCreator<InfoBoxMarket>() );
  addInfobox( building::granary,          OC3_STR_EXT(B_GRANARY), new BaseInfoboxCreator<InfoBoxGranary>() );
  addInfobox( building::grapeFarm,       OC3_STR_EXT(B_GRAPE_FARM), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::wheatFarm,       OC3_STR_EXT(B_WHEAT_FARM), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::B_VEGETABLE_FARM,   OC3_STR_EXT(B_VEGETABLE_FARM), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::B_OLIVE_FARM,       OC3_STR_EXT(B_OLIVE_FARM), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::B_FRUIT_FARM,       OC3_STR_EXT(B_FRUIT_FARM), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::warehouse,        OC3_STR_EXT(B_WAREHOUSE), new BaseInfoboxCreator<InfoBoxWarehouse>() );
  addInfobox( building::B_PIG_FARM,         OC3_STR_EXT(B_PIG_FARM), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::templeCeres,     OC3_STR_EXT(B_TEMPLE_CERES), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_TEMPLE_MARS,      OC3_STR_EXT(B_TEMPLE_MARS), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_TEMPLE_NEPTUNE,   OC3_STR_EXT(B_TEMPLE_NEPTUNE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_TEMPLE_VENUS,     OC3_STR_EXT(B_TEMPLE_VENUS), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_TEMPLE_MERCURE,   OC3_STR_EXT(B_TEMPLE_MERCURE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_BIG_TEMPLE_CERES, OC3_STR_EXT(B_BIG_TEMPLE_CERES), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_BIG_TEMPLE_MARS,  OC3_STR_EXT(B_BIG_TEMPLE_MARS), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_BIG_TEMPLE_NEPTUNE, OC3_STR_EXT(B_BIG_TEMPLE_NEPTUNE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_BIG_TEMPLE_VENUS,   OC3_STR_EXT(B_BIG_TEMPLE_VENUS), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_BIG_TEMPLE_MERCURE, OC3_STR_EXT(B_BIG_TEMPLE_MERCURE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_TEMPLE_ORACLE,    OC3_STR_EXT(B_TEMPLE_ORACLE), new BaseInfoboxCreator<InfoBoxTemple>() );
  addInfobox( building::B_SCHOOL,           OC3_STR_EXT(B_SCHOOL), new ServiceBaseInfoboxCreator( _("##school_title##"), _("##school_text##") ));
  addInfobox( building::B_COLLEGE,          OC3_STR_EXT(B_COLLEGE), new ServiceBaseInfoboxCreator( _("##college_title##"), _("##college_text##") ));
  addInfobox( building::B_LIBRARY,          OC3_STR_EXT(B_LIBRARY), new ServiceBaseInfoboxCreator( _("##library_title##"), _("##library_text##") ));
  addInfobox( construction::B_GARDEN,           OC3_STR_EXT(B_GARDEN), new InfoBoxBasicCreator( _("##building_garden##"), _("##garden_desc##")) );
  addInfobox( building::B_STATUE1,          OC3_STR_EXT(B_STATUE1), new InfoBoxBasicCreator( _("##building_statue_small##"), _("##statue_desc##")) );
  addInfobox( building::B_STATUE2,          OC3_STR_EXT(B_STATUE2), new InfoBoxBasicCreator( _("##building_statue_middle##"), _("##statue_desc##")) );
  addInfobox( building::B_STATUE3,          OC3_STR_EXT(B_STATUE3), new InfoBoxBasicCreator( _("##building_statue_big##"), _("##statue_desc##")) );
  addInfobox( construction::B_PLAZA,            OC3_STR_EXT(B_PLAZA), new CitizenInfoboxCreator<InfoBoxLand>( _d->city ) );
  addInfobox( building::unknown,            OC3_STR_EXT(unknown), new CitizenInfoboxCreator<InfoBoxLand>( _d->city ) );
  addInfobox( building::pottery,          OC3_STR_EXT(B_POTTERY), new BaseInfoboxCreator<GuiInfoFactory>() );
  addInfobox( building::B_WEAPONS_WORKSHOP, OC3_STR_EXT(B_WEAPONS_WORKSHOP), new BaseInfoboxCreator<GuiInfoFactory>() );
  addInfobox( building::B_FURNITURE,        OC3_STR_EXT(B_FURNITURE), new BaseInfoboxCreator<GuiInfoFactory>() );
  addInfobox( building::clayPit,         OC3_STR_EXT(B_CLAY_PIT), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::timberLogger,      OC3_STR_EXT(B_TIMBER_YARD), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::marbleQuarry,    OC3_STR_EXT(B_MARBLE_QUARRY), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::ironMine,        OC3_STR_EXT(B_IRON_MINE), new BaseInfoboxCreator<InfoBoxRawMaterial>() );
  addInfobox( building::B_WINE_WORKSHOP,    OC3_STR_EXT(B_WINE_WORKSHOP), new BaseInfoboxCreator<GuiInfoFactory>() );
  addInfobox( building::B_OIL_WORKSHOP,     OC3_STR_EXT(B_OIL_WORKSHOP), new BaseInfoboxCreator<GuiInfoFactory>() );
  addInfobox( building::senate,           OC3_STR_EXT(B_SENATE), new BaseInfoboxCreator<InfoBoxSenate>() );
  addInfobox( building::theater,          OC3_STR_EXT(buildingTheater), new ServiceBaseInfoboxCreator( _("##theater_title##"), _("##theater_text##")) );
  addInfobox( building::actorColony,     OC3_STR_EXT(B_ACTOR_COLONY), new ServiceBaseInfoboxCreator( _("##actor_colony_title##"), _("##actor_colony_text##")) );
  addInfobox( building::amphitheater, OC3_STR_EXT(buildingAmphitheater), new ServiceBaseInfoboxCreator( _("##amphitheater_title##"), _("##amphitheater_text##")) );
  addInfobox( building::gladiatorSchool, OC3_STR_EXT(B_GLADIATOR_SCHOOL), new ServiceBaseInfoboxCreator( _("##gladiator_school_title##"), _("##gladiator_school_text##")) );
  addInfobox( building::colloseum,       OC3_STR_EXT(B_COLLOSSEUM), new BaseInfoboxCreator<InfoBoxColosseum>() );
  addInfobox( building::lionHouse,       OC3_STR_EXT(B_LION_HOUSE), new ServiceBaseInfoboxCreator( _("##lion_house_title##"), _("##lion_house_text##")) );
  addInfobox( building::hippodrome,       OC3_STR_EXT(B_HIPPODROME), new ServiceBaseInfoboxCreator( _("##hippodrome_title##"), _("##hippodrome_text##")) );
  addInfobox( building::chariotSchool,    OC3_STR_EXT(chariotSchool), new ServiceBaseInfoboxCreator( _("##chario_maker_title##"), _("##chario_maker_text##")) );
  addInfobox( building::forum,            OC3_STR_EXT(forum), new ServiceBaseInfoboxCreator( _("##forum_title##"), _("##forum_text##")) );
  addInfobox( building::governorHouse,   OC3_STR_EXT(governorHouse), new ServiceBaseInfoboxCreator( _("##governor_house_title##"), _("##governonr_house_text##")) );
  addInfobox( building::governorVilla,   OC3_STR_EXT(governorVilla), new ServiceBaseInfoboxCreator( _("##governor_villa_title##"), _("##governonr_villa_text##")) );
  addInfobox( building::governorPalace,  OC3_STR_EXT(governorPalace), new ServiceBaseInfoboxCreator( _("##governor_palace_title##"), _("##governonr_palace_text##")) );
  addInfobox( building::highBridge,      OC3_STR_EXT(highBridge), new InfoBoxBasicCreator( _("##high_bridge_title##"), _("##high_bridge_text##")) );
  addInfobox( building::lowBridge,       OC3_STR_EXT(lowBridge), new InfoBoxBasicCreator( _("##low_bridge_title##"), _("##low_bridge_text##")) );
  addInfobox( building::wharf,            OC3_STR_EXT(wharf), new BaseInfoboxCreator<GuiInfoFactory>() );
}

InfoBoxManager::~InfoBoxManager()
{

}

void InfoBoxManager::showHelp( const Tile& tile )
{
  TileOverlayPtr overlay = tile.getOverlay();
  TileOverlay::Type type;

  if( _d->showDebugInfo )
  {
    Logger::warning( "Tile debug info: dsrbl=%d", tile.getDesirability() );
  }

  type = overlay.isNull() ? building::unknown : overlay->getType();

  Impl::InfoboxCreators::iterator findConstructor = _d->constructors.find( type );

  InfoBoxSimple* infoBox = findConstructor != _d->constructors.end()
                                  ? findConstructor->second->create( _d->gui->getRootWidget(), tile )
                                  : 0;
  
  if( infoBox && infoBox->isAutoPosition() )
  {
    Size rSize = _d->gui->getRootWidget()->getSize();
    int y = ( _d->gui->getCursorPos().getY() < rSize.getHeight() / 2 ) 
                ? rSize.getHeight() - infoBox->getHeight() - 5
                : 30;
    Point pos( ( rSize.getWidth() - infoBox->getWidth() ) / 2, y );

    infoBox->setPosition( pos );
  }
}

void InfoBoxManager::setShowDebugInfo( const bool showInfo )
{
  _d->showDebugInfo = showInfo;
} 

void InfoBoxManager::addInfobox( const TileOverlay::Type type, const std::string& typeName, InfoboxCreator* ctor )
{
  bool alreadyHaveConstructor = _d->name2typeMap.find( typeName ) != _d->name2typeMap.end();
  _OC3_DEBUG_BREAK_IF( alreadyHaveConstructor && "already have constructor for this type");

  if( !alreadyHaveConstructor )
  {
    _d->name2typeMap[ typeName ] = type;
    _d->constructors[ type ] = ctor;
  }
}

bool InfoBoxManager::canCreate(const TileOverlay::Type type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
