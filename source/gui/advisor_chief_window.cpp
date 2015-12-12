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
#include "core/color_list.hpp"
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

struct Advice
{
  typedef enum { employers=0, profit,
                 migration, foodStock,
                 foodConsumption,
                 military, crime,
                 health, education,
                 religion, entertainment,
                 sentiment,
                 count } Type;
  static const std::map<Type,std::string> row;
};

const std::map<Advice::Type,std::string> Advice::row = {
                                            {employers,     "##advchief_employment##"       },
                                            {profit,        "##advchief_finance##"          },
                                            {migration,     "##advchief_migration##"        },
                                            {foodStock,     "##advchief_food_stocks##"      },
                                            {foodConsumption,"##advchief_food_consumption##"},
                                            {military,      "##advchief_military##"         },
                                            {crime,         "##advchief_crime##"            },
                                            {health,        "##advchief_health##"           },
                                            {education,     "##advchief_education##"        },
                                            {religion,      "##advchief_religion##"         },
                                            {entertainment, "##advchief_entertainment##"    },
                                            {sentiment,     "##advchief_sentiment##"        }
                                          };

class InfomationRow : public Label
{
public:
  InfomationRow( Widget* parent, const std::string& title, const Rect& rectangle )
    : Label( parent, rectangle )
  {
    _title = title;
    _dfont = Font::create( FONT_2_WHITE );

    setIcon( gui::rc.panel, gui::id.chiefIcon );
    setIconOffset( Point( 5, 5 ) );
    setFont( Font::create( FONT_2 ) );

    setTextOffset( Point( 255, 0) );
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    canvasDraw( _(_title), Point( 20, 0), _dfont );
  }

  Font _dfont;
  std::string _title;
};

class Chief::Impl
{
public:  
  typedef std::map<Advice::Type,InfomationRow*> InformationTable;

  PlayerCityPtr city;
  InformationTable rows;
  TexturedButton* btnHelp;

public:
  void drawReportRow( Advice::Type, std::string text, NColor color=ColorList::black );
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
  __D_REF(_d, Chief )
  setupUI( ":/gui/chiefadv.gui" );

  WidgetEscapeCloser::insertTo( this );  

  _d.city = city;
  _d.initRows( this, width() );
  _d.drawEmploymentState();
  _d.drawProfitState();
  _d.drawMigrationState();
  _d.drawFoodStockState();
  _d.drawFoodConsumption();
  _d.drawMilitary();
  _d.drawCrime();
  _d.drawHealth();
  _d.drawEducation();
  _d.drawReligion();
  _d.drawEntertainment();
  _d.drawSentiment();

  add<HelpButton>( Point( 12, height() - 39 ), "advisor_chief" );
}

void Chief::Impl::initRows( Widget* parent, int width )
{
  Point startPoint( 20, 60 );
  Point offset( 0, 27 );

  Rect rowRect( startPoint, Size( width, offset.y() ) );
  for(const auto& row : Advice::row )
  {
    rows[ row.first ] = &parent->add<InfomationRow>( row.second, rowRect );
    rowRect += offset;
  }
}

void Chief::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Chief::Impl::drawReportRow(Advice::Type type, std::string text, NColor color )
{
  auto it = rows.find( type );
  if( it != rows.end() )
  {
    it->second->setColor( color );
    it->second->setText( text );
  }
}

void Chief::Impl::drawEmploymentState()
{
  Statistic::WorkersInfo wInfo = city->statistic().workers.details();
  int workless = city->statistic().workers.worklessPercent();
  std::string text;
  NColor color = ColorList::black;

  if( city->states().population == 0 )
  {
    text = _("##no_people_in_city##");
    color =  ColorList::brown;
  }
  else
  {
    int needWorkersNumber = wInfo.need - wInfo.current;
    if( needWorkersNumber > 10 )
    {
      text = fmt::format( "{} {}", _("##advchief_needworkers##"), needWorkersNumber );
      color = ColorList::brown;
    }
    else if( workless > bigWorklessPercent )
    {
      text = fmt::format( "{} {}%", _("##advchief_workless##"), workless );
      color = ColorList::brown;
    }
    else { text = _("##advchief_employers_ok##");  }
  }

  drawReportRow( Advice::employers, text, color );
}

void Chief::Impl::drawProfitState()
{
  std::string text;
  int profit = city->treasury().profit();
  std::string prefix = (profit >= 0 ? "##advchief_haveprofit##" : "##advchief_havedeficit##");
  text = _(prefix) + std::string(" ") + utils::i2str( profit );
  NColor textColor = profit > 0 ? ColorList::black : ColorList::brown;

  drawReportRow( Advice::profit, text, textColor );
}

