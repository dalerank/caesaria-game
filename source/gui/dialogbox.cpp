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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "dialogbox.hpp"
#include "gfx/picture.hpp"
#include "gfx/decorator.hpp"
#include "gui/label.hpp"
#include "texturedbutton.hpp"
#include "core/event.hpp"
#include "gfx/engine.hpp"

using namespace gfx;

namespace gui
{

namespace {
  int okBtnPicId = 239;
  int cancelBtnPicId = 243;
}

class DialogBox::Impl
{
signals public:
  Signal1<int> onResultSignal;
  Signal0<> onOkSignal;
  Signal0<> onCancelSignal;
  Signal0<> onNeverSignal;
};

DialogBox::DialogBox( Widget* parent, const Rect& rectangle, const std::string& title, 
                      const std::string& text, int buttons )
                      : Window( parent, rectangle, "" ), _d( new Impl )
{
  Font font = Font::create( FONT_3 );
  if( rectangle.size() == Size( 0, 0 ) )
  {    
    Size size = font.getTextSize( text );

    if( size.width() > 440 )
    {
      size.setHeight( size.width() / 440 * 40 );
      size.setWidth( 480 );
    }
    else
      size = Size( 480, 60 );

    size += Size( 0, 50 ); //title
    size += Size( 0, 50 ); //buttons

    setGeometry( Rect( Point( 0, 0 ), size ) );
    setCenter( parent->center() );
  }
  
  Label* lbTitle = new Label( this, Rect( 10, 10, width() - 10, 10 + 40), title );
  lbTitle->setFont( Font::create( FONT_5 ) );
  lbTitle->setTextAlignment( align::center, align::center );  

  Label* lbText = new Label( this, Rect( 10, 55, width() - 10, height() - 55 ), text );
  lbText->setTextAlignment( align::center, align::center );
  lbText->setWordwrap( true );

  if( (buttons == btnOk) || (buttons == btnCancel) )
  {
    new TexturedButton( this, Point( width() / 2 - 20, height() - 50),
                        Size( 39, 26 ), buttons,
                        buttons == btnOk ? okBtnPicId : cancelBtnPicId );
  }
  else if( buttons & (btnOk | btnCancel) )
  {
    new TexturedButton( this, Point( width() / 2 - 24 - 16, height() - 50),
                        Size( 39, 26 ), btnOk, okBtnPicId );
    new TexturedButton( this, Point( width() / 2 + 16, height() - 50 ),
                        Size( 39, 26 ), btnCancel, cancelBtnPicId );
  }

  if( buttons & btnNever )
  {
    new TexturedButton( this, Point( width() - 24 - 16, height() - 50),
                        Size( 39, 26 ), btnNever, cancelBtnPicId );


    }
  setModal();
}

Signal1<int>& DialogBox::onResult()
{
  return _d->onResultSignal;
}

bool DialogBox::onEvent( const NEvent& event )
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->ID();
    emit _d->onResultSignal( id );

    switch( id )
    {
    case btnOk: emit _d->onOkSignal(); break;
    case btnCancel: emit _d->onCancelSignal(); break;
    case btnNever: emit _d->onNeverSignal(); break;
    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal0<>& DialogBox::onOk() {  return _d->onOkSignal;}
Signal0<>& DialogBox::onCancel(){  return _d->onCancelSignal;}
Signal0<>& DialogBox::onNever() { return _d->onNeverSignal; }

void DialogBox::draw(gfx::Engine& painter )
{
  if( !visible() )
  {
    return;
  }

  Window::draw( painter );
}

}//end namespace gui
