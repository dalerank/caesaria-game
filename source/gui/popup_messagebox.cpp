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

#include "popup_messagebox.hpp"
#include "gfx/picture.hpp"
#include "pushbutton.hpp"
#include "listbox.hpp"
#include "editbox.hpp"
#include "core/gettext.hpp"
#include "label.hpp"
#include "gfx/decorator.hpp"
#include "gfx/engine.hpp"
#include "texturedbutton.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"
#include "game/datetimehelper.hpp"

using namespace gfx;

namespace gui
{

class PopupMessageBox::Impl
{
public:
  PushButton* btnExit;
  PushButton* btnHelp; 
  Label* lbText;
};

PopupMessageBox::PopupMessageBox( Widget* parent, const std::string& title, 
                                  const std::string& text,
                                  const DateTime& time,
                                  const std::string& receiver, int id )
  : Window( parent, Rect( 0, 0, 590, 320  ), "", id ), _d( new Impl )
{
  setupUI( ":/gui/popupmessage.bui" );
  setCenter( parent->center() );
  
  Label* lbTitle = findChildA<Label*>( "lbTitle", true, this );
  lbTitle->setText( title );

  _d->btnExit = findChildA<PushButton*>( "btnExit", true, this );
  CONNECT( _d->btnExit, onClicked(), this, PopupMessageBox::deleteLater );

  _d->btnHelp = findChildA<PushButton*>( "btnHelp", true, this );

  Label* lbTime = findChildA<Label*>( "lbTime", true, this );
  lbTime->setText( DateTimeHelper::toStr( time ) );

  Label* lbReceiver = findChildA<Label*>( "lbReceiver", true, this );
  lbReceiver->setText( receiver );

  _d->lbText = findChildA<Label*>( "lbText", true, this );
}

void PopupMessageBox::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

PopupMessageBox* PopupMessageBox::information(Widget* parent, const std::string& title, const std::string& text, const DateTime& time)
{
  return new PopupMessageBox( parent, title, text, time );
}

}//end namespace gui
