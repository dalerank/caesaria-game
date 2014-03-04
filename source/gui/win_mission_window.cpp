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

#include "win_mission_window.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "core/logger.hpp"
#include "core/gettext.hpp"

namespace gui
{

class WinMissionWindow::Impl
{
public:
  GameAutoPause locker;

public oc3_signals:
  Signal0<> onNextMissionSignal;
  Signal1<int> onContinueRulesSignal;
};

WinMissionWindow::WinMissionWindow(Widget* parent, std::string newTitle, bool mayContinue )
  : Widget( parent, -1, Rect( 0, 0, 540, 240 )), _d( new Impl )
{
  Logger::warning( "WinMissionWindow: show" );
  _d->locker.activate();

  Size pSize = getParent()->getSize() - getSize();
  setPosition( Point( pSize.width() / 2, pSize.height() / 2 ) );

  new Label( this, Rect( 0, 0, width(), height()), "", false, Label::bgWhiteFrame );

  Label* title = new Label( this, Rect( 10, 10, width() - 10, 10 + 30), _("##mission_win##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  Label* lb = new Label( this, Rect( 10, 40, width() - 10, 40 + 20), _("##caesar_assign_new_title##") );
  lb->setFont( Font::create( FONT_2 ) );
  lb->setTextAlignment( alignCenter, alignCenter );

  lb = new Label( this, Rect( 10, 60, width() - 10, 60 + 30), _( newTitle ) );
  lb->setFont( Font::create( FONT_3) );
  lb->setTextAlignment( alignCenter, alignCenter );

  PushButton* btn = new PushButton( this, Rect( 35, 115, width() - 35, 115 + 20 ), _("##accept_promotion##"),
                                    -1, false, PushButton::smallGrayBorderLine );

  CONNECT( btn, onClicked(), &_d->onNextMissionSignal, Signal0<>::emit );
  CONNECT( btn, onClicked(), this, WinMissionWindow::deleteLater );
}

WinMissionWindow::~WinMissionWindow(){}
Signal0<>& WinMissionWindow::onAcceptAssign(){  return _d->onNextMissionSignal; }
Signal1<int>& WinMissionWindow::onContinueRules(){  return _d->onContinueRulesSignal; }

}//end namespace gui
