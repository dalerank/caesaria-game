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

#include "advisor_chief_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "game/enums.hpp"
#include "city/city.hpp"
#include "objects/house.hpp"
#include "core/color.hpp"
#include "gui/texturedbutton.hpp"
#include "city/funds.hpp"
#include "objects/house_level.hpp"
#include "objects/constants.hpp"
#include "city/migration.hpp"
#include "city/statistic.hpp"
#include "city/cityservice_info.hpp"
#include "widgetescapecloser.hpp"
#include "city/cityservice_military.hpp"
#include "city/cityservice_disorder.hpp"
#include "city/cityservice_health.hpp"
#include "city/goods_updater.hpp"
#include "city/sentiment.hpp"
#include "world/barbarian.hpp"
#include "world/romechastenerarmy.hpp"
#include "world/empire.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

typedef enum { atEmployers=0, profitState,
               migrationState, foodStockState,
               foodConsumption,
               atMilitary, atCrime,
               atHealth, atEducation,
               atReligion, atEntertainment,
               atSentiment,
               atCount } AdviceType;

static const std::string titles[atCount] = {
  "##advchief_employment##",
  "##advchief_finance##",
  "##advchief_migration##",
  "##advchief_food_stocks##",
  "##advchief_food_consumption##",
  "##advchief_military##",
  "##advchief_crime##",
  "##advchief_health##",
  "##advchief_education##",
  "##advchief_religion##",
  "##advchief_entertainment##",
  "##advchief_sentiment##"
};

class AdvisorChiefWindowRow : public Label
{
public:
  AdvisorChiefWindowRow( Widget* parent, const std::string& title, const Rect& rectangle )
    : Label( parent, rectangle )
  {
    _title = title;

    setIcon( Picture::load( ResourceGroup::panelBackground, 48 ), Point( 5, 5 ) );
    setFont( Font::create( FONT_2 ) );

    setTextOffset( Point( 255, 0) );
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    Font font = Font::create( FONT_2_WHITE );
    font.draw( *_textPictureRef(), _(_title), Point( 20, 0), true );
  }

  std::string _title;
};

class AdvisorChiefWindow::Impl
{
public:  
  PlayerCityPtr city;
  std::vector<AdvisorChiefWindowRow*> rows;

  TexturedButton* btnHelp;

  void drawReportRow( AdviceType, std::string text, NColor color );

  void drawEmploymentState();
  void drawProfitState();
  void drawMigrationState();
  void drawFoodStockState();
  void drawFoodConsumption();
  void drawMilitary();
  void drawCrime();
  void drawHealth();
  void drawEducation();
  void drawReligion();
  void drawEntertainment();
  void drawSentiment();

  void initRows( Widget* parent, int width );
};

AdvisorChiefWindow::AdvisorChiefWindow(PlayerCityPtr city, Widget* parent, int id )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), __INIT_IMPL( AdvisorChiefWindow )
{
  __D_IMPL(_d, AdvisorChiefWindow)
  setupUI( ":/gui/chiefadv.gui" );

  _d->city = city;
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  WidgetEscapeCloser::insertTo( this );  

  _d->initRows( this, width() );

  _d->drawEmploymentState();
  _d->drawProfitState();
  _d->drawMigrationState();
  _d->drawFoodStockState();
  _d->drawFoodConsumption();
  _d->drawMilitary();
  _d->drawCrime();
  _d->drawHealth();
  _d->drawEducation();
  _d->drawReligion();
  _d->drawEntertainment();
  _d->drawSentiment();
}

void AdvisorChiefWindow::Impl::initRows( Widget* parent, int width )
{
  Point startPoint( 20, 60 );
  Point offset( 0, 27 );

  for( int i=0; i < atCount; i++ )
  {
    rows.push_back( new AdvisorChiefWindowRow( parent, titles[i], Rect( startPoint + offset * i, Size( width, offset.y() ) ) ) );
  }
}

