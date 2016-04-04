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
#include "core/utils.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "walker/walker.hpp"
#include "gfx/tilemap.hpp"
#include "events/script_event.hpp"
#include "gui/infobox_citizen_mgr.hpp"
#include "core/common.hpp"
#include <map>

using namespace gui;
using namespace gfx;

namespace gui
{

namespace infobox
{

class Manager::Impl
{
public:
  bool showDebugInfo;
  bool boxLocked;

  typedef std::map< object::Type, InfoboxCreator* > InfoboxCreators;
  std::map< std::string, object::Type > name2typeMap;

  InfoboxCreators constructors;
};

Manager::Manager() : _d( new Impl )
{
  _d->showDebugInfo = true;
  _d->boxLocked = true;

  citizen::PManager::instance().loadInfoboxes();
}

Manager::~Manager() {}

void Manager::showHelp( PlayerCityPtr city, Ui* gui, TilePos pos )
{
  Tile& tile = city->tilemap().at( pos );
  OverlayPtr overlay = tile.overlay();
  object::Type type;

  if( _d->showDebugInfo )
  {
    Logger::debug( "Tile debug info: dsrbl={}", tile.param( Tile::pDesirability ) );
  }

  type = object::typeOrDefault(overlay);

  Impl::InfoboxCreators::iterator findConstructor = _d->constructors.find( type );

  Infobox* infoBox = findConstructor != _d->constructors.end()
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
    infoBox->setWindowFlag( Window::fdraggable, !_d->boxLocked );
  }

  if (!infoBox)
  {
    VariantList vl; vl << pos;
    events::dispatch<events::ScriptFunc>("OnShowOverlayInfobox", vl);
  }
}

void Manager::setShowDebugInfo( const bool showInfo ) {  _d->showDebugInfo = showInfo; }

void Manager::addInfobox( const object::Type& type, InfoboxCreator* ctor )
{
  std::string name = object::toString( type );
  bool alreadyHaveConstructor = _d->name2typeMap.find( name ) != _d->name2typeMap.end();

  if (name == "unknown")
    Logger::debug( "InfoboxManager: added default infobox constructor" );

  if( !alreadyHaveConstructor )
  {
    _d->name2typeMap[ name ] = type;
    _d->constructors[ type ] = ctor;
  }
  else
  {
    Logger::debug("InfoboxManager: already have constructor for type " + name);
  }
}

bool Manager::canCreate(const object::Type type) const
{
  return _d->constructors.find( type ) != _d->constructors.end();
}

void infobox::Manager::setBoxLock(bool lock) { _d->boxLocked = lock; }


}//end namespace infobox

}//end namespave gui
