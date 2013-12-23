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
#include "player/city.hpp"
#include "objects/house.hpp"
#include "core/color.hpp"
#include "gui/texturedbutton.hpp"
#include "player/cityfunds.hpp"
#include "objects/house_level.hpp"
#include "objects/constants.hpp"
#include "player/citymigration.hpp"
#include "player/citystatistic.hpp"

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

  int needWorkersNumber = CityStatistic::getVacantionsNumber(_d->city );
  int workless = CityStatistic::getWorklessPercent( _d->city );
  std::string text;
  if( needWorkersNumber > 0 ) {    text = StringHelper::format( 0xff, "%s %d", _("##advchief_needworkers##"), needWorkersNumber );  }
  else if( workless > 10 )  {    text = StringHelper::format( 0xff, "%s %d%%", _("##advchief_workless##"), workless );  }
  else  {    text = _("##advchief_employers_ok##");  }

  _d->drawReportRow( startPoint, _("##advchief_employment##"), text );

  int profit = _d->city->getFunds().getProfit();
  if( profit >= 0 )  {    text = StringHelper::format( 0xff, "%s %d", _("##advchief_haveprofit##"), profit );  }
  else  {    text = StringHelper::format( 0xff, "%s %d", _("##advchief_havedeficit##"), profit );  }

  _d->drawReportRow( startPoint + offset * 1, _("##advchief_finance##"), text );

  SmartPtr<CityMigration> migration = _d->city->findService( CityMigration::getDefaultName() ).as<CityMigration>();
  //int migrationLastMonth = 0;
  text = _("##migration_unknown_reason##");
  if( migration.isValid() )
  {
    //migrationLastMonth = migration->getValue();
    text = migration->getReason();
  }

  _d->drawReportRow( startPoint + offset * 2, _("##advchief_migration##"), text );

  int foodStock = CityStatistic::getFoodStock( _d->city );
  int foodMontlyConsumption = CityStatistic::getFoodMonthlyConsumption( _d->city );
  int monthWithFood = foodStock / foodMontlyConsumption;

  switch( monthWithFood )
  {
    case 0: text = _("##have_no_food_on_next_month##"); break;
    case 1: text = _("##small_food_on_next_month##"); break;
    case 2: text = _("##some_food_on_next_month##"); break;

    default:
      text = StringHelper::format( 0xff, "%s %d", _("##have_food_for##"), monthWithFood );
  }

  _d->drawReportRow( startPoint + offset * 3, _("##advchief_food_stocks##"), text );

  int foodProducing = CityStatistic::getFoodProducing( _d->city );
  if( foodProducing == 0 )
  {
    text = _("##we_noproduce_food##");
  }
  else
  {
    int yearlyFoodConsumption = foodMontlyConsumption * DateTime::monthInYear;
    bool positiveProducing = (foodProducing - yearlyFoodConsumption >= 0 ? true : false);
    if( positiveProducing )
    {
      int foodKoeff = foodProducing / yearlyFoodConsumption;
      switch( foodKoeff )
      {
      case 0: case 1: text = _("##we_produce_some_than_eat##"); break;
      case 2: text = _("##we_produce_more_than_eat##"); break;
      default:
        text = _("##we_produce_much_than_eat##");
      }
    }
    else
    {
      text = _("##we_produce_less_than_eat##");
    }
  }

  _d->drawReportRow( startPoint + offset * 4, _("##advchief_food_consumption##"), text );
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

}//end namespace gui
