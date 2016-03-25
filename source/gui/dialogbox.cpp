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
#include <GameCity>
#include <GameGui>

using namespace gfx;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(Dialogbox)

class Dialogbox::Impl
{
public:
  enum { okPicId=239, cancelPicId=243 };
  bool never=true;

  struct {
    Signal1<int> onResult;
    Signal0<> onOk;
    Signal0<> onCancel;
    Signal1<bool> onNever;
    Signal1<Widget*> onOkEx;
    Signal1<Widget*> onCancelEx;
    Signal2<Widget*,bool> onNeverEx;
  } signal;
};

Dialogbox::Dialogbox(Widget* parent)
 : Window( parent->ui()->rootWidget() ), _d( new Impl )
{
  _initSimpleDialog();
}

Dialogbox::Dialogbox( Ui *ui, const Rect& rectangle, const std::string& title,
                      const std::string& text, int buttons)
                      : Window( ui->rootWidget(), rectangle, "" ), _d( new Impl )
{
  _initSimpleDialog();

  setText(text);
  setTitle(title);
  setButtons(buttons);
}

Signal1<int>& Dialogbox::onResult()
{
  return _d->signal.onResult;
}

bool Dialogbox::onEvent( const NEvent& event )
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
        case btnYes:
          emit _d->signal.onOk();
          emit _d->signal.onOkEx(this);
        break;

        case btnNo:
          emit _d->signal.onCancel();
          emit _d->signal.onCancelEx(this);
        break;

        case btnNever:
        {
          _d->never = !_d->never;
          event.gui.caller->setText(_d->never ? "X" : " ");
          emit _d->signal.onNever(_d->never);
          emit _d->signal.onNeverEx(this, _d->never);
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

void Dialogbox::setupUI(const VariantMap& ui)
{
  Window::setupUI( ui );
}

void Dialogbox::setupUI(const vfs::Path & ui)
{
  Window::setupUI(ui);
}

Signal0<>& Dialogbox::onYes() {  return _d->signal.onOk;}

Signal1<Widget*>&      Dialogbox::onYesEx()   { return _d->signal.onOkEx; }
Signal0<>&             Dialogbox::onNo()      { return _d->signal.onCancel;}
Signal1<Widget*>&      Dialogbox::onNoEx()    { return _d->signal.onCancelEx;  }
Signal1<bool>&         Dialogbox::onNever()   { return _d->signal.onNever; }
Signal2<Widget*,bool>& Dialogbox::onNeverEx() { return _d->signal.onNeverEx; }

void Dialogbox::_initSimpleDialog()
{
  Window::setupUI(":/gui/dialogbox.gui");
  _setSystemButtonsVisible(false);
  GameAutoPauseWidget::insertTo(this);
  onYes().connect( this, &Dialogbox::deleteLater );
  onNo().connect( this, &Dialogbox::deleteLater );

  moveToCenter();
  setModal();
}

void Dialogbox::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Dialogbox::setTitle(const std::string& title)
{
  INIT_WIDGET_FROM_UI( Label*, lbTitle )
  if( lbTitle )
      lbTitle->setText( title );
}

void Dialogbox::setText(const std::string& text)
{
  INIT_WIDGET_FROM_UI( Label*, lbText )

  Font titleFont = _titleWidget() ? _titleWidget()->font() : Font::create( "FONT_4" );

  Font textFont = lbText
                ? lbText->font()
                : Font::create( "FONT_3" );

  int titleHeight = titleFont.getTextSize( "A" ).height();
  int textLineHeight = textFont.getTextSize( "A" ).height() + textFont.kerningHeight();
  Size size = textFont.getTextSize( text );

  if( size.width() > 440 )
  {
    size.setHeight( size.width() / 440 * textLineHeight );
    size.setWidth( 480 );
  }
  else
    size = Size( 480, 40 );

  size += Size( 0, titleHeight ); //title
  size += Size( 0, 50 ); //buttons
  size += Size( 0, 30 ); //borders

  if( lbText )
    lbText->setText(text);

  setGeometry( Rect( Point( 0, 0 ), size ) );
  moveToCenter();
}

void Dialogbox::setNeverValue(bool value)
{
  _d->never = value;
  INIT_WIDGET_FROM_UI( PushButton*, btnActionNever )
  if( btnActionNever )
  {
    btnActionNever->show();
    btnActionNever->setText(_d->never ? "X" : " ");
  }
}

void Dialogbox::setButtons(int buttons)
{
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
}

namespace dialog
{

Dialogbox& Information(Ui* ui, const std::string &title, const std::string &text, bool showNever)
{
  Dialogbox& ret = ui->add<Dialogbox>( Rect(), title, text, Dialogbox::btnYes | (showNever ? Dialogbox::btnNever : 0) );
  return ret;
}

Dialogbox& Confirmation(Ui* ui, const std::string &title, const std::string &text, Callback callback)
{
  auto& dialog = Confirmation( ui, title, text);
  dialog.onYes().connect( callback );

  return dialog;
}

Dialogbox& Confirmation(Ui* ui, const std::string &title, const std::string &text,
                     Callback callbackOk, Callback callbackCancel)
{
  auto& dialog = Confirmation( ui, title, text);
  dialog.onYes().connect( callbackOk );
  dialog.onNo().connect( callbackCancel );

  return dialog;
}

Dialogbox& Confirmation(Ui* ui, const std::string &title, const std::string &text)
{
  Dialogbox& ret = ui->add<Dialogbox>( Rect(), title, text, Dialogbox::btnYesNo );
  return ret;
}

}//end namespace dialog

}//end namespace gui
