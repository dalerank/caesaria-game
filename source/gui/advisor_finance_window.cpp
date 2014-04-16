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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "advisor_finance_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "city/statistic.hpp"
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
#include "core/logger.hpp"

using namespace constants;
using namespace gfx;

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
  void decreaseTax();
  void increaseTax();
  int calculateTaxValue();
};

AdvisorFinanceWindow::AdvisorFinanceWindow(PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->city = city;
  setGeometry( Rect( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ),
               Size( 640, 420 ) ) );

  gui::Label* title = new gui::Label( this, Rect( 60, 10, 60 + 210, 10 + 40) );
  title->setText( _("##finance_advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( size() ) );

  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteFrame );

  Picture& icon = Picture::load( ResourceGroup::panelBackground, 265 );
  _d->background->draw( icon, Point( 11, 11 ) );

  Font fontWhite = Font::create( FONT_1_WHITE );

  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 70, 50 ), Size( width() - 86, 70 ) ), PictureDecorator::blackFrame);
  std::string moneyStr = StringHelper::format( 0xff, "%s %d %s", _("##city_have##"), city->funds().treasury(), _("##denaries##") );
  fontWhite.draw( *_d->background, moneyStr, 75, 55, false );
  fontWhite.draw( *_d->background, _("##tax_rate##"), 75, 75, false );

  _d->lbTaxRateNow = new gui::Label( this, Rect( 245, 75, 245 + 350, 75 + 20 ), "" );
  _d->lbTaxRateNow->setFont( fontWhite );
  _d->updateTaxRateNowLabel();

  std::string strRegPaeyrs = StringHelper::format( 0xff, "%d%% %s", 0, _("##population_registered_as_taxpayers##") );
  fontWhite.draw( *_d->background, strRegPaeyrs, 75, 95, false );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##last_year##"), 265, 130, false );
  font.draw( *_d->background, _("##this_year##"), 400, 130, false );

  Point startPoint( 75, 145 );
  Point offset( 0, 17 );

  _d->drawReportRow( startPoint, _("##taxes##"), city::Funds::taxIncome );
  _d->drawReportRow( startPoint + offset, _("##trade##"), city::Funds::exportGoods );
  _d->drawReportRow( startPoint + offset * 2, _("##donations##"), city::Funds::donation );
  _d->drawReportRow( startPoint + offset * 3, _("##debet##"), city::Funds::debet );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 3 + Point( 200, 0 ), Size( 72, 1) ) );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 3 + Point( 340, 0 ), Size( 72, 1) ) );
  
  startPoint += Point( 0, 6 );
  _d->drawReportRow( startPoint + offset * 4, _("##import_fn##"), city::Funds::importGoods );
  _d->drawReportRow( startPoint + offset * 5, _("##wages##"), city::Funds::workersWages );
  _d->drawReportRow( startPoint + offset * 6, _("##buildings##"), city::Funds::buildConstruction );
  _d->drawReportRow( startPoint + offset * 7, _("##percents##"), city::Funds::creditPercents );
  _d->drawReportRow( startPoint + offset * 8, _("##pn_salary##"), city::Funds::playerSalary );
   
  _d->drawReportRow( startPoint + offset * 9, _("##other##"), city::Funds::otherExpenditure );
  _d->drawReportRow( startPoint + offset * 10, _("##empire_tax##"), city::Funds::empireTax );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 10 + Point( 200, 0 ), Size( 72, 1) ) );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 10 + Point( 340, 0 ), Size( 72, 1) ) );

  _d->drawReportRow( startPoint + offset * 11, _("##credit##"), city::Funds::credit );

  startPoint += Point( 0, 6 );
  _d->drawReportRow( startPoint + offset * 12, _("##profit##"), city::Funds::cityProfit );
  
  startPoint += Point( 0, 6 );
  _d->drawReportRow( startPoint + offset * 13, _("##balance##"), city::Funds::balance );

  _d->btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );

  TexturedButton* btnDecreaseTax = new TexturedButton( this, Point( 185, 73 ), Size( 24 ), -1, 601 );
  TexturedButton* btnIncreaseTax = new TexturedButton( this, Point( 185+24, 73 ), Size( 24 ), -1, 605 );
  CONNECT( btnDecreaseTax, onClicked(), _d.data(), Impl::decreaseTax );
  CONNECT( btnIncreaseTax, onClicked(), _d.data(), Impl::increaseTax );
}

void AdvisorFinanceWindow::draw(gfx::Engine& painter )
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
  int taxValue = city::Statistic::getTaxValue( city );
  std::string strCurretnTax = StringHelper::format( 0xff, "%d%% %s %d %s",
                                                    city->funds().taxRate(), _("##may_collect_about##"),
                                                    taxValue, _("##denaries##") );
  lbTaxRateNow->setText( strCurretnTax );
}

void AdvisorFinanceWindow::Impl::decreaseTax()
{
  city->funds().setTaxRate( city->funds().taxRate() - 1 );
  updateTaxRateNowLabel();
}

void AdvisorFinanceWindow::Impl::increaseTax()
{
  city->funds().setTaxRate( city->funds().taxRate() + 1 );
  updateTaxRateNowLabel();
}

}//end namespace gui
