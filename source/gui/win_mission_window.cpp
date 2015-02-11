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

#include "win_mission_window.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "core/logger.hpp"
#include "core/gettext.hpp"
#include "widget_helper.hpp"
#include "core/event.hpp"
#include "widget_deleter.hpp"
#include "dialogbox.hpp"
#include "environment.hpp"

namespace gui
{

namespace dialog
{

class WinMission::Impl
{
public:
  GameAutoPause locker;

public signals:
  Signal0<> nextMissionSignal;
  Signal1<int> continueRulesSignal;
};

WinMission::WinMission(Widget* p, const std::string& newTitle, const std::string& winText, bool mayContinue )
  : Window( p, Rect( 0, 0, 540, 240 ), "" ), _d( new Impl )
{
  setupUI( ":/gui/winmission.gui" );

  Logger::warning( "dialog::WinMission show" );
  _d->locker.activate();

  setCenter( p->center() );

  Label* lbNewTitle;
  PushButton* btnContinue2years;
  PushButton* btnContinue5years;

  GET_WIDGET_FROM_UI( lbNewTitle )
  GET_WIDGET_FROM_UI( btnContinue2years )
  GET_WIDGET_FROM_UI( btnContinue5years )

  if( lbNewTitle ) lbNewTitle->setText( _( newTitle ) );

  if( !winText.empty() )
  {
    DialogBox::information( ui()->rootWidget(), "",  _(winText) );
  }

  if( btnContinue2years ) btnContinue2years->setVisible( mayContinue );
  if( btnContinue5years ) btnContinue5years->setVisible( mayContinue );
}

WinMission::~WinMission(){}

bool WinMission::onEvent(const NEvent &event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    switch( event.gui.caller->ID() )
    {
    case 0xff: emit _d->nextMissionSignal(); deleteLater(); break;
    default: emit _d->continueRulesSignal( event.gui.caller->ID()); deleteLater(); break;
    }
  }

  return Window::onEvent( event );
}

Signal0<>& WinMission::onAcceptAssign(){  return _d->nextMissionSignal; }
Signal1<int>& WinMission::onContinueRules(){  return _d->continueRulesSignal; }

}//end namespace dialog

}//end namespace gui