void AdvisorChiefWindow::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void AdvisorChiefWindow::Impl::drawReportRow( AdviceType type, std::string text, NColor color=DefaultColors::black )
{
  if( type < atCount )
  {
    AdvisorChiefWindowRow* row = rows[ type ];
    Font font = row->font();
    font.setColor( color );
    row->setFont( font );
    row->setText( text );
  }
}

void AdvisorChiefWindow::Impl::drawEmploymentState()
{
  int currentWorkers, maxWorkers;
  city::Statistic::getWorkersNumber( city, currentWorkers, maxWorkers );
  int workless = city::Statistic::getWorklessPercent( city );
  std::string text;
  NColor color = DefaultColors::black;

  if( city->population() == 0 )
  {
    text = _("##no_people_in_city##");
    color =  DefaultColors::brown;
  }
  else
  {
    int needWorkersNumber = maxWorkers - currentWorkers;
    if( needWorkersNumber > 10 )
    {
      text = StringHelper::format( 0xff, "%s %d", _("##advchief_needworkers##"), needWorkersNumber );
      color = DefaultColors::brown;
    }
    else if( workless > 10 )
    {
      text = StringHelper::format( 0xff, "%s %d%%", _("##advchief_workless##"), workless );
      color = DefaultColors::brown;
    }
    else { text = _("##advchief_employers_ok##");  }
  }

  drawReportRow( atEmployers, text, color );
}

void AdvisorChiefWindow::Impl::drawProfitState()
{
  std::string text;
  int profit = city->funds().profit();
  if( profit >= 0 )  {    text = StringHelper::format( 0xff, "%s %d", _("##advchief_haveprofit##"), profit );  }
  else  {    text = StringHelper::format( 0xff, "%s %d", _("##advchief_havedeficit##"), profit );  }

  drawReportRow( profitState, text,
                 profit > 0 ? DefaultColors::black : DefaultColors::brown );
}

void AdvisorChiefWindow::Impl::drawMigrationState()
{
  SmartPtr<city::Migration> migration = ptr_cast<city::Migration>( city->findService( city::Migration::defaultName() ) );

  std::string text = _("##migration_unknown_reason##");
  if( migration.isValid() )
  {
    text = migration->reason( city );
  }

  drawReportRow( migrationState, _( text ) );
}

void AdvisorChiefWindow::Impl::drawFoodStockState()
{ 
  SmartList<city::GoodsUpdater> goodsUpdaters;
  goodsUpdaters << city->services();

  bool romeSendWheat = false;
  foreach( it, goodsUpdaters )
  {
    if( (*it)->goodType() == Good::wheat )
    {
      romeSendWheat = true;
    }
  }

  std::string text = _("##food_stock_unknown_reason##");
  if( romeSendWheat )
  {
    text = "##rome_send_wheat_to_city##";
  }
  else
  {
    city::InfoPtr info;
    info << city->findService( city::Info::defaultName() );

    if( info.isValid() )
    {
      city::Info::Parameters lastMonth = info->lastParams();
      city::Info::Parameters prevMonth = info->params( 1 );

      if( lastMonth[ city::Info::foodStock ] < prevMonth[ city::Info::foodStock ] )
      {
        text = "##no_food_stored_last_month##";
      }
      else
      {
        int monthWithFood = lastMonth[ city::Info::monthWithFood ];
        switch( monthWithFood )
        {
          case 0: text = "##have_no_food_on_next_month##"; break;
          case 1: text = "##small_food_on_next_month##"; break;
          case 2: text = "##some_food_on_next_month##"; break;
          case 3: text = "##our_foods_level_are_low##"; break;

          default:
            text = StringHelper::format( 0xff, "%s %d %s", _("##have_food_for##"), monthWithFood, _("##months##") );
        }
      }
    }
  }

  drawReportRow( foodStockState, text );
}

