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

#include "oc3_infoboxmanager.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_guienv.hpp"
#include "oc3_road.hpp"
#include "oc3_prefecture.hpp"
#include "oc3_tile.hpp"
#include "oc3_service_building.hpp"
#include "oc3_buildingengineer.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_house.hpp"
#include "oc3_gettext.hpp"

class InfoBoxHouseCreator : public InfoboxCreator
{
public:
  GuiInfoBox* create( Widget* parent, const Tile& tile )
  {
    HousePtr house = tile.getTerrain().getOverlay().as<House>();
    if( house->getNbHabitants() > 0 )
    {
      return new InfoBoxHouse( parent, tile );
    }
    else
    {
      return new InfoBoxFreeHouse( parent, tile );
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

  GuiInfoBox* create( Widget* parent, const Tile& tile )
  {
    Size  size = parent->getSize();
    GuiInfoService* infoBox = new GuiInfoService( parent, tile.getTerrain().getOverlay().as<ServiceBuilding>() );
    infoBox->setPosition( Point( (size.getWidth() - infoBox->getWidth()) / 2, 
                                  size.getHeight() - infoBox->getHeight()) );

    infoBox->setTitle( title );
    infoBox->setText( text );
    return infoBox;
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

  GuiInfoBox* create( Widget* parent, const Tile& tile )
  {
    Size  size = parent->getSize();
    InfoBoxBasic* infoBox = new InfoBoxBasic( parent, tile );
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
    bool showDebugInfo;

    typedef std::map< BuildingType, InfoboxCreator* > InfoboxCreators;
    std::map< std::string, BuildingType > name2typeMap;
    InfoboxCreators constructors;
};

InfoBoxManagerPtr InfoBoxManager::create( GuiEnv* gui )
{
    InfoBoxManagerPtr ret( new InfoBoxManager() );
    ret->_d->gui = gui;
    ret->_d->showDebugInfo = true;

    return ret;
}

InfoBoxManager::InfoBoxManager() : _d( new Impl )
{
  addCreator( B_ROAD, OC3_STR_EXT(B_ROAD), new BaseInfoboxCreator<InfoBoxLand>() );
  addCreator( B_HOUSE, OC3_STR_EXT(B_HOUSE), new InfoBoxHouseCreator() );
  addCreator( B_PREFECT, OC3_STR_EXT(B_PREFECT), new ServiceBaseInfoboxCreator( "##prefecture_title##", "##prefecture_text##") );
  addCreator( B_ENGINEER, OC3_STR_EXT(B_ENGINEER), new ServiceBaseInfoboxCreator( "##engineering_post_title##", "##engineering_post_text##" ) ); 
  addCreator( B_WELL, OC3_STR_EXT(B_WELL), new ServiceBaseInfoboxCreator( "##well_title##", "##well_text##" ) );
  addCreator( B_FOUNTAIN, OC3_STR_EXT(B_FOUNTAIN), new ServiceBaseInfoboxCreator( "##fontaun_title##", "##fontaun_text##" ) );
  addCreator( B_MARKET, OC3_STR_EXT(B_MARKET), new BaseInfoboxCreator<GuiInfoMarket>() );
  addCreator( B_GRANARY, OC3_STR_EXT(B_GRANARY), new BaseInfoboxCreator<GuiInfoGranary>() );
  addCreator( B_GRAPE, OC3_STR_EXT(B_GRAPE), new BaseInfoboxCreator<InfoBoxFarm>() );
  addCreator( B_WHEAT_FARM, OC3_STR_EXT(B_WHEAT_FARM), new BaseInfoboxCreator<InfoBoxFarm>() );
  addCreator( B_OLIVE, OC3_STR_EXT(B_OLIVE), new BaseInfoboxCreator<InfoBoxFarm>() );
  addCreator( B_FRUIT, OC3_STR_EXT(B_FRUIT), new BaseInfoboxCreator<InfoBoxFarm>() );
  addCreator( B_WAREHOUSE, OC3_STR_EXT(B_WAREHOUSE), new BaseInfoboxCreator<InfoBoxWarehouse>() );
  addCreator( B_IRON_MINE, OC3_STR_EXT(B_IRON_MINE), new BaseInfoboxCreator<GuiInfoFactory>() );
  addCreator( B_MEAT, OC3_STR_EXT(B_MEAT), new BaseInfoboxCreator<InfoBoxFarm>() );
  addCreator( B_VEGETABLE, OC3_STR_EXT(B_VEGETABLE), new BaseInfoboxCreator<InfoBoxFarm>() );
  addCreator( B_TEMPLE_CERES, OC3_STR_EXT(B_TEMPLE_CERES), new BaseInfoboxCreator<InfoBoxTemple>() );
  addCreator( B_GARDEN, OC3_STR_EXT(B_GARDEN), new InfoBoxBasicCreator( _("building_garden"), _("##garden_desc##")) );
  addCreator( B_STATUE1, OC3_STR_EXT(B_STATUE1), new InfoBoxBasicCreator( _("building_statue_small"), _("##statue_desc##")) );
  addCreator( B_STATUE2, OC3_STR_EXT(B_STATUE2), new InfoBoxBasicCreator( _("building_statue_middle"), _("##statue_desc##")) );
  addCreator( B_STATUE3, OC3_STR_EXT(B_STATUE3), new InfoBoxBasicCreator( _("building_statue_big"), _("##statue_desc##")) );
  addCreator( B_PLAZA, OC3_STR_EXT(B_PLAZA), new BaseInfoboxCreator<InfoBoxLand>() );
  addCreator( B_NONE, OC3_STR_EXT(B_NONE), new BaseInfoboxCreator<InfoBoxLand>() );
}

InfoBoxManager::~InfoBoxManager()
{

}

void InfoBoxManager::showHelp( const Tile& tile )
{
  LandOverlayPtr overlay = tile.getTerrain().getOverlay();
  BuildingType type;

  if( _d->showDebugInfo )
  {
    StringHelper::debug( 0xff, "Tile debug info: dsrbl=%d", tile.getTerrain().getDesirability() ); 
  }

  type = overlay.isNull() ? B_NONE : overlay->getType();

  Impl::InfoboxCreators::iterator findConstructor = _d->constructors.find( type );

  GuiInfoBox* infoBox = findConstructor != _d->constructors.end() 
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

void InfoBoxManager::addCreator( const BuildingType type, const std::string& typeName, InfoboxCreator* ctor )
{
  bool alreadyHaveConstructor = _d->name2typeMap.find( typeName ) != _d->name2typeMap.end();
  _OC3_DEBUG_BREAK_IF( alreadyHaveConstructor && "already have constructor for this type");

  if( !alreadyHaveConstructor )
  {
    _d->name2typeMap[ typeName ] = type;
    _d->constructors[ type ] = ctor;
  }
}

bool InfoBoxManager::canCreate( const BuildingType type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
