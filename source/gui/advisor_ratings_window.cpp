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

#include "advisor_ratings_window.hpp"
#include "gfx/picture.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "game/player.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
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
#include "game/funds.hpp"
#include "city/states.hpp"
#include "dictionary.hpp"
#include "city/cityservice_peace.hpp"
#include "city/cityservice_military.hpp"
#include "city/requestdispatcher.hpp"
#include "city/cityservice_info.hpp"
#include "city/statistic.hpp"
#include "advisor_rating_button.hpp"

using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

namespace {
enum { muchPlebsPercent=30, peaceAverage=50, cityAmazinProsperity=90, peaceLongTime=90 };

const char* const cultureCoverageDesc[CultureRating::covCount] = { "school", "library", "academy", "temple", "theater" };
}

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
  PlayerCityPtr city;

public:
  void updateColumn( const Point& alignCenter, const int value );
  void checkCultureRating();
  void checkProsperityRating();
  void checkPeaceRating();
  void checkFavourRating();
};

void Ratings::Impl::updateColumn( const Point& center, const int value )
{
  int columnStartY = 275;
  Picture footer( ResourceGroup::panelBackground, 544 );
  Picture header( ResourceGroup::panelBackground, 546 );
  Picture body( ResourceGroup::panelBackground, 545 );

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
  CultureRatingPtr culture = city->statistic().services.find<CultureRating>();

  if( culture.isValid() )
  {
    if( culture->value() == 0  )
    {
      lbRatingInfo->setText( _("##no_culture_building_in_city##") );
      return;
    }

    StringArray troubles;

    for( int k=CultureRating::covSchool; k < CultureRating::covCount; k++)
    {
      int coverage = culture->coverage( CultureRating::Coverage(k) );
      int objects_n = culture->objects_n( CultureRating::Coverage(k) );
      if( objects_n == 0 )
      {
        std::string troubleDesc = fmt::format( "##haveno_{}_in_city##", cultureCoverageDesc[ k ] );
        troubles.push_back( troubleDesc );
      }
      else if( coverage < 100 )
      {
        std::string troubleDesc = fmt::format( "##have_less_{}_in_city_{}##", cultureCoverageDesc[ k ], coverage / 50 );
        troubles.push_back( troubleDesc );
      }
    }

    lbRatingInfo->setText( _( troubles.random() ) );
  }
}

void Ratings::Impl::checkProsperityRating()
{
  ProsperityRatingPtr prosperity = city->statistic().services.find<ProsperityRating>();

  std::string text;
  if( prosperity != 0 )
  {
    StringArray troubles;
    unsigned int currentProsperity = prosperity->value();
    if( currentProsperity == 0 )
    {
      lbRatingInfo->setText( _("##cant_calc_prosperity##") );
      return;
    }

    InfoPtr info = city->statistic().services.find<Info>();

    city::Info::Parameters current = info->lastParams();
    city::Info::Parameters lastYear = info->yearParams( 0 );

    if( current[ city::Info::prosperity ] > lastYear[ city::Info::prosperity ] ) { troubles <<  "##your_prosperity_raising##"; }

    if( prosperity->getMark( ProsperityRating::housesCap ) < 0 ) { troubles << "##bad_house_quality##"; }
    if( prosperity->getMark( ProsperityRating::haveProfit ) == 0 ) { troubles << "##lost_money_last_year##"; }
    if( prosperity->getMark( ProsperityRating::worklessPercent ) > config::workless::high ) { troubles << "##high_workless_number##"; }
    if( prosperity->getMark( ProsperityRating::workersSalary ) < 0 ) { troubles << "##workers_salary_less_then_rome##"; }
    if( prosperity->getMark( ProsperityRating::plebsPercent ) > muchPlebsPercent ) { troubles << "##much_plebs##"; }
    if( prosperity->getMark( ProsperityRating::changeValue ) == 0 )
    {
      troubles << "##no_prosperity_change##";
      troubles << "##how_to_grow_prosperity##";
    }
    if( currentProsperity > cityAmazinProsperity ) { troubles << "##amazing_prosperity_this_city##"; }
    if( current[ Info::payDiff ] > 0 ) { troubles << "##prosperity_lack_that_you_pay_less_rome##"; }


    unsigned int caesarsHelper = city->treasury().getIssueValue( econ::Issue::caesarsHelp, econ::Treasury::thisYear );
    caesarsHelper += city->treasury().getIssueValue( econ::Issue::caesarsHelp, econ::Treasury::lastYear );
    if( caesarsHelper > 0 )
    {
      troubles << "##emperor_send_money_to_you_nearest_time##";
    }

    text = troubles.empty()
                        ? "##good_prosperity##"
                        : troubles.random();


  }
  else
  {
    text = "##cant_calc_prosperity##";
  }

  lbRatingInfo->setText( _(text) );
}

