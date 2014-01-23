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

using namespace constants;

namespace gui
{


class AdvisorChiefWindow::Impl
{
public:
  PlayerCityPtr city;
  PictureRef background;

  TexturedButton* btnHelp;

  void drawReportRow( Point pos, std::string title, std::string text );
  void drawEmploymentState( Point pos );
  void drawProfitState( Point pos );
  void drawMigrationState( Point pos );
  void drawFoodStockState( Point pos );
  void drawFoodConsumption( Point pos );
};

AdvisorChiefWindow::AdvisorChiefWindow(PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->city = city;
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 420 ) ) );

  gui::Label* title = new gui::Label( this, Rect( 60, 10, 60 + 210, 10 + 40) );
  title->setText( _("##advisor_chief##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );

  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );
  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( 20, 55, getWidth() - 20, getHeight() - 20 ), PictureDecorator::blackFrame);

  Picture& icon = Picture::load( ResourceGroup::panelBackground, 266 );
  _d->background->draw( icon, Point( 11, 11 ) );

  Point startPoint( 20, 60 );
  Point offset( 0, 20 );

  _d->drawEmploymentState( startPoint );
  _d->drawProfitState( startPoint + offset );
  _d->drawMigrationState( startPoint + offset * 2 );
  _d->drawFoodStockState( startPoint + offset * 3 );
  _d->drawFoodConsumption( startPoint + offset * 4 );
}

void AdvisorChiefWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}

void AdvisorChiefWindow::Impl::drawReportRow(Point pos, std::string title, std::string text)
{
  Font font = Font::create( FONT_2_WHITE );
  Font font2 = Font::create( FONT_2 );

  Picture pointPic = Picture::load( ResourceGroup::panelBackground, 48 );

  background->draw( pointPic, pos + Point( 5, 10 ) );
  font.draw( *background, title, pos + Point( 20, 0), false );
  font2.draw( *background, text, pos + Point( 255, 0), false );
}

void AdvisorChiefWindow::Impl::drawEmploymentState(Point pos)
{
  int needWorkersNumber = CityStatistic::getVacantionsNumber( city );
  int workless = CityStatistic::getWorklessPercent( city );
  std::string text;
  if( needWorkersNumber > 0 ) { text = StringHelper::format( 0xff, "%s %d", _("##advchief_needworkers##"), needWorkersNumber );  }
  else if( workless > 10 )  {   text = StringHelper::format( 0xff, "%s %d%%", _("##advchief_workless##"), workless );  }
  else  {                       text = _("##advchief_employers_ok##");  }

  drawReportRow( pos, _("##advchief_employment##"), text );
}

void AdvisorChiefWindow::Impl::drawProfitState(Point pos)
{
  std::string text;
  int profit = city->getFunds().getProfit();
  if( profit >= 0 )  {    text = StringHelper::format( 0xff, "%s %d", _("##advchief_haveprofit##"), profit );  }
  else  {    text = StringHelper::format( 0xff, "%s %d", _("##advchief_havedeficit##"), profit );  }

  drawReportRow( pos, _("##advchief_finance##"), text );
}

void AdvisorChiefWindow::Impl::drawMigrationState(Point pos)
{
  SmartPtr<CityMigration> migration = ptr_cast<CityMigration>( city->findService( CityMigration::getDefaultName() ) );

  std::string text = _("##migration_unknown_reason##");
  if( migration.isValid() )
  {
    text = migration->getReason();
  }

  drawReportRow( pos, _("##advchief_migration##"), text );
}

void AdvisorChiefWindow::Impl::drawFoodStockState(Point pos)
{
  SmartPtr< CityServiceInfo > info = ptr_cast<CityServiceInfo>( city->findService( CityServiceInfo::getDefaultName() ) );

  std::string text = _("##food_stock_unknown_reason##");
  if( info.isValid() )
  {
    int monthWithFood = info->getLast().monthWithFood;
    switch( monthWithFood )
    {
      case 0: text = _("##have_no_food_on_next_month##"); break;
      case 1: text = _("##small_food_on_next_month##"); break;
      case 2: text = _("##some_food_on_next_month##"); break;

      default:
        text = StringHelper::format( 0xff, "%s %d", _("##have_food_for##"), monthWithFood );
    }
  }

  drawReportRow( pos, _("##advchief_food_stocks##"), text );
}

void AdvisorChiefWindow::Impl::drawFoodConsumption(Point pos)
{
  std::string text;
  SmartPtr< CityServiceInfo > info = ptr_cast<CityServiceInfo>( city->findService( CityServiceInfo::getDefaultName() ));

  switch( info->getLast().foodKoeff )
  {
  case -1: text= _("##we_produce_less_than_eat##"); break;
  case 0: text = _("##we_noproduce_food##"); break;
  case 1: text = _("##we_produce_some_than_eat##"); break;
  case 2: text = _("##we_produce_more_than_eat##"); break;

  default: text = _("##we_produce_much_than_eat##");
  }

  drawReportRow( pos, _("##advchief_food_consumption##"), text );
}

}//end namespace gui
