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

using namespace constants;
using namespace gfx;

namespace gui
{

class AdvisorChiefWindow::Impl
{
public:
  PlayerCityPtr city;
  PictureRef background;

  TexturedButton* btnHelp;

  void drawReportRow( Point pos, std::string title, std::string text, NColor color );
  void drawEmploymentState( Point pos );
  void drawProfitState( Point pos );
  void drawMigrationState( Point pos );
  void drawFoodStockState( Point pos );
  void drawFoodConsumption( Point pos );
  void drawMilitary( Point pos );
  void drawCrime( Point pos );
  void drawHealth( Point pos );
  void drawEducation( Point pos );
  void drawReligion( Point pos );
  void drawEntertainment( Point pos );
  void drawSentiment( Point pos );
};

AdvisorChiefWindow::AdvisorChiefWindow(PlayerCityPtr city, Widget* parent, int id )
  : Widget( parent, id, Rect( 0, 0, 1, 1 ) ), __INIT_IMPL( AdvisorChiefWindow )
{
  __D_IMPL(_d, AdvisorChiefWindow)
  _d->city = city;
  setGeometry( Rect( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ),
               Size( 640, 420 ) ) );

  gui::Label* title = new gui::Label( this, Rect( 60, 10, 60 + 320, 10 + 40) );
  title->setText( _("##chief_advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( align::upperLeft, align::center );

  WidgetEscapeCloser::insertTo( this );

  _d->background.reset( Picture::create( size() ) );

  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteFrame );
  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( 20, 55, width() - 20, height() - 20 ), PictureDecorator::blackFrame);

  Picture& icon = Picture::load( ResourceGroup::panelBackground, 266 );
  _d->background->draw( icon, Point( 11, 11 ) );

  Point startPoint( 20, 60 );
  Point offset( 0, 27 );

  _d->drawEmploymentState( startPoint );
  _d->drawProfitState( startPoint + offset );
  _d->drawMigrationState( startPoint + offset * 2 );
  _d->drawFoodStockState( startPoint + offset * 3 );
  _d->drawFoodConsumption( startPoint + offset * 4 );
  _d->drawMilitary( startPoint + offset * 5 );
  _d->drawCrime( startPoint + offset * 6 );
  _d->drawHealth( startPoint + offset * 7 );
  _d->drawEducation( startPoint + offset * 8 );
  _d->drawReligion( startPoint + offset * 9 );
  _d->drawEntertainment( startPoint + offset * 10 );
  _d->drawSentiment( startPoint + offset * 11 );
}

void AdvisorChiefWindow::draw( gfx::Engine& painter )
{
  if( !isVisible() )
    return;

  __D_IMPL(_d, AdvisorChiefWindow)
  painter.draw( *_d->background, screenLeft(), screenTop() );

  Widget::draw( painter );
}

void AdvisorChiefWindow::Impl::drawReportRow(Point pos, std::string title, std::string text, NColor color=DefaultColors::black )
{
  Font font = Font::create( FONT_2_WHITE );
  Font font2 = Font::create( FONT_2 );
  font2.setColor( color );

  Picture pointPic = Picture::load( ResourceGroup::panelBackground, 48 );

  background->draw( pointPic, pos + Point( 5, 5 ) );
  font.draw( *background, title, pos + Point( 20, 0), false );
  font2.draw( *background, text, pos + Point( 255, 0), false );
}

void AdvisorChiefWindow::Impl::drawEmploymentState(Point pos)
{
  int needWorkersNumber = city::Statistic::getVacantionsNumber( city );
  int workless = city::Statistic::getWorklessPercent( city );
  std::string text;
  NColor color = DefaultColors::black;
  if( needWorkersNumber > 0 )
  {
    text = StringHelper::format( 0xff, "%s %d", _("##advchief_needworkers##"), needWorkersNumber );
    color = DefaultColors::brown;
  }
  else if( workless > 10 )
  {
    text = StringHelper::format( 0xff, "%s %d%%", _("##advchief_workless##"), workless );
    color = DefaultColors::brown;
  }
  else  {  text = _("##advchief_employers_ok##");  }

  drawReportRow( pos, _("##advchief_employment##"), text, color );
}