void Ratings::Impl::checkPeaceRating()
{
  StringArray advices;
  MilitaryPtr ml = city->statistic().services.find<Military>();
  PeacePtr peaceRt = city->statistic().services.find<Peace>();

  if( ml.isNull() || peaceRt.isNull() || !lbRatingInfo )
  {
    Logger::warning( "!!! WARNING: checkPeaceRating failed some is null" );
    return;
  }

  unsigned int peace = city->peace();

  bool cityUnderRomeAttack = ml->haveNotification( Notification::chastener );
  bool cityUnderBarbarianAttack = ml->haveNotification( Notification::barbarian );

  if( cityUnderBarbarianAttack || cityUnderRomeAttack )
  {
    if( cityUnderRomeAttack ) { advices << "##city_under_rome_attack##"; }
    if( cityUnderBarbarianAttack ) { advices << "##city_under_barbarian_attack##"; }
  }
  else
  {
    if( ml->monthFromLastAttack() < 36 )
    {
      advices << "##province_has_peace_a_short_time##";
    }

    if( peace > peaceLongTime ) { advices << "##your_province_quiet_and_secure##"; }
    else if( peace > 80 ) { advices << "##overall_city_become_a_sleepy_province##"; }
    else if( peace > 70 ) { advices << "##its_very_peacefull_province##"; }
    else if( peace > 60 ) { advices << "##this_province_feels_peaceful##"; }
    else if( peace > peaceAverage ) { advices << "##this_lawab_province_become_very_peacefull##"; }
  }

  std::string peaceRtReason = peaceRt->reason();
  if( !peaceRtReason.empty() )
    advices << peaceRtReason;

  if( advices.empty() )
    advices << "##peace_rating_text##";

  lbRatingInfo->setText( _(advices.random()) );
}

void Ratings::Impl::checkFavourRating()
{
  StringArray problems;
  request::DispatcherPtr rd = city->statistic().services.find<request::Dispatcher>();
  InfoPtr info = city->statistic().services.find<Info>();

  Info::Parameters current = info->lastParams();
  Info::Parameters lastYear = info->yearParams( 0 );

  PlayerPtr player = city->mayor();
  world::GovernorRank rank = world::EmpireHelper::getRank( player->rank() );
  float salaryKoeff = player->salary() / (float)rank.salary;

  int brokenEmpireTax = city->treasury().getIssueValue( econ::Issue::overdueEmpireTax, econ::Treasury::lastYear );
  if( brokenEmpireTax > 0 )
  {
    int twoYearsAgoBrokenTax = city->treasury().getIssueValue( econ::Issue::overdueEmpireTax, econ::Treasury::twoYearsAgo );

    if( twoYearsAgoBrokenTax > 0 ) { problems << "##broke_empiretax_with2years_warning##"; }
    else { problems << "##broke_empiretax_warning##"; }
  }

  if( salaryKoeff >= 3.f )     { problems << "##high_salary_angers_senate##";  }
  else if( salaryKoeff > 2.5f ) { problems << "##more_salary_dispeasure_senate##";  }
  else if( salaryKoeff > 2.f ){ problems << "##try_reduce_your_high_salary##"; }
  else if( salaryKoeff > 1.5f ) { problems << "##try_reduce_your_salary##"; }
  else if( salaryKoeff > 1.f ) { problems << "##your_salary_frowned_senate##"; }

  if( current[ Info::favour ] == lastYear[ Info::favour ] )   {    problems << "##your_favour_unchanged_from_last_year##";  }
  else if( current[ Info::favour ] > lastYear[ Info::favour ] ) { problems << "##your_favour_increased_from_last_year##"; }

  if( current[ Info::favour ] < 30 ) { problems << "##your_favor_is_dropping_catch_it##"; }
  else if( current[ Info::favour ] > 90 ) { problems << "##emperoradv_caesar_has_high_respect_for_you##"; }

  if( rd.isValid() )
  {
    if( rd->haveCanceledRequest() )
    {
      problems << "##imperial_request_cance_badly_affected##";
      problems << "##request_failed##";
    }
  }

  if( problems.empty() ) { problems << "##no_favour_problem##"; }

  lbRatingInfo->setText( _(problems.random()) );
}

