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
#include "oc3_scenario.hpp"
#include "oc3_win_targets.hpp"
#include "oc3_texturedbutton.hpp"

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
    PictureRef& pic = _getBackground( state );
    digitFont.draw( *pic, StringHelper::format( 0xff, "%d", _value ), getWidth() / 2 - 10, 15, false );

    Font targetFont = Font::create( FONT_1 );
    targetFont.draw( *pic, StringHelper::format( 0xff, "%d need", _target), 10, getHeight() - 25, false );
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
  TexturedButton* btnHelp;

  void drawColumn( const Point& center, const int value );

  CityPtr city;
};

void AdvisorRatingsWindow::Impl::drawColumn( const Point& center, const int value )
{
  int columnStartY = 275;
  Picture& footer = Picture::load( ResourceGroup::panelBackground, 544 );
  Picture& header = Picture::load( ResourceGroup::panelBackground, 546 );
  Picture& body = Picture::load( ResourceGroup::panelBackground, 545 );

  for( int i=0; i < value; i++ )
  {
    background->draw( body, center.getX() - body.getWidth() / 2, columnStartY - 10 - i * 2 );
  }

  background->draw( footer, center.getX() - footer.getWidth() / 2, columnStartY - footer.getHeight() );
  if( value >= 50 )
  {
    background->draw( header, center.getX() - header.getWidth() / 2, columnStartY - 10 - value * 2);
  }
}

AdvisorRatingsWindow::AdvisorRatingsWindow( Widget* parent, int id, const CityPtr city ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->city = city;
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 432 ) ) );

  Label* title = new Label( this, Rect( 60, 10, 225, 10 + 40) );
  title->setText( "Ratings" );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  GuiPaneling::instance().draw_white_frame(*_d->background, 0, 0, getWidth(), getHeight() );

  //buttons _d->_d->background
  GuiPaneling::instance().draw_black_frame( *_d->background, 66, 360, 510, 60 );

  _d->background->draw( Picture::load( ResourceGroup::menuMiddleIcons, 27), 60, 50 );

  CityWinTargets& targets = Scenario::instance().getWinTargets();

  Font font = Font::create( FONT_2 );
  font.draw( *_d->background, StringHelper::format( 0xff, "(%s %d)", _("##need_population##"), targets.getPopulation() ), 225, 15, false );

  _d->btnCulture    = new RatingButton( this, Point( 80,  290), "culture" );
  _d->btnCulture->setTarget( targets.getCulture() );
  _d->btnCulture->setValue( 0 );
  _d->drawColumn( _d->btnCulture->getRelativeRect().getCenter(), 0 );

  _d->btnProsperity = new RatingButton( this, Point( 200, 290), "prosperity");
  _d->btnProsperity->setValue( _d->city->getProsperity() );
  _d->btnProsperity->setTarget( targets.getProsperity() );
  _d->drawColumn( _d->btnProsperity->getRelativeRect().getCenter(), _d->city->getProsperity() );

  _d->btnPeace      = new RatingButton( this, Point( 320, 290), "peace" );
  //_d->btnPeace->setValue( _d->city->getPeace() );
  _d->btnPeace->setTarget( targets.getPeace() );
  _d->drawColumn( _d->btnPeace->getRelativeRect().getCenter(), 0 );

  _d->btnFavour     = new RatingButton( this, Point( 440, 290), "favour" );
  //_d->btnFavour->setValue( _d->city->getFavour() );
  _d->btnFavour->setTarget( targets.getFavour() );
  _d->drawColumn( _d->btnFavour->getRelativeRect().getCenter(), 0 );

  _d->btnHelp = new TexturedButton( this, Point( 12, getHeight() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
}

void AdvisorRatingsWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}