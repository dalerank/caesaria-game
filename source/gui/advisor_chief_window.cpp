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
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "game/enums.hpp"
#include "city/helper.hpp"
#include "objects/house.hpp"
#include "core/color.hpp"
#include "gui/texturedbutton.hpp"
#include "city/funds.hpp"
#include "objects/barracks.hpp"
#include "objects/house_level.hpp"
#include "objects/constants.hpp"
#include "city/migration.hpp"
#include "city/statistic.hpp"
#include "dictionary.hpp"
#include "city/cityservice_info.hpp"
#include "widgetescapecloser.hpp"
#include "city/cityservice_military.hpp"
#include "city/cityservice_disorder.hpp"
#include "city/cityservice_health.hpp"
#include "city/cityservice_festival.hpp"
#include "city/goods_updater.hpp"
#include "city/sentiment.hpp"
#include "world/barbarian.hpp"
#include "game/gamedate.hpp"
#include "city/cityservice_culture.hpp"
#include "world/romechastenerarmy.hpp"
#include "world/empire.hpp"
#include "core/logger.hpp"

using namespace constants;
using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
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

class InfomationRow : public Label
{
public:
  InfomationRow( Widget* parent, const std::string& title, const Rect& rectangle )
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

class AdvisorChief::Impl
{
public:  
  PlayerCityPtr city;
  std::vector<InfomationRow*> rows;

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

AdvisorChief::AdvisorChief(PlayerCityPtr city, Widget* parent, int id )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), __INIT_IMPL( AdvisorChief )
{
  __D_IMPL(_d, AdvisorChief )
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

  TexturedButton* btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  CONNECT( btnHelp, onClicked(), this, AdvisorChief::_showHelp );
}

void AdvisorChief::Impl::initRows( Widget* parent, int width )
{
  Point startPoint( 20, 60 );
  Point offset( 0, 27 );

  for( int i=0; i < atCount; i++ )
  {
    rows.push_back( new InfomationRow( parent, titles[i], Rect( startPoint + offset * i, Size( width, offset.y() ) ) ) );
  }
}

void AdvisorChief::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void AdvisorChief::_showHelp()
{
  DictionaryWindow::show( this, "advisor_chief" );
}

void AdvisorChief::Impl::drawReportRow( AdviceType type, std::string text, NColor color=DefaultColors::black )
{
  if( type < atCount )
  {
    InfomationRow* row = rows[ type ];
    Font font = row->font();
    font.setColor( color );
    row->setFont( font );
    row->setText( text );
  }
}

void AdvisorChief::Impl::drawEmploymentState()
{
  int currentWorkers, maxWorkers;
  statistic::getWorkersNumber( city, currentWorkers, maxWorkers );
  int workless = statistic::getWorklessPercent( city );
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
      text = utils::format( 0xff, "%s %d", _("##advchief_needworkers##"), needWorkersNumber );
      color = DefaultColors::brown;
    }
    else if( workless > 10 )
    {
      text = utils::format( 0xff, "%s %d%%", _("##advchief_workless##"), workless );
      color = DefaultColors::brown;
    }
    else { text = _("##advchief_employers_ok##");  }
  }

  drawReportRow( atEmployers, text, color );
}

void AdvisorChief::Impl::drawProfitState()
{
  std::string text;
  int profit = city->funds().profit();
  if( profit >= 0 )  {    text = utils::format( 0xff, "%s %d", _("##advchief_haveprofit##"), profit );  }
  else  {    text = utils::format( 0xff, "%s %d", _("##advchief_havedeficit##"), profit );  }

  drawReportRow( profitState, text,
                 profit > 0 ? DefaultColors::black : DefaultColors::brown );
}

void AdvisorChief::Impl::drawMigrationState()
{
  SmartPtr<city::Migration> migration = ptr_cast<city::Migration>( city->findService( city::Migration::defaultName() ) );

  std::string text = _("##migration_unknown_reason##");
  if( migration.isValid() )
  {
    text = migration->reason();
  }

  drawReportRow( migrationState, _( text ) );
}

void AdvisorChief::Impl::drawFoodStockState()
{ 
  SmartList<city::GoodsUpdater> goodsUpdaters;
  goodsUpdaters << city->services();

  bool romeSendWheat = false;
  foreach( it, goodsUpdaters )
  {
    if( (*it)->goodType() == good::wheat )
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
            text = utils::format( 0xff, "%s %d %s", _("##have_food_for##"), monthWithFood, _("##months##") );
        }
      }
    }
  }

  drawReportRow( foodStockState, text );
}

void AdvisorChief::Impl::drawFoodConsumption()
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

