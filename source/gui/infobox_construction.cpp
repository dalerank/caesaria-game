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

#include "infobox_construction.hpp"
#include "core/event.hpp"
#include "game/settings.hpp"
#include "label.hpp"
#include "core/logger.hpp"
#include "events/showtileinfo.hpp"

using namespace constants;

namespace gui
{

namespace infobox
{

AboutConstruction::AboutConstruction( Widget* parent, Rect rect, Rect blackArea )
  : Simple( parent, rect, blackArea )
{
  setupUI( ":/gui/infoboxconstr.gui" );
  _btnToggleWorking = 0;
}

AboutConstruction::~AboutConstruction() {}

bool AboutConstruction::onEvent(const NEvent& event)
{
  switch( event.EventType )
  {
  case sEventKeyboard:
    if( event.keyboard.key == KEY_COMMA || event.keyboard.key == KEY_PERIOD )
    {
      _switch( event.keyboard.key );
    }
  break;

  case sEventGui:
    if( event.gui.type == guiButtonClicked && ( event.gui.caller->ID() == KEY_COMMA || event.gui.caller->ID() == KEY_PERIOD ) )
    {
      _switch(event.gui.caller->ID());
    }
  break;

  default: break;
  }

  return Simple::onEvent( event );
}

PushButton* AboutConstruction::_btnToggleWorkingRef() { return _btnToggleWorking; }

void AboutConstruction::_setWorkingVisible(bool show)
{
  if( !_btnToggleWorking && _lbBlackFrameRef() )
  {
     _btnToggleWorking = new PushButton( _lbBlackFrameRef(), Rect( 0, 0, 100, 25 ), "", -1, false, PushButton::blackBorderUp  );
     _btnToggleWorking->setFont( Font::create( FONT_1 ) );
     _btnToggleWorking->setPosition( Point( _lbBlackFrameRef()->width() - 110, (_lbBlackFrameRef()->height() - 25)/2 ) );
     _updateWorkingText();

     CONNECT( _btnToggleWorking, onClicked(), this, AboutConstruction::_resolveToggleWorking );
  }

  if( _btnToggleWorking )
  {
    _btnToggleWorking->setVisible( show );
  }
}

void AboutConstruction::_setWorkingActive(bool working)
{
  if( _btnToggleWorking )
    _btnToggleWorking->setText( working ? "Working" : "Not working");
}

void AboutConstruction::_updateWorkingText()
{
  WorkingBuildingPtr working = ptr_cast<WorkingBuilding>( base() );
  _setWorkingActive( working.isValid() ? working->isActive() : false );
}

void AboutConstruction::_resolveToggleWorking()
{
  WorkingBuildingPtr working = ptr_cast<WorkingBuilding>( base() );
  if( working.isValid() )
  {
    working->setActive( !working->isActive() );
    _setWorkingActive( working->isActive() );
  }
}

ConstructionPtr AboutConstruction::base() const { return _construction; }
void AboutConstruction::setBase(ConstructionPtr construction) { _construction = construction; }

void AboutConstruction::_switch(int flag)
{
  if( _construction.isValid() )
  {
    events::GameEventPtr e = events::ShowTileInfo::create( base()->pos(), flag == KEY_PERIOD
                                                           ? events::ShowTileInfo::next
                                                           : events::ShowTileInfo::prew );
    deleteLater();
    e->dispatch();
  }
}

}

}//end namespace gui