void AdvisorChiefWindow::Impl::drawProfitState(Point pos)
{
  std::string text;
  int profit = city->funds().profit();
  if( profit >= 0 )  {    text = StringHelper::format( 0xff, "%s %d", _("##advchief_haveprofit##"), profit );  }
  else  {    text = StringHelper::format( 0xff, "%s %d", _("##advchief_havedeficit##"), profit );  }

  drawReportRow( pos, _("##advchief_finance##"), text,
                 profit > 0 ? DefaultColors::black : DefaultColors::brown );
}

void AdvisorChiefWindow::Impl::drawMigrationState(Point pos)
{
  SmartPtr<city::Migration> migration = ptr_cast<city::Migration>( city->findService( city::Migration::getDefaultName() ) );

  std::string text = _("##migration_unknown_reason##");
  if( migration.isValid() )
  {
    text = migration->getReason();
  }

  drawReportRow( pos, _("##advchief_migration##"), _( text ) );
}

void AdvisorChiefWindow::Impl::drawFoodStockState(Point pos)
{
  city::InfoPtr info;
  info << city->findService( city::Info::getDefaultName() );

  std::string text = _("##food_stock_unknown_reason##");
  if( info.isValid() )
  {
    int monthWithFood = info->getLast().monthWithFood;
    switch( monthWithFood )
    {
      case 0: text = "##have_no_food_on_next_month##"; break;
      case 1: text = "##small_food_on_next_month##"; break;
      case 2: text = "##some_food_on_next_month##"; break;

      default:
        text = StringHelper::format( 0xff, "%s %d", _("##have_food_for##"), monthWithFood );
    }
  }

  drawReportRow( pos, _("##advchief_food_stocks##"), text );
}

void AdvisorChiefWindow::Impl::drawFoodConsumption(Point pos)
{
  std::string text;
  city::InfoPtr info;
  info << ptr_cast<city::Info>( city->findService( city::Info::getDefaultName() ));

  switch( info->getLast().foodKoeff )
  {
  case -1: text= "##we_produce_less_than_eat##"; break;
  case 0: text = "##we_noproduce_food##"; break;
  case 1: text = "##we_produce_some_than_eat##"; break;
  case 2: text = "##we_produce_more_than_eat##"; break;

  default: text = "##we_produce_much_than_eat##";
  }

  drawReportRow( pos, _("##advchief_food_consumption##"), _(text) );
}

void AdvisorChiefWindow::Impl::drawMilitary(Point pos)
{
  std::string text;
  city::MilitaryPtr mil = ptr_cast<city::Military>( city->findService( city::Military::getDefaultName() ) );
  if( mil.isValid() )
  {
    city::Military::Notification n = mil->getPriorityNotification();
    text = n.message;
  }
  if( text.empty() )
  {
    text = "##no_warning_for_us##";
  }
  drawReportRow( pos, _("##advchief_military##"), text );
}

void AdvisorChiefWindow::Impl::drawCrime(Point pos)
{
  std::string text;
  drawReportRow( pos, _("##advchief_crime##"), text );
}

void AdvisorChiefWindow::Impl::drawHealth(Point pos)
{
  std::string text;
  drawReportRow( pos, _("##advchief_health##"), text );
}

void AdvisorChiefWindow::Impl::drawEducation(Point pos)
{
  std::string text;
  HouseList houses = city::Statistic::getEvolveEducationReadyHouse( city );

  text = houses.empty()
            ? "##advchief_education_ok##"
            : "##advchief_some_need_education##";

  drawReportRow( pos, _("##advchief_education##"), _( text ) );
}

void AdvisorChiefWindow::Impl::drawReligion(Point pos)
{
  std::string text;
  drawReportRow( pos, _("##advchief_religion##"), text );
}

void AdvisorChiefWindow::Impl::drawEntertainment(Point pos)
{
  std::string text;
  drawReportRow( pos, _("##advchief_entertainment##"), text );
}

void AdvisorChiefWindow::Impl::drawSentiment(Point pos)
{
  std::string text;
  drawReportRow( pos, _("##advchief_sentiment##"), text );
}

}//end namespace gui