void AdvisorChief::Impl::drawMilitary()
{
  StringArray reasons;
  city::MilitaryPtr mil;
  mil << city->findService( city::Military::defaultName() );
  bool isBesieged = false;

  if( mil.isValid() )
  {
    isBesieged = mil->threatValue() > 100;

    if( !isBesieged )
    {
      city::Military::Notification n = mil->priorityNotification();          
      reasons << n.message;
    }    
  }

  if( reasons.empty() )
  {
    world::ObjectList objs = city->empire()->findObjects( city->location(), 200 );   

    if( !objs.empty() )
    {
      int minDistance = 999;
      world::ObjectPtr maxThreat;
      foreach( i, objs )
      {
        if( is_kind_of<world::Barbarian>( *i ) ||
            is_kind_of<world::RomeChastenerArmy>( *i ) )
        {
          int distance = city->location().distanceTo( (*i)->location() );
          if( minDistance > distance )
          {
            maxThreat = *i;
            minDistance = distance;
          }
        }
      }

      if( maxThreat.isValid() )
      {
        if( minDistance <= 40 )
        {
          std::string threatText = utils::format( 0xff, "##%s_troops_at_our_gates##", maxThreat->type().c_str() );
          reasons << threatText;
        }
        else if( minDistance <= 100 )
        {
          reasons << "##our_enemies_near_city##";
        }
        else
        {
          reasons << "##getting_reports_about_enemies##";
        }
      }
    }
  }

  if( reasons.empty() )
  {
    city::Helper helper( city );

    BarracksList barracks = helper.find<Barracks>( objects::barracks );

    bool needWeapons = false;
    foreach( it, barracks )
    {
      if( (*it)->isNeedWeapons() )
      {
        needWeapons = true;
        break;
      }
    }

    if( needWeapons )
    {
      reasons << "##some_soldiers_need_weapon##";
    }
  }

  if( reasons.empty() )
  {
    reasons << "##no_warning_for_us##";
  }

  drawReportRow( atMilitary, _(reasons.random()) );
}

void AdvisorChief::Impl::drawCrime()
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

void AdvisorChief::Impl::drawHealth()
{
  std::string text;

  city::HealthCarePtr cityHealth;
  cityHealth << city->findService( city::HealthCare::defaultName() );
  if( cityHealth.isValid() )
  {
    text = cityHealth->reason();
  }

  text = text.empty() ? "##advchief_health_good##" : text;

  drawReportRow( atHealth, _(text));
}

void AdvisorChief::Impl::drawEducation()
{
  std::string text;

  StringArray reasons;
  int avTypes[] = { objects::school, objects::library, objects::academy, objects::unknown };
  std::string avReasons[] = { "##advchief_some_need_education##", "##advchief_some_need_library##",
                              "##advchief_some_need_academy##", "" };

  for( int i=0; avTypes[ i ] != objects::unknown; i++ )
  {
    std::set<int> availableTypes;
    availableTypes.insert( avTypes[ i ] );

    HouseList houses = statistic::getEvolveHouseReadyBy( city, availableTypes );
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

void AdvisorChief::Impl::drawReligion()
{
  std::string text;
  drawReportRow( atReligion, text );
}

void AdvisorChief::Impl::drawEntertainment()
{
  StringArray reasons;

  city::FestivalPtr srvc;
  srvc << city->findService( city::Festival::defaultName() );

  city::CultureRatingPtr cltr;
  cltr << city->findService( city::CultureRating::defaultName() );

  if( srvc.isValid() )
  {
    int monthFromLastFestival = srvc->lastFestivalDate().monthsTo( game::Date::current() );
    if( monthFromLastFestival > 6 )
    {
      reasons << "##citizens_grumble_lack_festivals_held##";
    }
  }

  if( cltr.isValid() )
  {
    int theaterCoverage = cltr->coverage( city::CultureRating::covTheatres );
    if( theaterCoverage >= 100 )
    {
      reasons << "##current_play_runs_for_another##";
    }
  }

  int hippodromeCoverage = statistic::getEntertainmentCoverage( city, Service::hippodrome );
  if( hippodromeCoverage >= 100 )
  {
    reasons << "##current_races_runs_for_another##";
  }

  drawReportRow( atEntertainment, _( reasons.random() ) );
}

void AdvisorChief::Impl::drawSentiment()
{
  city::SentimentPtr st;
  st << city->findService( city::Sentiment::defaultName() );

  std::string text = st.isValid()
                     ? st->reason()
                     : "##unknown_sentiment_reason##";

  drawReportRow( atSentiment, text );
}

}//end namespace advisorwnd

}//end namespace gui
