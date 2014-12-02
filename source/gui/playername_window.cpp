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

#include "playername_window.hpp"
#include "editbox.hpp"
#include "pushbutton.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "widgetescapecloser.hpp"
#include "widget_helper.hpp"

namespace gui
{

namespace dialog
{

class ChangePlayerName::Impl
{
public signals:
  Signal1<std::string> onNameChangeSignal;
  Signal0<> onCloseSignal;
};

ChangePlayerName::ChangePlayerName(Widget* parent)
  : Window( parent, Rect( 0, 0, 10, 10 ), "", -1 ), _d( new Impl )
{
  Widget::setupUI( ":/gui/playername.gui" );

  WidgetEscapeCloser::insertTo( this );

  setCenter( parent->center() );

  EditBox* edPlayerName;
  PushButton* btnContinue;
  GET_WIDGET_FROM_UI( edPlayerName )
  GET_WIDGET_FROM_UI( btnContinue )

  CONNECT( edPlayerName, onTextChanged(), &_d->onNameChangeSignal, Signal1<std::string>::_emit );
  CONNECT( btnContinue, onClicked(), &_d->onCloseSignal, Signal0<>::_emit );
  CONNECT( edPlayerName, onEnterPressed(), &_d->onCloseSignal, Signal0<>::_emit );

  if( edPlayerName )
  {
    edPlayerName->moveCursor( edPlayerName->text().length() );
  }
}

ChangePlayerName::~ChangePlayerName(){}

std::string ChangePlayerName::text() const
{
  const EditBox* ed = findChildA<EditBox*>( "edPlayerName", true, this );
  return ed ? ed->text() : "";
}

void ChangePlayerName::setModal()
{
  Window::setModal();

  EditBox* edPlayerName;
  GET_WIDGET_FROM_UI( edPlayerName )

  if( edPlayerName ) edPlayerName->setFocus();
}

Signal0<>& ChangePlayerName::onClose(){  return _d->onCloseSignal;}
Signal1<std::string>& ChangePlayerName::onNameChange(){  return _d->onNameChangeSignal;}

}//end namespace dialog

}//end namespace gui
