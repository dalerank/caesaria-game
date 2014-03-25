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

#include "advisor_finance_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "game/enums.hpp"
#include "city/helper.hpp"
#include "objects/house.hpp"
#include "core/color.hpp"
#include "gui/texturedbutton.hpp"
#include "city/funds.hpp"
#include "objects/house_level.hpp"
#include "objects/constants.hpp"

using namespace constants;

namespace gui
{


class AdvisorFinanceWindow::Impl
{
public:
  PlayerCityPtr city;
  PictureRef background;

  gui::Label* lbTaxRateNow;
  TexturedButton* btnHelp;

  void drawReportRow( const Point& pos, const std::string& title, city::Funds::IssueType type );
  void updateTaxRateNowLabel();
  int calculateTaxValue();
};

AdvisorFinanceWindow::AdvisorFinanceWindow(PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->city = city;
  setGeometry( Rect( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ),
               Size( 640, 420 ) ) );

  gui::Label* title = new gui::Label( this, Rect( 60, 10, 60 + 210, 10 + 40) );
  title->setText( _("##Finance advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( size() ) );

  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteFrame );
  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 70, 50 ), Size( width() - 86, 70 ) ), PictureDecorator::blackFrame);

  Picture& icon = Picture::load( ResourceGroup::panelBackground, 265 );
  _d->background->draw( icon, Point( 11, 11 ) );

  Font fontWhite = Font::create( FONT_1_WHITE );

  std::string moneyStr = StringHelper::format( 0xff, "%s %d %s", _("##city_have##"), city->funds().treasury(), _("##denaries##") );
  fontWhite.draw( *_d->background, moneyStr, 70, 55, false );
  fontWhite.draw( *_d->background, _("##tax_rate##"), 65, 75, false );

  _d->lbTaxRateNow = new gui::Label( this, Rect( 245, 75, 245 + 350, 75 + 20 ), "" );
  _d->lbTaxRateNow->setFont( fontWhite );
  _d->updateTaxRateNowLabel();

  std::string strRegPaeyrs = StringHelper::format( 0xff, "%d%% %s", 0, _("##population_registered_as_taxpayers##") );
  fontWhite.draw( *_d->background, strRegPaeyrs, 70, 95, false );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##Last year##"), 265, 130, false );
  font.draw( *_d->background, _("##This year##"), 400, 130, false );

  Point startPoint( 75, 145 );
  Point offset( 0, 17 );

  _d->drawReportRow( startPoint, _("##Taxes##"), city::Funds::taxIncome );
  _d->drawReportRow( startPoint + offset, _("##Trade##"), city::Funds::exportGoods );
  _d->drawReportRow( startPoint + offset * 2, _("##Donations##"), city::Funds::donation );
  _d->drawReportRow( startPoint + offset * 3, _("##Debet##"), city::Funds::debet );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 3 + Point( 200, 0 ), Size( 72, 1) ) );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 3 + Point( 340, 0 ), Size( 72, 1) ) );
  
  startPoint += Point( 0, 6 );
  _d->drawReportRow( startPoint + offset * 4, _("##Import##"), city::Funds::importGoods );
  _d->drawReportRow( startPoint + offset * 5, _("##Wages##"), city::Funds::workersWages );
  _d->drawReportRow( startPoint + offset * 6, _("##Buildings##"), city::Funds::buildConstruction );
  _d->drawReportRow( startPoint + offset * 7, _("##Percents##"), city::Funds::creditPercents );
  _d->drawReportRow( startPoint + offset * 8, _("##Salary##"), city::Funds::playerSalary );
   
  _d->drawReportRow( startPoint + offset * 9, _("##Other##"), city::Funds::otherExpenditure );
  _d->drawReportRow( startPoint + offset * 10, _("##Empire tax##"), city::Funds::empireTax );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 10 + Point( 200, 0 ), Size( 72, 1) ) );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 10 + Point( 340, 0 ), Size( 72, 1) ) );

  _d->drawReportRow( startPoint + offset * 11, _("##Credit##"), city::Funds::credit );

  startPoint += Point( 0, 6 );
  _d->drawReportRow( startPoint + offset * 12, _("##Profit##"), city::Funds::profit );
  
  startPoint += Point( 0, 6 );
  _d->drawReportRow( startPoint + offset * 13, _("##Balance##"), city::Funds::balance );

  _d->btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );

  new TexturedButton( this, Point( 185, 70 ), Size( 24 ), -1, 601 );
  new TexturedButton( this, Point( 185+24, 70 ), Size( 24 ), -1, 605 );
}

void AdvisorFinanceWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, screenLeft(), screenTop() );

  Widget::draw( painter );
}


void AdvisorFinanceWindow::Impl::drawReportRow(const Point& pos, const std::string& title, city::Funds::IssueType type)
{
  Font font = Font::create( FONT_1 );

  int lyvalue = city->funds().getIssueValue( type, city::Funds::lastYear );
  int tyvalue = city->funds().getIssueValue( type, city::Funds::thisYear );

  font.draw( *background, title, pos, false );
  font.draw( *background, StringHelper::format( 0xff, "%d", lyvalue ), pos + Point( 215, 0), false );
  font.draw( *background, StringHelper::format( 0xff, "%d", tyvalue ), pos + Point( 355, 0), false );
}

void AdvisorFinanceWindow::Impl::updateTaxRateNowLabel()
{
  int taxValue = calculateTaxValue();
  std::string strCurretnTax = StringHelper::format( 0xff, "%d%% %s %d %s",
                                                    city->funds().getTaxRate(), _("##may_collect_about##"),
                                                    taxValue, _("##denaries##") );
  lbTaxRateNow->setText( strCurretnTax );
}

int AdvisorFinanceWindow::Impl::calculateTaxValue()
{
  city::Helper helper( city );

  HouseList houses = helper.find<House>( building::house );

  float taxValue = 0.f;
  float taxRate = city->funds().getTaxRate();
  foreach( house, houses )
  {
    int maxhb = (*house)->getMaxHabitants();
    if( maxhb == 0 )
      continue;

    int maturehb = (*house)->getHabitants().count( CitizenGroup::mature );
    int housetax = (*house)->getSpec().taxRate();
    taxValue += housetax * maturehb * taxRate / maxhb;
  }

  return taxValue;
}

}//end namespace gui
