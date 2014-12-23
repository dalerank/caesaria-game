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
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "game/datetimehelper.hpp"
#include "widget_helper.hpp"

using namespace gfx;

namespace gui
{

namespace messagebox
{

class Popup::Impl
{
public:
  PushButton* btnExit;
  PushButton* btnHelp; 
  Label* lbText;
};

Popup::Popup( Widget* parent, const std::string& title,
                                  const std::string& text,
                                  const DateTime& time,
                                  const std::string& receiver, int id )
  : Window( parent, Rect( 0, 0, 590, 320  ), "", id ), _d( new Impl )
{
  setupUI( ":/gui/popupmessage.bui" );
  setCenter( parent->center() );
  
  Label* lbTitle;
  Label* lbTime;
  Label* lbReceiver;
  GET_WIDGET_FROM_UI( lbTitle )
  GET_DWIDGET_FROM_UI( _d, btnExit )
  GET_DWIDGET_FROM_UI( _d, btnHelp )
  GET_WIDGET_FROM_UI( lbTime )
  GET_WIDGET_FROM_UI( lbReceiver )
  GET_DWIDGET_FROM_UI( _d, lbText  )

  if( lbTitle ) lbTitle->setText( title );
  if( lbTime ) lbTime->setText( util::date2str( time ) );
  if( lbReceiver ) lbReceiver->setText( receiver );

  CONNECT( _d->btnExit, onClicked(), this, Popup::deleteLater );
}

void Popup::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

Popup* Popup::information(Widget* parent, const std::string& title, const std::string& text, const DateTime& time)
{
  return new Popup( parent, title, text, time );
}

}//end namespace messagebox

}//end namespace gui
