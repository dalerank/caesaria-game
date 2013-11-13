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


#include "advisor_ratings_window.hpp"
#include "gfx/picture.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "core/font.hpp"
#include "game/city.hpp"
#include "game/win_targets.hpp"
#include "texturedbutton.hpp"
#include "game/cityservice_culture.hpp"
#include "game/cityservice_prosperity.hpp"
#include "core/logger.hpp"

namespace gui
{

class RatingButton : public PushButton
{
public:
  RatingButton( Widget* parent, const Point& pos, const std::string& title )
    : PushButton( parent, Rect( pos, Size( 108, 65 )), title, -1, false, PushButton::whiteBorderUp )
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
    targetFont.draw( *pic, StringHelper::format( 0xff, "%d %s", _target, _("##wndrt_need##") ), 10, getHeight() - 25, false );
  }

  void setValue( const int value )
  {
    _value = value;
    _resizeEvent();
  }

  void setTarget( const int value )
  {
    _target = value;
    _resizeEvent();
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
  Label* lbRatingInfo;

  void drawColumn( const Point& center, const int value );
  void checkCultureRating();
  void checkProsperityRating();
  void checkPeaceRating();

  PlayerCityPtr city;
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

void AdvisorRatingsWindow::Impl::checkCultureRating()
{
  SmartPtr< CityServiceCulture > culture = city->findService( CityServiceCulture::getDefaultName() ).as<CityServiceCulture>();

  if( culture != 0 )
  {
    if( culture->getValue() == 0 )
    {
      lbRatingInfo->setText( _("##no_culture_building_in_city##") );
      return;
    }

    StringArray troubles;
    if( culture->getCoverage( CityServiceCulture::ccSchool ) < 100 ) { troubles.push_back( _("##have_less_school_in_city##") ); }
    if( culture->getCoverage( CityServiceCulture::ccLibrary ) < 100 ) { troubles.push_back( _("##have_less_library_in_city##" ) ); }
    if( culture->getCoverage( CityServiceCulture::ccAcademy ) < 100 ) { troubles.push_back( _("##have_less_academy_in_city##" ) ); }
    if( culture->getCoverage( CityServiceCulture::ccReligion ) < 100 ) { troubles.push_back( _("##have_less_temples_in_city##" ) ); }
    if( culture->getCoverage( CityServiceCulture::ccReligion ) < 100 ) { troubles.push_back( _("##have_less_theatres_in_city##" ) ); }

    if( !troubles.empty() )
    {
      lbRatingInfo->setText( troubles.at( rand() % troubles.size() ) );
    }
  }
}

void AdvisorRatingsWindow::Impl::checkProsperityRating()
{
  SmartPtr< CityServiceProsperity > prosperity = city->findService( CityServiceProsperity::getDefaultName() ).as<CityServiceProsperity>();

  if( prosperity != 0 )
  {
    if( prosperity->getValue() == 0 )
    {
      lbRatingInfo->setText( _("##cant_calc_prosperity##") );
      return;
    }

    StringArray troubles;
    if( prosperity->getMark( CityServiceProsperity::cmHousesCap ) < 0 ) { troubles.push_back( _("##bad_house_quality##") ); }
    if( prosperity->getMark( CityServiceProsperity::cmHaveProfit ) == 0 ) { troubles.push_back( _("##lost_money_last_year##") ); }
    if( prosperity->getMark( CityServiceProsperity::cmWorkless ) > 15 ) { troubles.push_back( _("##high_workless_number##") ); }
    if( prosperity->getMark( CityServiceProsperity::cmWorkersSalary ) < 0 ) { troubles.push_back( _("##workers_salary_less_then_rome##") ); }
    if( prosperity->getMark( CityServiceProsperity::cmPercentPlebs ) > 30 ) { troubles.push_back( _("##much_plebs##") ); }
    if( prosperity->getMark( CityServiceProsperity::cmChange ) == 0 )
    {
      troubles.push_back( _("##no_prosperity_change##") );
      troubles.push_back( _("##how_to_grow_prosperity##") );
    }

    std::string text = troubles.empty()
                        ? _("##good_prosperity##")
                        : troubles.at( rand() % troubles.size() );

    lbRatingInfo->setText( text );
  }
}

void AdvisorRatingsWindow::Impl::checkPeaceRating()
{
  lbRatingInfo->setText( _("##peace_rating_text##") );
}

AdvisorRatingsWindow::AdvisorRatingsWindow(Widget* parent, int id, const PlayerCityPtr city )
  : Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->city = city;
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 432 ) ) );

  Label* title = new Label( this, Rect( 60, 10, 225, 10 + 40) );
  title->setText( _("##wnd_ratings_title##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  //buttons _d->_d->background
  Rect r( Point( 66, 360 ), Size( 510, 60 ) );
  PictureDecorator::draw( *_d->background, r, PictureDecorator::blackFrame );
  _d->lbRatingInfo = new Label( this, r, _("##click_on_rating_for_info##") );

  _d->background->draw( Picture::load( ResourceGroup::menuMiddleIcons, 27), 60, 50 );

  const CityWinTargets& targets = city->getWinTargets();

  Font font = Font::create( FONT_2 );
  font.draw( *_d->background, StringHelper::format( 0xff, "(%s %d)", _("##need_population##"), targets.getPopulation() ), 225, 15, false );

  _d->btnCulture    = new RatingButton( this, Point( 80,  290), _("##wdnrt_culture##") );
  _d->btnCulture->setTarget( targets.getCulture() );
  _d->btnCulture->setValue( _d->city->getCulture() );
  _d->drawColumn( _d->btnCulture->getRelativeRect().getCenter(), 0 );
  CONNECT( _d->btnCulture, onClicked(), _d.data(), Impl::checkCultureRating );

  _d->btnProsperity = new RatingButton( this, Point( 200, 290), _("##wndrt_prosperity##") );
  _d->btnProsperity->setValue( _d->city->getProsperity() );
  _d->btnProsperity->setTarget( targets.getProsperity() );
  _d->drawColumn( _d->btnProsperity->getRelativeRect().getCenter(), _d->city->getProsperity() );
  CONNECT( _d->btnProsperity, onClicked(), _d.data(), Impl::checkProsperityRating );

  _d->btnPeace      = new RatingButton( this, Point( 320, 290), _("##wndrt_peace##") );
  //_d->btnPeace->setValue( _d->city->getPeace() );
  _d->btnPeace->setTarget( targets.getPeace() );
  _d->drawColumn( _d->btnPeace->getRelativeRect().getCenter(), 0 );
  CONNECT( _d->btnPeace, onClicked(), _d.data(), Impl::checkPeaceRating );

  _d->btnFavour     = new RatingButton( this, Point( 440, 290), _("##wndrt_favour##") );
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

}//end namespace gui
