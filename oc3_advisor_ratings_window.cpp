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
#include "oc3_font.hpp"
#include "oc3_city.hpp"

class RatingButton : public PushButton
{
public:
  RatingButton( Widget* parent, const Point& pos, const std::string& title )
    : PushButton( parent, Rect( pos, Size( 108, 65 )), title, -1, false, PushButton::WhiteBorderUp )
  {
    setTextAlignment( alignCenter, alignUpperLeft );
    _value = 0;
    _target = 0;
  }

  void _updateTexture( ElementState state )
  {
    PushButton::_updateTexture( state );

    Font digitFont = Font::create( FONT_3 );
    PictureRef& pic = _getPicture( state );
    digitFont.draw( *pic, StringHelper::format( 0xff, "%d", _value ), getWidth() / 2 - 10, 15 );

    Font targetFont = Font::create( FONT_1 );
    targetFont.draw( *pic, StringHelper::format( 0xff, "%d need", _target), 10, getHeight() - 25 );
  }

  void setValue( const int value )
  {
    _value = value;
    resizeEvent_();
  }

  void setTarget( const int value )
  {
    _target = value;
    resizeEvent_();
  }

private:
  int _value;
  int _target;
};

class AdvisorRatingsWindow::Impl
{
public:
  PictureRef background;
  RatingButton* btnCulture;
  RatingButton* btnProsperity;
  RatingButton* btnPeace;
  RatingButton* btnFavour;

  City const* city;
};

AdvisorRatingsWindow::AdvisorRatingsWindow( Widget* parent, int id, const City& city ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->city = &city;
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

  _d->btnCulture    = new RatingButton( this, Point( 80,  290), "culture" );
  _d->btnProsperity = new RatingButton( this, Point( 200, 290), "prosperity");
  _d->btnProsperity->setValue( _d->city->getProsperity() );

  _d->btnPeace      = new RatingButton( this, Point( 320, 290), "peace" );
  _d->btnFavour     = new RatingButton( this, Point( 440, 290), "favour" );
}

void AdvisorRatingsWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}