void AdvisorChiefWindow::Impl::drawFoodConsumption()
{
  std::string text;
  city::InfoPtr info;
  info << city->findService( city::Info::defaultName() );

  int fk = info->lastParams()[ city::Info::foodKoeff ];

  if( fk < -4 )
  {
    text = "##we_eat_much_then_produce##";
  }
  else if( fk > 2 )
  {
    text = "##we_produce_much_than_eat##";
  }
  else
  {
    switch( info->lastParams()[ city::Info::foodKoeff ] )
    {
    case -3: text = "##we_eat_more_thie_produce##"; break;
    case -2: text = "##we_eat_some_then_produce##"; break;
    case -1: text= "##we_produce_less_than_eat##"; break;
    case 0: text = "##we_noproduce_food##"; break;
    case 1: text = "##we_produce_some_than_eat##"; break;
    case 2: text = "##we_produce_more_than_eat##"; break;
    }
  }

  drawReportRow( foodConsumption, _(text) );
}

void AdvisorChiefWindow::Impl::drawMilitary()
{
  StringArray reasons;
  city::MilitaryPtr mil;
  mil << city->findService( city::Military::defaultName() );

  if( mil.isValid() )
  {
    city::Military::Notification n = mil->priorityNotification();
    reasons << n.message;
  }

  world::ObjectList objs = city->empire()->findObjects( city->location(), 200 );
  foreach( i, objs )
  {
    if( is_kind_of<world::Barbarian>( *i ) ||
        is_kind_of<world::RomeChastenerArmy>( *i ) )
    {
      reasons << "##getting_reports_about_enemies##";
      break;
    }
  }

  std::string text = reasons.empty() ? "##no_warning_for_us##" : reasons.random();
  drawReportRow( atMilitary, _(text) );
}

void AdvisorChiefWindow::Impl::drawCrime()
{
  std::string text;

  city::DisorderPtr ds;
  ds << city->findService( city::Disorder::defaultName() );

  if( ds.isValid() )
  {
    text = ds->reason();
  }

  text = text.empty() ? "##advchief_no_crime##" : text;

  drawReportRow( atCrime, _(text) );
}

void AdvisorChiefWindow::Impl::drawHealth()
{
  std::string text;

  city::HealthCarePtr ds;
  ds << city->findService( city::HealthCare::defaultName() );
  if( ds.isValid() )
  {
    text = ds->reason( city );
  }

  text = text.empty() ? "##advchief_health_good##" : text;

  drawReportRow( atHealth, _(text));
}

void AdvisorChiefWindow::Impl::drawEducation()
{
  std::string text;

  StringArray reasons;
  int avTypes[] = { building::school, building::library, building::academy, building::unknown };
  std::string avReasons[] = { "##advchief_some_need_education##", "##advchief_some_need_library##",
                              "##advchief_some_need_academy##", "" };

  for( int i=0; avTypes[ i ] != building::unknown; i++ )
  {
    std::set<int> availableTypes;
    availableTypes.insert( avTypes[ i ] );

    HouseList houses = city::Statistic::getEvolveHouseReadyBy( city, availableTypes );
    if( houses.size() > 0 )
    {
      reasons << avReasons[i];
    }
  }

  text = reasons.random();

  text = text.empty()
            ? "##advchief_education_ok##"
            : text;


  drawReportRow( atEducation, _( text ) );
}

void AdvisorChiefWindow::Impl::drawReligion()
{
  std::string text;
  drawReportRow( atReligion, text );
}

void AdvisorChiefWindow::Impl::drawEntertainment()
{
  std::string text;
  drawReportRow( atEntertainment, text );
}

void AdvisorChiefWindow::Impl::drawSentiment()
{
  city::SentimentPtr st;
  st << city->findService( city::Sentiment::defaultName() );

  std::string text = st.isValid()
                     ? st->reason()
                     : "##unknown_sentiment_reason##";

  drawReportRow( atSentiment, text );
}

}//end namespace gui
