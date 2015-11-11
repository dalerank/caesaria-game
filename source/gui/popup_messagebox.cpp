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

Popup::Popup( Widget* parent, const std::string& title,
                              const std::string& text,
                              const DateTime& time,
                              const std::string& receiver, int id )
  : Window( parent, Rect( 0, 0, 590, 320  ), "", id )
{
  setupUI( ":/gui/popupmessage.bui" );
  setCenter( parent->center() );
  
  INIT_WIDGET_FROM_UI(Label*, lbTitle )
  INIT_WIDGET_FROM_UI(PushButton*, btnExit )
  //GET_DWIDGET_FROM_UI( _d, btnHelp )
  INIT_WIDGET_FROM_UI(Label*, lbTime )
  INIT_WIDGET_FROM_UI(Label*, lbReceiver )
  INIT_WIDGET_FROM_UI(Label*, lbText  )

  if( lbTitle ) lbTitle->setText( title );
  if( lbTime ) lbTime->setText( utils::date2str( time, true ) );
  if( lbReceiver ) lbReceiver->setText( receiver );

  CONNECT( btnExit, onClicked(), this, Popup::deleteLater );
}

void Popup::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

Popup* Popup::information(Widget* parent, const std::string& title, const std::string& text, const DateTime& time)
{
  return &parent->add<Popup>( title, text, time );
}

}//end namespace messagebox

}//end namespace gui
