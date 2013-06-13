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

#include "oc3_popup_messagebox.hpp"
#include "oc3_picture.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_listbox.hpp"
#include "oc3_editbox.hpp"
#include "oc3_gettext.hpp"
#include "oc3_label.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_texturedbutton.hpp"

class PopupMessageBox::Impl
{
public:
  PictureRef background;
  PushButton* btnExit;
  PushButton* btnHelp; 
  Label* lbText;

};

PopupMessageBox::PopupMessageBox( Widget* parent, const std::string& title, 
                                 const std::string& text, 
                                 const std::string& time,
                                 const std::string& receiver, int id ) 
  : Widget( parent, id, Rect( 0, 0, 590, 320 ) ), _d( new Impl )
{
  setPosition( Point( (parent->getWidth() - getWidth())/2, (parent->getHeight() - getHeight()) / 2 ) );
  
  Label* lbTitle = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 30), title );
  lbTitle->setFont( Font::create( FONT_3 ) );
  lbTitle->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  GuiPaneling::instance().draw_white_frame(*_d->background, 0, 0, getWidth(), getHeight() );
  GuiPaneling::instance().draw_black_frame(*_d->background, 18, 50, getWidth() - 34, 220 );

  _d->btnExit = new TexturedButton( this, Point( getWidth() - 40, getHeight() - 40 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  _d->btnExit->setTooltipText( _("##infobox_tooltip_exit##") );
  CONNECT( _d->btnExit, onClicked(), this, PopupMessageBox::deleteLater );

  _d->btnHelp = new TexturedButton( this, Point( 18, getHeight() - 40 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  _d->btnHelp->setTooltipText( _("##infobox_tooltip_help##") );

  Font font2 = Font::create( FONT_2_WHITE );
  font2.draw( *_d->background, time, 20, 50 );
  font2.draw( *_d->background, receiver, 180, 50 );

  _d->lbText = new Label( this, Rect( Point( 20, 100 ), Size( 550, 165 )), text, false, true );
}

void PopupMessageBox::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}