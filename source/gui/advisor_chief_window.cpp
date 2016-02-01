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

class InfomationRow : public PushButton
{
public:
  InfomationRow( Widget* parent, const std::string& title, const Rect& rectangle )
    : PushButton( parent, rectangle, "", -1, false, PushButton::noBackground )
  {
    _title = title;
    _dfont = Font::create( FONT_2_WHITE );

    setIcon( gui::rc.panel, gui::id.chiefIcon );
    setIconOffset( { 6, 8 } );
    setFont( FONT_2 );
    setTextAlignment(align::upperLeft, align::center);

    setTextOffset( Point( 255, 0) );
    Decorator::draw( _border, Rect( 0, 0, width(), height() ), Decorator::brownBorder );
  }

  virtual void draw(Engine &painter)
  {
    PushButton::draw( painter );

    if( _state() == stHovered )
      painter.draw( _border, absoluteRect().lefttop(), &absoluteClippingRectRef() );
  }

  void setReasons( const ColoredStrings& reasons )
  {
    _reasons = reasons;
    auto r = _reasons.random();
    setFont( FONT_2, r.color );
    setText( _( r ) );
  }

protected:
  virtual void _updateTexture()
  {
    PushButton::_updateTexture();

    canvasDraw( _(_title), Point( 20, 0), _dfont );
  }

  Font _dfont;
  std::string _title;
  ColoredStrings _reasons;
  Pictures _border;
};

class Chief::Impl
{
public:  
  class InformationRows
     : public std::map<Advice::Type,InfomationRow*>
  {
  public:
    Point startPoint = { 22, 60 };
    int relHeight = 27;
  };

