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
// Copyright 2012-2015 dalerank, dalerankn8@gmail.com

#include "infoboxmanager.hpp"
#include "gui/info_box.hpp"
#include "gui/environment.hpp"
#include "gfx/tile.hpp"
#include "objects/service.hpp"
#include "gui/infobox_land.hpp"
#include "core/utils.hpp"
#include "objects/house.hpp"
#include "core/gettext.hpp"
#include "city/helper.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "walker/walker.hpp"
#include "gfx/tilemap.hpp"
#include "gui/infobox_house.hpp"
#include "objects/ruins.hpp"
#include "gui/infobox_citizen_mgr.hpp"
#include "gui/infobox_working.hpp"
#include "game/settings.hpp"
#include <map>

using namespace constants;
using namespace gui;
using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_STATICINFOBOX_IN_FACTORY(elevation,object::elevation,"", "##elevation_info##" )
REGISTER_STATICINFOBOX_IN_FACTORY(aqueduct,object::aqueduct,"", "##aqueduct_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(garden,object::garden,"", "##garden_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(stsmall,object::statue_small,"", "##statue_small_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(stmiddle,object::statue_middle,"", "##statue_middle_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(stbig,object::statue_big,"", "##statue_big_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(native_hut,object::native_hut,"", "##nativeHut_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(gatehouse,object::gatehouse,"", "##gatehouse_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(native_field,object::native_field,"", "##nativeField_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(native_center,object::native_center,"", "##nativeCenter_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(hbrifge,object::high_bridge,"", "##high_bridge_info##")
REGISTER_STATICINFOBOX_IN_FACTORY(lbridge,object::low_bridge,"", "##bridge_extends_city_area##")
REGISTER_STATICINFOBOX_IN_FACTORY(burning_ruins,object::burning_ruins,"", "##this_fire_can_spread##" )
REGISTER_STATICINFOBOX_IN_FACTORY(rift,object::rift,"", "##these_rift_info##" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(prefecture,object::prefecture,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(engineering_post,object::engineering_post,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(clinic, object::clinic,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(baths, object::baths,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(barber, object::barber,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(hospital, object::hospital,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(missionaryPost, object::missionaryPost, "", "")
REGISTER_SERVICEINFOBOX_IN_FACTORY(school, object::school,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(academy, object::academy,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(library, object::library,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(dock,object::dock,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY(actorcolony,object::actorColony,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY( gladiatorSchool, object::gladiatorSchool,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY( lionsNursery, object::lionsNursery,"", "")
REGISTER_SERVICEINFOBOX_IN_FACTORY( hippodrome, object::hippodrome,"", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY( chariotSchool, object::chariotSchool, "", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY( forum, object::forum, "", "" )
REGISTER_SERVICEINFOBOX_IN_FACTORY( governorHouse, object::governorHouse, "", "##governor_house_text##")
REGISTER_SERVICEINFOBOX_IN_FACTORY( governorVilla, object::governorVilla, "", "##governor_villa_text##")
REGISTER_SERVICEINFOBOX_IN_FACTORY( governorPalace, object::governorPalace,  "", "##governor_palace_text##")

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

class Manager::Impl
{
public:
  bool showDebugInfo;

  typedef std::map< object::Type, InfoboxCreator* > InfoboxCreators;
  std::map< std::string, object::Type > name2typeMap;

  InfoboxCreators constructors;
};

Manager::Manager() : _d( new Impl )
{
  _d->showDebugInfo = true;

  citizen::PManager::instance().loadInfoboxes( *this );

#define ADD_INFOBOX(typen, creator) addInfobox(typen, CAESARIA_STR_EXT(typen), new creator);
  ADD_INFOBOX( object::house,        InfoboxHouseCreator() )

}

Manager::~Manager() {}

Manager& Manager::instance()
{
  static Manager inst;
  return inst;
}

void Manager::showHelp( PlayerCityPtr city, Ui* gui, TilePos pos )
{
  Tile& tile = city->tilemap().at( pos );
  OverlayPtr overlay = tile.overlay();
  object::Type type;

  if( _d->showDebugInfo )
  {
    Logger::warning( "Tile debug info: dsrbl=%d", tile.param( Tile::pDesirability ) );
  }

  type = overlay.isNull() ? object::unknown : overlay->type();

  Impl::InfoboxCreators::iterator findConstructor = _d->constructors.find( type );

  Simple* infoBox = findConstructor != _d->constructors.end()
                                  ? findConstructor->second->create( city, gui->rootWidget(), pos )
                                  : 0;
  
  if( infoBox && infoBox->isAutoPosition() )
  {
    bool lockWindow = SETTINGS_VALUE( lockInfobox );

    Size rSize = gui->rootWidget()->size();
    int y = ( gui->cursorPos().y() < rSize.height() / 2 )
                ? rSize.height() - infoBox->height() - 5
                : 30;
    Point pos( ( rSize.width() - infoBox->width() ) / 2, y );

    infoBox->setPosition( pos );
    infoBox->setFocus();
    infoBox->setWindowFlag( Window::fdraggable, !lockWindow);
  }
}

void Manager::setShowDebugInfo( const bool showInfo ) {  _d->showDebugInfo = showInfo; }

void Manager::addInfobox( const object::Type type, const std::string& typeName, InfoboxCreator* ctor )
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

bool Manager::canCreate(const object::Type type) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}

StaticInfoboxCreator::StaticInfoboxCreator(const std::string &caption, const std::string &desc):
  title( caption ), text( desc )
{}

Simple *StaticInfoboxCreator::create(PlayerCityPtr city, Widget *parent, TilePos pos)
{
  Size  size = parent->size();
  Simple* infoBox = new Simple( parent, Rect( 0, 0, 510, 300 ) );
  infoBox->setPosition( Point( (size.width() - infoBox->width()) / 2,
                               size.height() - infoBox->height()) );
  OverlayPtr overlay = city->getOverlay( pos );

  std::string caption = overlay.isValid()
      ? MetaDataHolder::findPrettyName( overlay->type() )
      : title;

  infoBox->setTitle( _( caption ) );
  infoBox->setText( _( text ) );
  return infoBox;
}

ServiceInfoboxCreator::ServiceInfoboxCreator(const std::string &caption, const std::string &descr, bool drawWorkers)
  : title( caption ), text( descr ), isDrawWorkers( drawWorkers )
{
}

Simple* ServiceInfoboxCreator::create(PlayerCityPtr city, Widget *parent, TilePos pos)
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

}//end namespace infobox

}//end namespave gui
