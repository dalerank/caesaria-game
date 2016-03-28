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
#include "core/gettext.hpp"
#include "events/showtileinfo.hpp"
#include "events/playsound.hpp"

using namespace events;

namespace gui
{

namespace infobox
{

AboutConstruction::AboutConstruction( Widget* parent, Rect rect, Rect blackArea )
  : Infobox( parent, rect, blackArea )
{
  setupUI( ":/gui/infoboxconstr.gui" );
  _btnToggleWorking = nullptr;
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

  return Infobox::onEvent( event );
}

PushButton* AboutConstruction::_buttonToggleWorking() { return _btnToggleWorking; }

void AboutConstruction::_setWorkingVisible(bool show)
{
  if( !_btnToggleWorking && _lbBlackFrame() )
  {
     Rect btnRect( Point( _lbBlackFrame()->width() - 110, (_lbBlackFrame()->height() - 25)/2 ), Size( 100, 25 ) );
     _btnToggleWorking = &_lbBlackFrame()->add<PushButton>( btnRect, "", -1, false, PushButton::blackBorderUp  );
     _btnToggleWorking->setFont( "FONT_1" );
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
    _btnToggleWorking->setText( working ? _("##abwrk_working##") : _("##abwrk_not_working##") );
}

void AboutConstruction::_updateWorkingText()
{
  auto workingBuilding = base().as<WorkingBuilding>();
  _setWorkingActive( workingBuilding.isValid() ? workingBuilding->isActive() : false );
}

void AboutConstruction::_resolveToggleWorking()
{
  auto workingBuilding = base().as<WorkingBuilding>();
  if( workingBuilding.isValid() )
  {
    workingBuilding->setActive( !workingBuilding->isActive() );
    _setWorkingActive( workingBuilding->isActive() );
  }
}

void AboutConstruction::_baseAssigned()
{
  if( base().isValid() )
  {
    events::dispatch<PlaySound>( "bmsel_" + base()->info().typeName(), 1, 100, audio::infobox, true );
  }
}

ConstructionPtr AboutConstruction::base() const { return _construction; }

void AboutConstruction::_switch(int flag)
{
  if( _construction.isValid() )
  {
    events::dispatch<ShowTileInfo>( base()->pos(), flag == KEY_PERIOD
                                    ? ShowTileInfo::next
                                    : ShowTileInfo::prew );
    deleteLater();
  }
}

}//end namespace infobox

}//end namespace gui