Ratings::Ratings(Widget* parent, int id, const PlayerCityPtr city )
  : Base( parent, city, id ), _d( new Impl )
{
  _d->city = city;
  setupUI( ":/gui/ratingsadv.gui" );

  INIT_WIDGET_FROM_UI( Label*, lbNeedPopulation )
  GET_DWIDGET_FROM_UI( _d, lbRatingInfo )

  const city::VictoryConditions& targets = city->victoryConditions();

  if( lbNeedPopulation )
  {
    std::string text = fmt::format( "{0} {1} ({2} {3}",
                                    _("##population##"), city->states().population,
                                    targets.needPopulation(), ("##need_population##")  );
    lbNeedPopulation->setText( text );
  }

  GET_DWIDGET_FROM_UI( _d, btnCulture )
  if( _d->btnCulture )
  {
    _d->btnCulture->setTarget( targets.needCulture() );
    _d->btnCulture->setValue( _d->city->culture() );
    _d->updateColumn( _d->btnCulture->relativeRect().center(), _d->city->culture() );
  }
  CONNECT( _d->btnCulture, onClicked(), _d.data(), Impl::checkCultureRating );

  GET_DWIDGET_FROM_UI( _d, btnProsperity )
  if( _d->btnProsperity )
  {
    _d->btnProsperity->setValue( _d->city->prosperity() );
    _d->btnProsperity->setTarget( targets.needProsperity() );
    _d->updateColumn( _d->btnProsperity->relativeRect().center(), _d->city->prosperity() );
  }
  CONNECT( _d->btnProsperity, onClicked(), _d.data(), Impl::checkProsperityRating );

  GET_DWIDGET_FROM_UI( _d, btnPeace )
  if( _d->btnPeace )
  {
    _d->btnPeace->setValue( _d->city->peace() );
    _d->btnPeace->setTarget( targets.needPeace() );
    _d->updateColumn( _d->btnPeace->relativeRect().center(), _d->city->peace() );
  }
  CONNECT( _d->btnPeace, onClicked(), _d.data(), Impl::checkPeaceRating );

  GET_DWIDGET_FROM_UI( _d, btnFavour )
  if( _d->btnFavour )
  {
    _d->btnFavour->setValue( _d->city->favour() );
    _d->btnFavour->setTarget( targets.needFavour() );
    _d->updateColumn( _d->btnFavour->relativeRect().center(), _d->city->favour() );
  }
  CONNECT( _d->btnFavour, onClicked(), _d.data(), Impl::checkFavourRating );

  _d->btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  CONNECT( _d->btnHelp, onClicked(), this, Ratings::_showHelp );
}

void Ratings::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );

  painter.draw( _d->columns, absoluteRect().lefttop(), &absoluteClippingRectRef() );
}

void Ratings::_showHelp() { DictionaryWindow::show( this, "ratings_advisor" ); }

}

}//end namespace gui