void Chief::Impl::drawMigrationState()
{
  MigrationPtr migration = city->statistic().services.find<Migration>();

  std::string text = _("##migration_unknown_reason##");
  if( migration.isValid() )
  {
    text = migration->reason();
  }

  drawReportRow( Advice::migration, _( text ) );
}

void Chief::Impl::drawFoodStockState()
{   
  bool romeSendWheat = city->statistic().goods.isRomeSend( good::wheat );

  std::string text = _("##food_stock_unknown_reason##");
  if( romeSendWheat )
  {
    text = "##rome_send_wheat_to_city##";
  }
  else
  {
    InfoPtr info = city->statistic().services.find<Info>();

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
            text = fmt::format( "{} {} {}", _("##have_food_for##"), monthWithFood, _("##months##") );
        }
      }
    }
  }

  drawReportRow( Advice::foodStock, _(text) );
}

void Chief::Impl::drawFoodConsumption()
{
  std::string text;
  city::InfoPtr info = city->statistic().services.find<Info>();

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

  drawReportRow( Advice::foodConsumption, _(text) );
}

void Chief::Impl::drawMilitary()
{
  StringArray reasons;
  MilitaryPtr military = city->statistic().services.find<Military>();
  bool isBesieged = false;

  if( military.isValid() )
  {
    isBesieged = military->value() > bigThreatValue;

    if( !isBesieged )
    {
      Notification n = military->priorityNotification();
      reasons << n.desc.message;
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
          std::string threatText = fmt::format( "##{}_troops_at_our_gates##", maxThreat->type() );
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
    BarracksList barracks = city->statistic().objects.find<Barracks>();

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

  drawReportRow( Advice::military, _(reasons.random()) );
}

void Chief::Impl::drawCrime()
{
  std::string text;

  auto disorders = city->statistic().services.find<Disorder>();
  if( disorders.isValid() )
  {
    text = disorders->reason();
  }

  text = text.empty() ? "##advchief_no_crime##" : text;

  drawReportRow( Advice::crime, _(text) );
}

void Chief::Impl::drawHealth()
{
  std::string text;

  auto cityHealth = city->statistic().services.find<HealthCare>();
  if( cityHealth.isValid() )
  {
    text = cityHealth->reason();
  }

  text = text.empty() ? "##advchief_health_good##" : text;

  drawReportRow( Advice::health, _(text));
}

void Chief::Impl::drawEducation()
{
  std::string text;

  StringArray reasons;
  std::map<object::Type, std::string> avTypes = { {object::school,  "##advchief_some_need_education##"},
                                                  {object::library, "##advchief_some_need_library##"  },
                                                  {object::academy, "##advchief_some_need_academy##"  } };
  for( const auto& item : avTypes )
  {
    HouseList houses = city->statistic().houses.ready4evolve( item.first );
    if( houses.size() > 0 )
      reasons << item.second;
  }

  text = reasons.random();

  if( text.empty() )
    text = "##advchief_education_ok##";

  drawReportRow( Advice::education, _( text ) );
}

void Chief::Impl::drawReligion()
{
  std::string text = "##advchief_religion_unknown##";
  ReligionPtr srvc = city->statistic().services.find<Religion>();
  if( srvc.isValid() )
  {

  }
  drawReportRow( Advice::religion, text );
}

void Chief::Impl::drawEntertainment()
{
  StringArray reasons;

  auto festivals = city->statistic().services.find<Festival>();
  if( festivals.isValid() )
  {
    int monthFromLastFestival = festivals->last().monthsTo( game::Date::current() );
    if( monthFromLastFestival > DateTime::monthsInYear / 2 )
    {
      reasons << "##citizens_grumble_lack_festivals_held##";
    }
  }

  auto cultures = city->statistic().services.find<CultureRating>();
  if( cultures.isValid() )
  {
    int theaterCoverage = cultures->coverage( CultureRating::covTheatres );
    if( theaterCoverage >= serviceAwesomeCoverage )
    {
      reasons << "##current_play_runs_for_another##";
    }
  }

  int hippodromeCoverage = city->statistic().entertainment.coverage( Service::hippodrome );
  if( hippodromeCoverage >= serviceAwesomeCoverage )
  {
    reasons << "##current_races_runs_for_another##";
  }

  drawReportRow( Advice::entertainment, _( reasons.random() ) );
}

void Chief::Impl::drawSentiment()
{
  SentimentPtr sentiment = city->statistic().services.find<Sentiment>();

  std::string text = sentiment.isValid()
                     ? sentiment->reason()
                     : "##unknown_sentiment_reason##";

  drawReportRow( Advice::sentiment, text );
}

}//end namespace advisorwnd

}//end namespace gui
