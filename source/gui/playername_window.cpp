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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "playername_window.hpp"
#include "editbox.hpp"
#include "pushbutton.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "core/event.hpp"
#include "widget_helper.hpp"
#include "core/gettext.hpp"
#include "label.hpp"

namespace gui
{

namespace dialog
{

class ChangePlayerName::Impl
{
public:
  bool mayExit;
  EditBox* edPlayerName;
  Label* lbExitHelp;

  struct {
    Signal1<std::string> onNameChange;
    Signal0<> onClose;
    Signal0<> onNewGame;
  } signal;
};

ChangePlayerName::ChangePlayerName(Widget* parent)
  : Window( parent, Rect( 0, 0, 10, 10 ), "", -1 ), _d( new Impl )
{
  Widget::setupUI( ":/gui/playername.gui" );

  setCenter( parent->center() );

  GET_DWIDGET_FROM_UI( _d, edPlayerName )
  GET_DWIDGET_FROM_UI( _d, lbExitHelp)

  CONNECT( _d->edPlayerName, onTextChanged(), &_d->signal.onNameChange, Signal1<std::string>::_emit );
  CONNECT( _d->edPlayerName, onEnterPressed(), &_d->signal.onNewGame, Signal0<>::_emit );
  LINK_WIDGET_ACTION( PushButton*, btnContinue, onClicked(), &_d->signal.onNewGame, Signal0<>::_emit );

  if( _d->edPlayerName )
  {
    _d->edPlayerName->moveCursor( _d->edPlayerName->text().length() );
  }

  _d->mayExit = true;

  setModal();
}

bool ChangePlayerName::onEvent(const NEvent& event)
{
  if( event.EventType == sEventKeyboard && !event.keyboard.pressed && event.keyboard.key == KEY_ESCAPE )
  {
    if( _d->mayExit )
    {
      deleteLater();
      emit _d->signal.onClose();

      return true;
    }
  }

  return Window::onEvent( event );
}

void ChangePlayerName::setMayExit(bool value)
{
  _d->mayExit = value;
  if( _d->lbExitHelp )
    _d->lbExitHelp->setVisible( value );
}

ChangePlayerName::~ChangePlayerName(){}

std::string ChangePlayerName::name() const
{
  return _d->edPlayerName ? _d->edPlayerName->text() : "";
}

void ChangePlayerName::setName(const std::string& text)
{
  std::string correctName = text.empty() ? _("##new_governor##") : text;
  if( _d->edPlayerName ) _d->edPlayerName->setText( correctName );
}

void ChangePlayerName::setModal()
{
  Window::setModal();

  if( _d->edPlayerName )
    _d->edPlayerName->setFocus();
}

Signal0<>& ChangePlayerName::onClose(){  return _d->signal.onClose;}
Signal0<>& ChangePlayerName::onContinue(){  return _d->signal.onNewGame;}
Signal1<std::string>& ChangePlayerName::onNameChange(){  return _d->signal.onNameChange;}

}//end namespace dialog

}//end namespace gui
