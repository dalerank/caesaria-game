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

#include "advisor_chief_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "city/statistic.hpp"
#include "objects/house.hpp"
#include "core/color.hpp"
#include "gui/texturedbutton.hpp"
#include "game/funds.hpp"
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
#include "city/cityservice_religion.hpp"
#include "city/cityservice_festival.hpp"
#include "city/goods_updater.hpp"
#include "city/sentiment.hpp"
#include "world/barbarian.hpp"
#include "game/gamedate.hpp"
#include "city/cityservice_culture.hpp"
#include "world/romechastenerarmy.hpp"
#include "world/empire.hpp"
#include "core/logger.hpp"
#include "city/states.hpp"

using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

enum { cityHavenotFood=-4, cityHavenotFoodNextMonth=0, cityHaveSmallFoodNextMonth=1,
       cityPriduceMoreFood=2,
       bigWorklessPercent=10, enemyNearCityGatesDistance=40, enemyNearCityDistance=100,
       bigThreatValue=100, serviceAwesomeCoverage=100, haveThreatDistance=200, infinteDistance=999 };

typedef enum { atEmployers=0, profitState,
               migrationState, foodStockState,
               foodConsumption,
               atMilitary, atCrime,
               atHealth, atEducation,
               atReligion, atEntertainment,
               atSentiment,
               atCount } AdviceType;

static const std::string titles[atCount] =
{
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

    Picture pic;
    pic.load( ResourceGroup::panelBackground, 48 ), Point( 5, 5 );
    setIcon( pic, Point( 5, 5 ) );
    setFont( Font::create( FONT_2 ) );

    setTextOffset( Point( 255, 0) );
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    Font font = Font::create( FONT_2_WHITE );
    font.draw( _textPicture(), _(_title), Point( 20, 0), true );
  }

  std::string _title;
};

class Chief::Impl
{
public:  
  typedef std::vector<InfomationRow*> InformationTable;

