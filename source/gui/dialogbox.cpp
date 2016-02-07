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
#include <GameGfx>
#include <GameEvents>
#include <GameLogger>
#include <GameGui>

using namespace gfx;

namespace gui
{

namespace dialog
{

class Dialog::Impl
{
public:
  enum { okPicId=239, cancelPicId=243 };
  GameAutoPause locker;
  bool never=true;

  struct {
    Signal1<int> onResult;
    Signal0<> onOk;
    Signal0<> onCancel;
    Signal1<bool> onNever;
  } signal;
};

Dialog::Dialog(Ui *ui, const Rect& rectangle, const std::string& title,
                      const std::string& text, int buttons, bool lockGame)
                      : Window( ui->rootWidget(), rectangle, "" ), _d( new Impl )
{
  Window::setupUI( ":/gui/dialogbox.gui" );

  button( buttonClose )->hide();
  button( buttonMin )->hide();
  button( buttonMax )->hide();

  INIT_WIDGET_FROM_UI( Label*, lbText )
  INIT_WIDGET_FROM_UI( Label*, lbTitle )

  Font font = lbText
                ? lbText->font()
                : Font::create( FONT_3 );

  int titleHeight = font.getTextSize( "A" ).height();
  if( rectangle.size() == Size::zero )
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
    moveTo( Widget::parentCenter );
  }
  
  if( lbText )
    lbText->setText( text );

  if( lbTitle )
    lbTitle->setText( title );

  if( (buttons & btnYesNo) == btnYesNo )
  {
    Window::setupUI( ":/gui/dialogbox_yesno.gui" );
  }
  else if( (buttons & btnYes) == btnYes || (buttons & btnNo) == btnNo )
  {
    Window::setupUI( ":/gui/dialogbox_confirmation.gui" );
    INIT_WIDGET_FROM_UI( TexturedButton*, btnAction )
    if( btnAction )
    {
      TexturedButton::States states( (buttons & btnYes)== btnYes ? Impl::okPicId : Impl::cancelPicId );
      btnAction->changeImageSet( states );
      btnAction->setID( btnYes );
    }
  }

  INIT_WIDGET_FROM_UI( PushButton*, btnActionNever )
  INIT_WIDGET_FROM_UI( TexturedButton*, btnActionYes )
  INIT_WIDGET_FROM_UI( TexturedButton*, btnActionNo )
  if( btnActionNever )
  {
    btnActionNever->setVisible( (buttons & btnNever) == btnNever );
    btnActionNever->setID( btnNever );
  }

  if( btnActionYes )
    btnActionYes->setID( btnYes );

  if( btnActionNo )
    btnActionNo->setID( btnNo );

  if( lockGame )
    _d->locker.activate();

  moveTo( Widget::parentCenter );
  setModal();
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
        case btnYes: emit _d->signal.onOk(); break;
        case btnNo: emit _d->signal.onCancel(); break;
        case btnNever:
        {
          _d->never = !_d->never;
          event.gui.caller->setText( _d->never ? "X" : " " );
          emit _d->signal.onNever( _d->never );
        }
        break;
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

void Dialog::setupUI(const VariantMap& ui)
{
  Window::setupUI( ui );
}

Signal0<>& Dialog::onYes() {  return _d->signal.onOk;}
Signal0<>& Dialog::onNo(){  return _d->signal.onCancel;}
Signal1<bool>& Dialog::onNever() { return _d->signal.onNever; }

void Dialog::draw(gfx::Engine& painter )
{
  if( !visible() )
  {
    return;
  }

  Window::draw( painter );
}

Dialog& Information(Ui* ui, const std::string &title, const std::string &text, bool showNever)
{
  Dialog& ret = ui->add<Dialog>( Rect(), title, text, Dialog::btnYes | (showNever ? Dialog::btnNever : 0) );

  ret.onYes().connect( &ret, &Dialog::deleteLater );
  ret.onNo().connect( &ret, &Dialog::deleteLater );

  return ret;
}

Dialog& Confirmation(Ui* ui, const std::string &title, const std::string &text, Callback callback, bool pauseGame)
{
  auto& dialog = Confirmation( ui, title, text, pauseGame );
  dialog.onYes().connect( callback );

  return dialog;
}

Dialog& Confirmation(Ui* ui, const std::string &title, const std::string &text,
                     Callback callbackOk, Callback callbackCancel, bool pauseGame)
{
  auto& dialog = Confirmation( ui, title, text, pauseGame );
  dialog.onYes().connect( callbackOk );
  dialog.onNo().connect( callbackCancel );

  return dialog;
}

Dialog& Confirmation(Ui* ui, const std::string &title, const std::string &text, bool pauseGame)
{
  Dialog& ret = ui->add<Dialog>( Rect(), title, text, Dialog::btnYesNo, pauseGame );

  ret.onYes().connect( &ret, &Dialog::deleteLater );
  ret.onNo().connect( &ret, &Dialog::deleteLater );

  return ret;
}

}//end namespace dialog

}//end namespace gui
