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

#include "dialogbox.hpp"
#include "gfx/picture.hpp"
#include "gfx/decorator.hpp"
#include "gui/label.hpp"
#include "texturedbutton.hpp"
#include "core/event.hpp"
#include "gfx/engine.hpp"
#include "environment.hpp"
#include "gameautopause.hpp"
#include "core/logger.hpp"

using namespace gfx;

namespace gui
{

namespace dialog
{

namespace {
  int okBtnPicId = 239;
  int cancelBtnPicId = 243;
}

class Dialog::Impl
{
public:
  GameAutoPause locker;

  struct {
    Signal1<int> onResult;
    Signal0<> onOk;
    Signal0<> onCancel;
    Signal0<> onNever;
  } signal;
};

Dialog::Dialog(Ui *ui, const Rect& rectangle, const std::string& title,
                      const std::string& text, int buttons, bool lockGame)
                      : Window( ui->rootWidget(), rectangle, "" ), _d( new Impl )
{
  Font font = Font::create( FONT_3 );

  button( buttonClose )->hide();
  button( buttonMin )->hide();
  button( buttonMax )->hide();

  int titleHeight = font.getTextSize( "A" ).height();
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

    size += Size( 0, titleHeight ); //title
    size += Size( 0, 50 ); //buttons
    size += Size( 0, 30 ); //borders

    setGeometry( Rect( Point( 0, 0 ), size ) );
    setCenter( parent()->center() );
  }
  
  auto& lbTitle = add<Label>( Rect( 10, 10, width() - 10, 10 + titleHeight), title );
  lbTitle.setFont( Font::create( FONT_5 ) );
  lbTitle.setTextAlignment( align::center, align::center );

  auto& lbText = add<Label>( Rect( 10, 20 + titleHeight, width() - 10, height() - 50 ), text );
  lbText.setTextAlignment( align::center, align::upperLeft );
  lbText.setWordwrap( true );

  if( (buttons == btnOk) || (buttons == btnCancel) )
  {
    add<TexturedButton>( Point( width() / 2 - 20, height() - 50),
                         Size( 39, 26 ), buttons,
                         buttons == btnOk ? okBtnPicId : cancelBtnPicId );
  }
  else if( buttons & (btnOk | btnCancel) )
  {
    add<TexturedButton>( Point( width() / 2 - 24 - 16, height() - 50),
                         Size( 39, 26 ), btnOk, okBtnPicId );
    add<TexturedButton>( Point( width() / 2 + 16, height() - 50 ),
                         Size( 39, 26 ), btnCancel, cancelBtnPicId );
  }

  if( buttons & btnNever )
  {
    add<TexturedButton>( Point( width() - 24 - 16, height() - 50),
                         Size( 39, 26 ), btnNever, cancelBtnPicId );


  }

  moveTo( Widget::parentCenter );
  setModal();

  if( lockGame )
    _d->locker.activate();
}

Signal1<int>& Dialog::onResult()
{
  return _d->signal.onResult;
}

bool Dialog::onEvent( const NEvent& event )
{
  switch( event.EventType )
  {
    case sEventGui:
      if( event.gui.type == guiButtonClicked )
      {
        int id = event.gui.caller->ID();
        emit _d->signal.onResult( id );

        switch( id )
        {
        case btnOk: emit _d->signal.onOk(); break;
        case btnCancel: emit _d->signal.onCancel(); break;
        case btnNever: emit _d->signal.onNever(); break;
        }

        return true;
      }
    break;

    case sEventKeyboard:
    {
      switch( event.keyboard.key )
      {
      case KEY_ESCAPE: emit _d->signal.onCancel(); break;
      case KEY_RETURN: emit _d->signal.onOk(); break;
      default: break;
      }

      return true;
    }
    break;

    default:
    break;
  }

  return Widget::onEvent( event );
}

Signal0<>& Dialog::onOk() {  return _d->signal.onOk;}
Signal0<>& Dialog::onCancel(){  return _d->signal.onCancel;}
Signal0<>& Dialog::onNever() { return _d->signal.onNever; }

void Dialog::draw(gfx::Engine& painter )
{
  if( !visible() )
  {
    return;
  }

  Window::draw( painter );
}

Dialog* Information(Ui* ui, const std::string &title, const std::string &text)
{
  Dialog* ret = &ui->add<Dialog>( Rect(), title, text, Dialog::btnOk );

  CONNECT( ret, onOk(), ret, Dialog::deleteLater )
  CONNECT( ret, onCancel(), ret, Dialog::deleteLater )

  return ret;
}

Dialog* Confirmation(Ui* ui, const std::string &title, const std::string &text, Callback callback, bool pauseGame)
{
  auto* dialog = Confirmation( ui, title, text, pauseGame );
  dialog->onOk().connect( callback );

  return dialog;
}

Dialog* Confirmation(Ui* ui, const std::string &title, const std::string &text, bool pauseGame)
{
  Dialog* ret = &ui->add<Dialog>( Rect(), title, text, Dialog::btnOkCancel, pauseGame );

  CONNECT( ret, onOk(), ret, Dialog::deleteLater )
  CONNECT( ret, onCancel(), ret, Dialog::deleteLater )

  return ret;
}

}//end namespace dialog

}//end namespace gui
