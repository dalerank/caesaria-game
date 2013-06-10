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


#include "oc3_advisor_ratings_window.hpp"
#include "oc3_picture.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_gettext.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gfx_engine.hpp"

class AdvisorRatingsWindow::Impl
{
public:
  PictureRef background;
  PushButton* btnCulture;
  PushButton* btnProsperity;
  PushButton* btnPeace;
  PushButton* btnFavour;
};

AdvisorRatingsWindow::AdvisorRatingsWindow( Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 432 ) ) );

  Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40) );
  title->setText( "Ratings" );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  GuiPaneling::instance().draw_white_frame(*_d->background, 0, 0, getWidth(), getHeight() );

  //buttons _d->_d->background
  GuiPaneling::instance().draw_black_frame( *_d->background, 66, 360, 510, 60 );

  _d->background->draw( Picture::load( ResourceGroup::menuMiddleIcons, 27), 60, 50 );

  _d->btnCulture = new PushButton( this, Rect( Point( 80, 290), Size( 108, 65 ) ), "", -1, false, PushButton::WhiteBorderUp );
  _d->btnProsperity = new PushButton( this, Rect( Point( 200, 290), Size( 108, 65 ) ), "", -1, false, PushButton::WhiteBorderUp );
  _d->btnPeace = new PushButton( this, Rect( Point( 320, 290), Size( 108, 65 ) ), "", -1, false, PushButton::WhiteBorderUp );
  _d->btnFavour = new PushButton( this, Rect( Point( 440, 290), Size( 108, 65 ) ), "", -1, false, PushButton::WhiteBorderUp );
}

void AdvisorRatingsWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}