  PlayerCityPtr city;
  InformationTable rows;
  TexturedButton* btnHelp;

public:
  void drawReportRow( AdviceType, std::string text, NColor color=DefaultColors::black );
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

Chief::Chief(PlayerCityPtr city, Widget* parent, int id )
  : Base( parent, city ), __INIT_IMPL( Chief )
{
  __D_IMPL(_d, Chief )
  setupUI( ":/gui/chiefadv.gui" );

  WidgetEscapeCloser::insertTo( this );  

  _d->city = city;
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
  CONNECT( btnHelp, onClicked(), this, Chief::_showHelp );
}

void Chief::Impl::initRows( Widget* parent, int width )
{
  Point startPoint( 20, 60 );
  Point offset( 0, 27 );

  for( int i=0; i < atCount; i++ )
  {
    rows.push_back( new InfomationRow( parent, titles[i], Rect( startPoint + offset * i, Size( width, offset.y() ) ) ) );
  }
}

void Chief::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Chief::_showHelp()
{
  DictionaryWindow::show( this, "advisor_chief" );
}

void Chief::Impl::drawReportRow( AdviceType type, std::string text, NColor color )
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

void Chief::Impl::drawEmploymentState()
{
  statistic::WorkersInfo wInfo = statistic::getWorkersNumber( city );
  int workless = statistic::getWorklessPercent( city );
  std::string text;
  NColor color = DefaultColors::black;

  if( city->states().population == 0 )
  {
    text = _("##no_people_in_city##");
    color =  DefaultColors::brown;
  }
  else
  {
    int needWorkersNumber = wInfo.need - wInfo.current;
    if( needWorkersNumber > 10 )
    {
      text = utils::format( 0xff, "%s %d", _("##advchief_needworkers##"), needWorkersNumber );
      color = DefaultColors::brown;
    }
    else if( workless > bigWorklessPercent )
    {
      text = utils::format( 0xff, "%s %d%%", _("##advchief_workless##"), workless );
      color = DefaultColors::brown;
    }
    else { text = _("##advchief_employers_ok##");  }
  }

  drawReportRow( atEmployers, text, color );
}

void Chief::Impl::drawProfitState()
{
  std::string text;
  int profit = city->treasury().profit();
  std::string prefix = (profit >= 0 ? "##advchief_haveprofit##" : "##advchief_havedeficit##");
  text = _(prefix) + std::string(" ") + utils::i2str( profit );
  NColor textColor = profit > 0 ? DefaultColors::black : DefaultColors::brown;

  drawReportRow( profitState, text, textColor );
}

void Chief::Impl::drawMigrationState()
{
  city::MigrationPtr migration = ptr_cast<Migration>( city->findService( Migration::defaultName() ) );

  std::string text = _("##migration_unknown_reason##");
  if( migration.isValid() )
  {
    text = migration->reason();
  }

  drawReportRow( migrationState, _( text ) );
}

void Chief::Impl::drawFoodStockState()
{ 
  SmartList<GoodsUpdater> goodsUpdaters;
  goodsUpdaters << city->services();

  bool romeSendWheat = false;
  for( auto updater : goodsUpdaters )
  {
    if( updater->goodType() == good::wheat )
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
    InfoPtr info = city::statistic::getService<Info>( city );

    if( info.isValid() )
    {
      Info::Parameters lastMonth = info->lastParams();
      Info::Parameters prevMonth = info->params( Info::lastMonth );

      if( lastMonth[ Info::foodStock ] < prevMonth[ Info::foodStock ] )
      {
        text = "##no_food_stored_last_month##";
      }
      else
      {
        int monthWithFood = lastMonth[ Info::monthWithFood ];
        switch( monthWithFood )
        {
          case cityHavenotFoodNextMonth: text = "##have_no_food_on_next_month##"; break;
          case cityHaveSmallFoodNextMonth: text = "##small_food_on_next_month##"; break;
          case 2: text = "##some_food_on_next_month##"; break;
          case 3: text = "##our_foods_level_are_low##"; break;

          default:
            text = utils::format( 0xff, "%s %d %s", _("##have_food_for##"), monthWithFood, _("##months##") );
        }
      }
    }
  }

  drawReportRow( foodStockState, _(text) );
}

void Chief::Impl::drawFoodConsumption()
{
  std::string text;
  city::InfoPtr info = statistic::getService<Info>( city );

  int fk = info->lastParams()[ Info::foodKoeff ];

  if( fk < cityHavenotFood )
  {
    text = "##we_eat_much_then_produce##";
  }
  else if( fk > cityPriduceMoreFood )
  {
    text = "##we_produce_much_than_eat##";
  }
  else
  {
    switch( info->lastParams()[ Info::foodKoeff ] )
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

void Chief::Impl::drawMilitary()
{
  StringArray reasons;
  MilitaryPtr mil = statistic::getService<Military>( city );
  bool isBesieged = false;

  if( mil.isValid() )
  {
    isBesieged = mil->threatValue() > bigThreatValue;

    if( !isBesieged )
    {
      Notification n = mil->priorityNotification();
      reasons << n.message;
    }    
  }

  if( reasons.empty() )
  {
    world::ObjectList objs = city->empire()->findObjects( city->location(), haveThreatDistance );

    if( !objs.empty() )
    {
      int minDistance = infinteDistance;
      world::ObjectPtr maxThreat;
      for( auto obj : objs )
      {
        if( is_kind_of<world::Barbarian>( obj ) ||
            is_kind_of<world::RomeChastenerArmy>( obj ) )
        {
          int distance = city->location().distanceTo( obj->location() );
          if( minDistance > distance )
          {
            maxThreat = obj;
            minDistance = distance;
          }
        }
      }

      if( maxThreat.isValid() )
      {
        if( minDistance <= enemyNearCityGatesDistance )
        {
          std::string threatText = utils::format( 0xff, "##%s_troops_at_our_gates##", maxThreat->type().c_str() );
          reasons << threatText;
        }
        else if( minDistance <= enemyNearCityDistance )
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
    BarracksList barracks = statistic::getObjects<Barracks>( city, object::barracks );

    bool needWeapons = false;
    for( auto barrack : barracks )
    {
      if( barrack->isNeedWeapons() )
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

void Chief::Impl::drawCrime()
{
  std::string text;

  DisorderPtr ds = statistic::getService<Disorder>( city );
  if( ds.isValid() )
  {
    text = ds->reason();
  }

  text = text.empty() ? "##advchief_no_crime##" : text;

  drawReportRow( atCrime, _(text) );
}

void Chief::Impl::drawHealth()
{
  std::string text;

  HealthCarePtr cityHealth = statistic::getService<HealthCare>( city );
  if( cityHealth.isValid() )
  {
    text = cityHealth->reason();
  }

  text = text.empty() ? "##advchief_health_good##" : text;

  drawReportRow( atHealth, _(text));
}

void Chief::Impl::drawEducation()
{
  std::string text;

  StringArray reasons;
  object::Type avTypes[] = { object::school, object::library, object::academy, object::unknown };
  std::string avReasons[] = { "##advchief_some_need_education##", "##advchief_some_need_library##",
                              "##advchief_some_need_academy##", "" };

  for( int i=0; avTypes[i] != object::unknown; i++ )
  {
    HouseList houses = statistic::getEvolveHouseReadyBy( city, avTypes[ i ] );
    if( houses.size() > 0 )
    {
      reasons << avReasons[i];
    }
  }

  text = reasons.random();

  if( text.empty() )
    text = "##advchief_education_ok##";

  drawReportRow( atEducation, _( text ) );
}

void Chief::Impl::drawReligion()
{
  std::string text = "##advchief_religion_unknown##";
  ReligionPtr srvc = statistic::getService<Religion>( city );
  if( srvc.isValid() )
  {

  }
  drawReportRow( atReligion, text );
}

void Chief::Impl::drawEntertainment()
{
  StringArray reasons;

  FestivalPtr srvc = statistic::getService<Festival>( city );
  if( srvc.isValid() )
  {
    int monthFromLastFestival = srvc->lastFestival().monthsTo( game::Date::current() );
    if( monthFromLastFestival > DateTime::monthsInYear / 2 )
    {
      reasons << "##citizens_grumble_lack_festivals_held##";
    }
  }

  CultureRatingPtr cltr = statistic::getService<CultureRating>( city );
  if( cltr.isValid() )
  {
    int theaterCoverage = cltr->coverage( CultureRating::covTheatres );
    if( theaterCoverage >= serviceAwesomeCoverage )
    {
      reasons << "##current_play_runs_for_another##";
    }
  }

  int hippodromeCoverage = statistic::getEntertainmentCoverage( city, Service::hippodrome );
  if( hippodromeCoverage >= serviceAwesomeCoverage )
  {
    reasons << "##current_races_runs_for_another##";
  }

  drawReportRow( atEntertainment, _( reasons.random() ) );
}

void Chief::Impl::drawSentiment()
{
  SentimentPtr sentiment = statistic::getService<Sentiment>( city );

  std::string text = sentiment.isValid()
                     ? sentiment->reason()
                     : "##unknown_sentiment_reason##";

  drawReportRow( atSentiment, text );
}

}//end namespace advisorwnd

}//end namespace gui
