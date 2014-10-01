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
#include "widget_deleter.hpp"

namespace gui
{

class WinMissionWindow::Impl
{
public:
  GameAutoPause locker;

public signals:
  Signal0<> onNextMissionSignal;
  Signal1<int> onContinueRulesSignal;
};

WinMissionWindow::WinMissionWindow(Widget* p, const std::string& newTitle, const std::string& winText, bool mayContinue )
  : Window( p, Rect( 0, 0, 540, 240 ), "" ), _d( new Impl )
{
  setupUI( ":/gui/winmission.gui" );

  Logger::warning( "WinMissionWindow: show" );
  _d->locker.activate();

  setCenter( p->center() );

  Label* lbNewTitle;
  GET_WIDGET_FROM_UI( lbNewTitle )
  if( lbNewTitle ) lbNewTitle->setText( _( newTitle ) );

  PushButton* btnAccept;
  GET_WIDGET_FROM_UI( btnAccept )

  CONNECT( btnAccept, onClicked(), &_d->onNextMissionSignal, Signal0<>::_emit );
  CONNECT( btnAccept, onClicked(), this, WinMissionWindow::deleteLater );

  if( !winText.empty() )
  {
    Label* lbWin = new Label( this, Rect( 30, 30, width()-30, height()-30 ), _(winText), false, Label::bgWhiteFrame );
    lbWin->setTextAlignment( align::center, align::center );
    lbWin->setWordwrap( true );
    WidgetDeleter::assignTo( lbWin, 3000 );
  }
}

WinMissionWindow::~WinMissionWindow(){}
Signal0<>& WinMissionWindow::onAcceptAssign(){  return _d->onNextMissionSignal; }
Signal1<int>& WinMissionWindow::onContinueRules(){  return _d->onContinueRulesSignal; }

}//end namespace gui
