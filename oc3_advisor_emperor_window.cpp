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


#include "oc3_advisor_emperor_window.hpp"
#include "oc3_picture.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_gettext.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_groupbox.hpp"

class AdvisorEmperorWindow::Impl
{
public:
  PictureRef background;
  Label* lbEmperorFavour;
  Label* lbEmperorFavourDesc;
  Label* lbPost;
  Label* lbPrimaryFunds;  
  PushButton* btnSendGift;
  PushButton* btnSend2City;
  PushButton* btnChangeSalary;
};

void AdvisorEmperorWindow::showChangeSalaryWindow()
{
  GroupBox* gb = new GroupBox( getParent(), Rect( Point( 256, 176 ), Size( 510, 400 )), -1, GroupBox::whiteFrame );
}

AdvisorEmperorWindow::AdvisorEmperorWindow( Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 432 ) ) );

  Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40) );
  title->setText( "Player name" );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  GuiPaneling::instance().draw_white_frame(*_d->background, 0, 0, getWidth(), getHeight() );

  //buttons _d->_d->background
  Point startPos( 32, 91 );
  GuiPaneling::instance().draw_black_frame(*_d->background, startPos.getX(), startPos.getY(), 570, 220 );

  GuiPaneling::instance().draw_black_frame( *_d->background, 66, 325, 510, 94 );
  
  _d->lbEmperorFavour = new Label( this, Rect( Point( 58, 44 ), Size( 550, 20 ) ), "Favour of the emperor 50" );
  _d->lbEmperorFavourDesc = new Label( this, _d->lbEmperorFavour->getRelativeRect() + Point( 0, 20 ), "The emperor has mixed feelings to you" );

  _d->lbPost = new Label( this, Rect( Point( 70, 336 ), Size( 240, 26 ) ), "Post");
  _d->lbPrimaryFunds = new Label( this, Rect( Point( 70, 370 ), Size( 240, 20 ) ), "PrimaryFunds 0" );

  _d->btnSendGift = new PushButton( this, Rect( Point( 322, 343), Size( 250, 20 ) ), "Send gift", -1, false, PushButton::BlackBorderUp );
  _d->btnSend2City = new PushButton( this, Rect( Point( 322, 370), Size( 250, 20 ) ), "Send to city", -1, false, PushButton::BlackBorderUp );
  _d->btnChangeSalary = new PushButton( this, Rect( Point( 70, 395), Size( 500, 20 ) ), "Change salary", -1, false, PushButton::BlackBorderUp );  
}

void AdvisorEmperorWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}