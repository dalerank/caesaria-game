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
#include "sound/engine.hpp"
#include "game/scripting.hpp"

namespace gui
{

namespace dialog
{

class WinMission::Impl
{
public:
  audio::Muter muter;
  audio::SampleDeleter speechDel;

  struct {
    Signal0<> nextMission;
    Signal1<int> continueRules;
  } signal;
};

WinMission::WinMission( Widget* parent, const std::string& newTitle,
                        const std::string& winText,
                        const std::string& speech, bool mayContinue )
  : Window( parent, Rect( 0, 0, 540, 240 ), "" ), _d( new Impl )
{
  game::Scripting::doFile( ":/gui/winmission.js" );
  //setupUI( ":/gui/winmission.gui" );

  Logger::warning( "dialog::WinMission show" );

  INIT_WIDGET_FROM_UI(Label*, lbNewTitle )
  INIT_WIDGET_FROM_UI(PushButton*, btnContinue2years )
  INIT_WIDGET_FROM_UI(PushButton*, btnContinue5years )

  if( lbNewTitle ) lbNewTitle->setText( _( newTitle ) );
  if( !winText.empty() )  dialog::Information( ui(), "",  _(winText) );
  if( btnContinue2years ) btnContinue2years->setVisible( mayContinue );
  if( btnContinue5years ) btnContinue5years->setVisible( mayContinue );

  if( !speech.empty() )
  {
    _d->muter.activate(5);
    _d->speechDel.assign( speech );
    audio::Engine::instance().play( speech, 100, audio::speech );
  }

  GameAutoPause::insertTo( this );
  moveTo( Widget::parentCenter );
}

WinMission::~WinMission(){}
Signal0<>& WinMission::onAcceptAssign()    { return _d->signal.nextMission;   }
Signal1<int>& WinMission::onContinueRules(){ return _d->signal.continueRules; }

bool WinMission::_onButtonClicked(Widget* sender)
{
  switch( sender->ID() )
  {
  case 0xff: emit _d->signal.nextMission(); deleteLater(); break;
  default: emit _d->signal.continueRules( sender->ID()); deleteLater(); break;
  }

  return true;
}

}//end namespace dialog

}//end namespace gui
