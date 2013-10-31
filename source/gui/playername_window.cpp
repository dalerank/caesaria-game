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

#include "playername_window.hpp"
#include "game/settings.hpp"
#include "editbox.hpp"
#include "pushbutton.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"

namespace gui
{

class WindowPlayerName::Impl
{
public oc3_signals:
  Signal1<std::string> onNameChangeSignal;
  Signal0<> onCloseSignal;
};

WindowPlayerName::WindowPlayerName(Widget* parent)
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setupUI( GameSettings::rcpath( "/gui/playername.gui" ) );

  Point offset( getWidth(), getHeight() );
  setPosition( parent->getRelativeRect().getCenter() - offset / 2 );

  const bool searchRecursive = true;
  EditBox* ed = findChild<EditBox*>( "edPlayerName", searchRecursive );
  CONNECT( ed, onTextChanged(), &_d->onNameChangeSignal, Signal1<std::string>::emit );

  PushButton* btn = findChild<PushButton*>( "btnContinue", searchRecursive );
  CONNECT( btn, onClicked(), &_d->onCloseSignal, Signal0<>::emit );
}

WindowPlayerName::~WindowPlayerName()
{

}

Signal0<>& WindowPlayerName::onClose()
{
  return _d->onCloseSignal;
}

Signal1<std::string>& WindowPlayerName::onNameChange()
{
  return _d->onNameChangeSignal;
}

}//end namespace gui