  PlayerCityPtr city;
  InformationRows rows;

public:
  void drawReportRow(Advice::Type, const ColoredStrings& strings);
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

Chief::Chief(PlayerCityPtr city, Widget* parent)
  : Base( parent, city, advisor::main ), __INIT_IMPL( Chief )
{
  __D_REF(_d, Chief )
  Base::setupUI( ":/gui/chiefadv.gui" );

  WidgetClose::insertTo( this );  

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

void Chief::setupUI(const VariantMap& ui)
{
  Base::setupUI( ui );
}

void Chief::Impl::initRows( Widget* parent, int width )
{
  Rect rowRect( rows.startPoint, Size( width-rows.startPoint.x()*2, rows.relHeight ) );
  for(const auto& row : Advice::row )
  {
    rows[ row.first ] = &parent->add<InfomationRow>( row.second, rowRect );
    rowRect += Point( 0, rows.relHeight );
  }
}

void Chief::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Chief::Impl::drawReportRow(Advice::Type type, const ColoredStrings& strings )
{
  auto it = rows.find( type );
  if( it != rows.end() )
    it->second->setReasons( strings );
}

void Chief::Impl::drawEmploymentState()
{
  Statistic::WorkersInfo wInfo = city->statistic().workers.details();
  int workless = city->statistic().workers.worklessPercent();
  ColoredStrings reasons;

  if( city->states().population == 0 )
  {
    reasons.addIfValid( {"##no_people_in_city##", ColorList::brown} );
  }
  else
  {
    int needWorkersNumber = wInfo.need - wInfo.current;
    if( needWorkersNumber > 10 )
    {
      reasons.addIfValid( { fmt::format( "{} {}", _("##advchief_needworkers##"), needWorkersNumber ),
                            ColorList::brown } );
    }
    else if( workless > bigWorklessPercent )
    {
      reasons.addIfValid( { fmt::format( "{} {}%", _("##advchief_workless##"), workless ),
                            ColorList::brown } );
    }
    else
      reasons.addIfValid( { "##advchief_employers_ok##", ColorList::black } );
  }

  drawReportRow( Advice::employers, reasons );
}

void Chief::Impl::drawProfitState()
{
  ColoredStrings reasons;
  int profit = city->treasury().profit();
  std::string prefix = (profit >= 0 ? "##advchief_haveprofit##" : "##advchief_havedeficit##");
  NColor textColor = profit > 0 ? ColorList::black : ColorList::brown;

  reasons.addIfValid( { _(prefix) + std::string(" ") + utils::i2str( profit ),
                        textColor } );

  drawReportRow( Advice::profit, reasons);
}

void Chief::Impl::drawMigrationState()
{
  ColoredStrings reasons;
  MigrationPtr migration = city->statistic().services.find<Migration>();

  if( migration.isValid() )
  {
    reasons.addIfValid( { migration->reason(), ColorList::black } );
  }
  else
  {
    reasons.addIfValid( { "##migration_unknown_reason##", ColorList::red } );
  }

  drawReportRow( Advice::migration, reasons );
}

void Chief::Impl::drawFoodStockState()
{   
  bool romeSendWheat = city->statistic().goods.isRomeSend( good::wheat );

  ColoredStrings reasons;
  if( romeSendWheat )
  {
    reasons.addIfValid( { "##rome_send_wheat_to_city##",
                          ColorList::green } );
  }
  else
  {
    InfoPtr info = city->statistic().services.find<Info>();

    if( info.isValid() )
    {
      std::string text;
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

      reasons.addIfValid( { text, ColorList::black } );
    }
  }

  if( reasons.empty() )
  {
    reasons.addIfValid( { "##food_stock_unknown_reason##", ColorList::red } );
  }

  drawReportRow( Advice::foodStock, reasons );
}

void Chief::Impl::drawFoodConsumption()
{
  std::string text;
  NColor color = ColorList::black;
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

  ColoredStrings reasons;
  reasons.addIfValid( { text, color } );
  drawReportRow( Advice::foodConsumption, reasons );
}

void Chief::Impl::drawMilitary()
{
  ColoredStrings reasons;
  MilitaryPtr military = city->statistic().services.find<Military>();
  bool isBesieged = false;

  if( military.isValid() )
  {
    isBesieged = military->value() > bigThreatValue;

    if( !isBesieged )
    {
      Notification n = military->priorityNotification();
      reasons.addIfValid( { n.desc.message, ColorList::black } );
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
          reasons.addIfValid( { threatText, ColorList::red } );
        }
        else if( minDistance <= enemyNearCityDistance )
        {
          reasons.addIfValid( { "##our_enemies_near_city##", ColorList::red } );
        }
        else
        {
          reasons.addIfValid( { "##getting_reports_about_enemies##", ColorList::yellow } );
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
      reasons.addIfValid( { "##some_soldiers_need_weapon##", ColorList::yellow } );
    }
  }

  if( reasons.empty() )
  {
    reasons.addIfValid( { "##no_warning_for_us##", ColorList::green } );
  }

  drawReportRow( Advice::military, reasons );
}

void Chief::Impl::drawCrime()
{
  ColoredStrings reasons;

  auto disorders = city->statistic().services.find<Disorder>();
  if( disorders.isValid() )
  {
    reasons.addIfValid( { disorders->reason(), ColorList::red } );
  }

  if( reasons.empty() )
  {
    reasons.addIfValid( { "##advchief_no_crime##", ColorList::green } );
  }

  drawReportRow( Advice::crime, reasons );
}

void Chief::Impl::drawHealth()
{
  ColoredStrings reasons;

  auto cityHealth = city->statistic().services.find<HealthCare>();
  if( cityHealth.isValid() )
  {
    reasons.addIfValid( { cityHealth->reason(), ColorList::red } );
  }

  if( reasons.empty() )
  {
    reasons.addIfValid( { "##advchief_health_good##", ColorList::green } );
  }

  drawReportRow( Advice::health, reasons );
}

void Chief::Impl::drawEducation()
{
  ColoredStrings reasons;

  std::map<object::Type, std::string> avTypes = { {object::school,  "##advchief_some_need_education##"},
                                                  {object::library, "##advchief_some_need_library##"  },
                                                  {object::academy, "##advchief_some_need_academy##"  } };
  for( const auto& item : avTypes )
  {
    HouseList houses = city->statistic().houses.ready4evolve( item.first );
    if( houses.size() > 0 )
      reasons.addIfValid( { item.second, ColorList::yellow } );
  }

  if( reasons.empty() )
    reasons.addIfValid( { "##advchief_education_ok##", ColorList::black } );

  drawReportRow( Advice::education, reasons );
}

void Chief::Impl::drawReligion()
{
  ColoredStrings reasons;
  ReligionPtr srvc = city->statistic().services.find<Religion>();
  if( srvc.isValid() )
  {
    reasons.addIfValid( { srvc->reason(), ColorList::black } );
  }

  if( reasons.empty() )
    reasons.addIfValid( { "##advchief_religion_unknown##", ColorList::red } );

  drawReportRow( Advice::religion, reasons );
}

void Chief::Impl::drawEntertainment()
{
  ColoredStrings reasons;

  auto festivals = city->statistic().services.find<Festival>();
  if( festivals.isValid() )
  {
    int monthFromLastFestival = festivals->last().monthsTo( game::Date::current() );
    if( monthFromLastFestival > DateTime::monthsInYear / 2 )
    {
      reasons.addIfValid( { "##citizens_grumble_lack_festivals_held##", ColorList::yellow } );
    }
  }

  auto cultures = city->statistic().services.find<CultureRating>();
  if( cultures.isValid() )
  {
    int theaterCoverage = cultures->coverage( CultureRating::covTheatres );
    if( theaterCoverage >= serviceAwesomeCoverage )
    {
      reasons.addIfValid( { "##current_play_runs_for_another##", ColorList::yellow } );
    }
  }

  int hippodromeCoverage = city->statistic().entertainment.coverage( Service::hippodrome );
  if( hippodromeCoverage >= serviceAwesomeCoverage )
  {
    reasons.addIfValid( { "##current_races_runs_for_another##", ColorList::yellow } );
  }

  drawReportRow( Advice::entertainment, reasons );
}

void Chief::Impl::drawSentiment()
{
  SentimentPtr sentiment = city->statistic().services.find<Sentiment>();

  ColoredStrings reasons;

  if( sentiment.isValid() )
  {
    reasons.addIfValid( { sentiment->reason(), ColorList::black } );
  }
  else
  {
    reasons.addIfValid( { "##unknown_sentiment_reason##", ColorList::red } );
  }

  drawReportRow( Advice::sentiment, reasons );
}

}//end namespace advisorwnd

}//end namespace gui
