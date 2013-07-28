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

#include "oc3_gui_dialogbox.hpp"
#include "oc3_picture.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_event.hpp"

class DialogBox::Impl
{
public:
  PictureRef background;

oc3_signals public:
  Signal1<int> onResultSignal;
  Signal0<> onOkSignal;
  Signal0<> onCancelSignal;
};

DialogBox::DialogBox( Widget* parent, const Rect& rectangle, const std::string& title, 
                      const std::string& text, int buttons )
                      : Widget( parent, -1, rectangle ), _d( new Impl )
{
  if( rectangle.getSize() == Size( 0, 0 ) )
  {
    Size defaultSize( 480, 160 );
    Rect defaultRect( Point( (parent->getWidth() - defaultSize.getWidth())/2,(parent->getHeight() - defaultSize.getHeight())/2),
                      defaultSize );
    setGeometry( defaultRect );
  }

  _d->background.reset( Picture::create( getSize() ) );
  GuiPaneling::instance().draw_white_frame( *_d->background, 0, 0, getWidth(), getHeight() );
  Label* lbTitle = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40), title );
  lbTitle->setFont( Font::create( FONT_3 ) );
  lbTitle->setTextAlignment( alignCenter, alignCenter );

  Label* lbText = new Label( this, Rect( 10, 55, getWidth() - 10, 55 + 55 ), text );
  lbText->setTextAlignment( alignCenter, alignCenter );

  new TexturedButton( this, Point( getWidth() / 2 - 24 - 16, getHeight() - 50), 
                      Size( 39, 26 ), btnOk, ResourceMenu::okBtnPicId );
  new TexturedButton( this, Point( getWidth() / 2 + 16, getHeight() - 50 ), 
                      Size( 39, 26 ), btnCancel, ResourceMenu::cancelBtnPicId );
}

Signal1<int>& DialogBox::onResult()
{
  return _d->onResultSignal;
}

bool DialogBox::onEvent( const NEvent& event )
{
  if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_BUTTON_CLICKED )
  {
    int id = event.GuiEvent.Caller->getID();
    _d->onResultSignal.emit( id );

    switch( id )
    {
    case btnOk: _d->onOkSignal.emit(); break;
    case btnCancel: _d->onCancelSignal.emit(); break;
    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal0<>& DialogBox::onOk()
{
  return _d->onOkSignal;
}

Signal0<>& DialogBox::onCancel()
{
  return _d->onCancelSignal;
}

void DialogBox::draw( GfxEngine& painter )
{
  if( !isVisible() )
  {
    return;
  }

  if( _d->background )
  {
    painter->drawPicture( *_d->background, getScreenLeft(), getScreenTop() );
  }

  Widget::draw( painter );
}