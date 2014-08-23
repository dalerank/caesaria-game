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
#include "objects/construction.hpp"
#include "city/city.hpp"
#include "city/victoryconditions.hpp"
#include "texturedbutton.hpp"
#include "city/cityservice_culture.hpp"
#include "city/cityservice_prosperity.hpp"
#include "world/empire.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"
#include "world/emperor.hpp"
#include "city/funds.hpp"
#include "city/cityservice_military.hpp"
#include "city/requestdispatcher.hpp"
#include "city/cityservice_info.hpp"

using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

class RatingButton : public PushButton
{
public:
  RatingButton( Widget* parent, Point pos, std::string title, std::string tooltip )
    : PushButton( parent, Rect( pos, Size( 108, 65 )), _(title), -1, false, PushButton::whiteBorderUp )
  {
    setTextAlignment( align::center, align::upperLeft );
    setTooltipText( _(tooltip) );
    _value = 0;
    _target = 0;
  }

  virtual void _updateTextPic()
  {
    PushButton::_updateTextPic();

    Font digitFont = Font::create( FONT_4 );
    PictureRef& pic = _textPictureRef();
    if( pic )
    {
      digitFont.draw( *pic, StringHelper::format( 0xff, "%d", _value ), width() / 2 - 10, 17, true, false );

      Font targetFont = Font::create( FONT_1 );
      targetFont.draw( *pic, StringHelper::format( 0xff, "%d %s", _target, _("##wndrt_need##") ), 10, height() - 20, true, false );

      pic->update();
    }     
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

class Ratings::Impl
{
public:
  Pictures columns;
  RatingButton* btnCulture;
  RatingButton* btnProsperity;
  RatingButton* btnPeace;
  RatingButton* btnFavour;
  TexturedButton* btnHelp;
  Label* lbRatingInfo;

  void updateColumn( const Point& alignCenter, const int value );
  void checkCultureRating();
  void checkProsperityRating();
  void checkPeaceRating();
  void checkFavourRating();

  PlayerCityPtr city;
};

void Ratings::Impl::updateColumn( const Point& center, const int value )
{
  int columnStartY = 275;
  const Picture& footer = Picture::load( ResourceGroup::panelBackground, 544 );
  const Picture& header = Picture::load( ResourceGroup::panelBackground, 546 );
  const Picture& body = Picture::load( ResourceGroup::panelBackground, 545 );

  for( int i=0; i < value; i++ )
  {
    columns.append( body, Point( center.x() - body.width() / 2, -columnStartY + (10 + i * 2)) );
  }

  columns.append( footer, Point( center.x() - footer.width() / 2, -columnStartY + footer.height()) );
  if( value >= 50 )
  {
    columns.append( header, Point( center.x() - header.width() / 2, -columnStartY + (10 + value * 2)) );
  }
}

void Ratings::Impl::checkCultureRating()
{
  city::CultureRatingPtr culture;
  culture << city->findService( city::CultureRating::defaultName() );

  if( culture != 0 )
  {
    if( culture->value() == 0 )
    {
      lbRatingInfo->setText( _("##no_culture_building_in_city##") );
      return;
    }

    StringArray troubles;

    const char* covTypename[CultureRating::covCount] = { "school", "library", "academy", "temple", "theater" };
    for( int k=CultureRating::covSchool; k < CultureRating::covCount; k++)
    {
      int coverage = culture->coverage( CultureRating::Coverage(k) );
      if( coverage < 100 )
      {
        std::string troubleDesc = StringHelper::format( 0xff, "##have_less_%s_in_city_%d##", covTypename[ k ], coverage / 50 );
        troubles.push_back( troubleDesc );
      }
    }

    if( !troubles.empty() )
    {
      lbRatingInfo->setText( _( troubles.random() ) );
    }
  }
}

void Ratings::Impl::checkProsperityRating()
{
  city::ProsperityRatingPtr prosperity;
  prosperity << city->findService( city::ProsperityRating::defaultName() );

  if( prosperity != 0 )
  {
    StringArray troubles;
    unsigned int prValue = prosperity->value();
    if( prValue == 0 )
    {
      lbRatingInfo->setText( _("##cant_calc_prosperity##") );
      return;
    }

    city::InfoPtr info;
    info << city->findService( city::Info::defaultName() );

    city::Info::Parameters current = info->lastParams();
    city::Info::Parameters lastYear = info->yearParams( 0 );

    if( current.prosperity > lastYear.prosperity ) { troubles <<  "##your_prosperity_raising##"; }

    if( prosperity->getMark( city::ProsperityRating::cmHousesCap ) < 0 ) { troubles << "##bad_house_quality##"; }
    if( prosperity->getMark( city::ProsperityRating::cmHaveProfit ) == 0 ) { troubles << "##lost_money_last_year##"; }
    if( prosperity->getMark( city::ProsperityRating::cmWorkless ) > 15 ) { troubles << "##high_workless_number##"; }
    if( prosperity->getMark( city::ProsperityRating::cmWorkersSalary ) < 0 ) { troubles << "##workers_salary_less_then_rome##"; }
    if( prosperity->getMark( city::ProsperityRating::cmPercentPlebs ) > 30 ) { troubles << "##much_plebs##"; }
    if( prosperity->getMark( city::ProsperityRating::cmChange ) == 0 )
    {
      troubles << "##no_prosperity_change##";
      troubles << "##how_to_grow_prosperity##";
    }
    if( prValue > 90 ) { troubles << "##amazing_prosperity_this_city##"; }


    unsigned int caesarsHelper = city->funds().getIssueValue( city::Funds::caesarsHelp, city::Funds::thisYear );
    caesarsHelper += city->funds().getIssueValue( city::Funds::caesarsHelp, city::Funds::lastYear );
    if( caesarsHelper > 0 )
    {
      troubles << "##emperor_send_money_to_you_nearest_time##";
    }

    std::string text = troubles.empty()
                        ? "##good_prosperity##"
                        : troubles.random();

    lbRatingInfo->setText( _(text) );
  }
}

void Ratings::Impl::checkPeaceRating()
{
  StringArray advices;
  city::MilitaryPtr ml;
  ml << city->findService( city::Military::defaultName() );

  unsigned int peace = city->peace();

  if( ml->month2lastAttack() < 36 )
  {
    advices << "##province_has_peace_a_short_time##";
  }

  if( peace > 90 ) { advices << "##your_province_quiet_and_secure##"; }
  else if(peace > 80 ) { advices << "##overall_city_become_a_sleepy_province##"; }
  else if( peace > 50 ) { advices << "##this_lawab_province_become_very_peacefull##"; }

  std::string text = advices.empty()
                      ? "##peace_rating_text##"
                      : advices.random();

  lbRatingInfo->setText( _(text) );
}

void Ratings::Impl::checkFavourRating()
{
  StringArray problems;
  city::request::DispatcherPtr rd;
  rd << city->findService( city::request::Dispatcher::defaultName() );

  city::InfoPtr info;
  info << city->findService( city::Info::defaultName() );

  city::Info::Parameters current = info->lastParams();
  city::Info::Parameters lastYear = info->yearParams( 0 );

  PlayerPtr player = city->player();
  world::GovernorRank rank = world::EmpireHelper::getRank( player->rank() );
  float salaryKoeff = player->salary() / (float)rank.salary;

  int brokenEmpireTax = city->funds().getIssueValue( city::Funds::overdueEmpireTax, city::Funds::lastYear );
  if( brokenEmpireTax > 0 )
  {
    int twoYearsAgoBrokenTax = city->funds().getIssueValue( city::Funds::overdueEmpireTax, city::Funds::twoYearAgo );

    if( twoYearsAgoBrokenTax > 0 ) { problems << "##broke_empiretax_with2years_warning##"; }
    else { problems << "##broke_empiretax_warning##"; }
  }

  if( salaryKoeff >= 3.f )     { problems << "##high_salary_angers_senate##";  }
  else if( salaryKoeff > 2.5f ) { problems << "##more_salary_dispeasure_senate##";  }
  else if( salaryKoeff > 2.f ){ problems << "##try_reduce_your_high_salary##"; }
  else if( salaryKoeff > 1.5f ) { problems << "##try_reduce_your_salary##"; }
  else if( salaryKoeff > 1.f ) { problems << "##your_salary_frowned_senate##"; }

  if( current.favour == lastYear.favour )   {    problems << "##your_favour_unchanged_from_last_year##";  }
  else if( current.favour > lastYear.favour ) { problems << "##your_favour_increased_from_last_year##"; }

  if( current.favour < 30 )
  {
    problems << "##your_favor_is_dropping_catch_it##";
  }

  if( rd.isValid() )
  {
    if( rd->haveCanceledRequest() )
    {
      problems << "##imperial_request_cance_badly_affected##";
    }
  }

  std::string text = problems.empty()
                      ? _("##no_favour_problem##")
                      : problems.random();

  lbRatingInfo->setText( _(text) );
}

Ratings::Ratings(Widget* parent, int id, const PlayerCityPtr city )
  : Window( parent, Rect( 0, 0, 640, 432 ), "", id ), _d( new Impl )
{
  _d->city = city;
  setupUI( ":/gui/ratingsadv.gui" );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  Label* lbNeedPopulation;
  GET_WIDGET_FROM_UI( lbNeedPopulation )
  GET_DWIDGET_FROM_UI( _d, lbRatingInfo )

  const city::VictoryConditions& targets = city->victoryConditions();

  if( lbNeedPopulation ) lbNeedPopulation->setText( StringHelper::format( 0xff, "(%s %d)", _("##need_population##"), targets.needPopulation() ) );

  _d->btnCulture    = new RatingButton( this, Point( 80,  290), "##wdnrt_culture##", "##wndrt_culture_tooltip##" );
  _d->btnCulture->setTarget( targets.needCulture() );
  _d->btnCulture->setValue( _d->city->culture() );
  _d->updateColumn( _d->btnCulture->relativeRect().getCenter(), 0 );
  CONNECT( _d->btnCulture, onClicked(), _d.data(), Impl::checkCultureRating );

  _d->btnProsperity = new RatingButton( this, Point( 200, 290), "##wndrt_prosperity##", "##wndrt_prosperity_tooltip##" );
  _d->btnProsperity->setValue( _d->city->prosperity() );
  _d->btnProsperity->setTarget( targets.needProsperity() );
  _d->updateColumn( _d->btnProsperity->relativeRect().getCenter(), _d->city->prosperity() );
  CONNECT( _d->btnProsperity, onClicked(), _d.data(), Impl::checkProsperityRating );

  _d->btnPeace      = new RatingButton( this, Point( 320, 290), "##wndrt_peace##", "##wndrt_peace_tooltip##" );
  _d->btnPeace->setValue( _d->city->peace() );
  _d->btnPeace->setTarget( targets.needPeace() );
  _d->updateColumn( _d->btnPeace->relativeRect().getCenter(), 0 );
  CONNECT( _d->btnPeace, onClicked(), _d.data(), Impl::checkPeaceRating );

  _d->btnFavour     = new RatingButton( this, Point( 440, 290), "##wndrt_favour##", "##wndrt_favour_tooltip##" );
  _d->btnFavour->setValue( _d->city->favour() );
  _d->btnFavour->setTarget( targets.needFavour() );
  _d->updateColumn( _d->btnFavour->relativeRect().getCenter(), 0 );
  CONNECT( _d->btnFavour, onClicked(), _d.data(), Impl::checkFavourRating );

  _d->btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
}

void Ratings::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );

  painter.draw( _d->columns, absoluteRect().lefttop(), &absoluteClippingRectRef() );
}

}

}//end namespace gui
