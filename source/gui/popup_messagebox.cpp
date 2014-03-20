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

namespace gui
{

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
  setPosition( Point( (parent->width() - width())/2, (parent->height() - height()) / 2 ) );
  
  Label* lbTitle = new Label( this, Rect( 10, 10, width() - 10, 10 + 30), title );
  lbTitle->setFont( Font::create( FONT_3 ) );
  lbTitle->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( size() ) );
  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteFrame );
  PictureDecorator::draw( *_d->background, Rect( Point( 18, 50 ), Size( width() - 34, 220 ) ), PictureDecorator::blackFrame );

  _d->btnExit = new TexturedButton( this, Point( width() - 40, height() - 40 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  _d->btnExit->setTooltipText( _("##infobox_tooltip_exit##") );
  CONNECT( _d->btnExit, onClicked(), this, PopupMessageBox::deleteLater );

  _d->btnHelp = new TexturedButton( this, Point( 18, height() - 40 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  _d->btnHelp->setTooltipText( _("##infobox_tooltip_help##") );

  Font font2 = Font::create( FONT_2_WHITE );
  font2.draw( *_d->background, time, 20, 50 );
  font2.draw( *_d->background, receiver, 180, 50 );

  _d->lbText = new Label( this, Rect( Point( 20, 100 ), Size( 550, 165 )), text );
}

void PopupMessageBox::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, screenLeft(), screenTop() );

  Widget::draw( painter );
}

}//end namespace gui
