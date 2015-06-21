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

#include "advisor_finance_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "city/statistic.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "objects/construction.hpp"
#include "city/helper.hpp"
#include "objects/house.hpp"
#include "core/color.hpp"
#include "gui/texturedbutton.hpp"
#include "game/funds.hpp"
#include "objects/house_level.hpp"
#include "objects/constants.hpp"
#include "core/logger.hpp"
#include "city/statistic.hpp"
#include "dictionary.hpp"
#include "widget_helper.hpp"

using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

namespace {
  const Point startPoint( 75, 145 );
  Point offset( 0, 17 );
}

class Finance::Impl
{
public:
};

enum { rowDonation=2, rowDebt=3, rowImports=4, rowWages=5,
       rowConstructions=6, rowCredit=7, rowSalary=8,
       rowSundries=9, rowEmpireTax=10, rowExpensive=11,
       rowProfit=12, rowBalance=13 };

Finance::Finance(PlayerCityPtr city, Widget* parent, int id )
: Base( parent, city, id ), _d( new Impl )
{
  setupUI( ":/gui/financeadv.gui" );

  _updateCityTreasure();
  _updateTaxRateNowLabel();
  _updateRegPayers();
  _initReportRows();
  _initTaxManager();
}

void Finance::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );

  Rect p( startPoint + absoluteRect().lefttop() + offset * rowDebt + Point( 200, 0 ), Size( 72, 1) );
  painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );

  p = Rect( startPoint + absoluteRect().lefttop() + offset * rowDebt + Point( 340, 0 ), Size( 72, 1) );
  painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );

  p = Rect( startPoint + absoluteRect().lefttop() + offset * rowExpensive + Point( 200, 10 ), Size( 72, 1) );
  painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );

  p =  Rect( startPoint + absoluteRect().lefttop() + offset * rowExpensive + Point( 340, 10 ), Size( 72, 1) );
  painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );
}

void Finance::_showHelp()
{
  DictionaryWindow::show( this, "finance_advisor" );
}

void Finance::_drawReportRow(const Point& pos, const std::string& title, int type)
{
  Font font = Font::create( FONT_1 );

  int lyvalue = _city->treasury().getIssueValue( (econ::Issue::Type)type, econ::Treasury::lastYear );
  int tyvalue = _city->treasury().getIssueValue( (econ::Issue::Type)type, econ::Treasury::thisYear );

  Size size( 100, 20 );
  Label* lb = new Label( this, Rect( pos, size), title );
  lb->setFont( font );

  lb = new Label( this, Rect( pos + Point( 215, 0), size), utils::i2str( lyvalue ) );
  lb->setFont( font );

  lb = new Label( this, Rect( pos + Point( 355, 0), size), utils::i2str( tyvalue ) );
  lb->setFont( font );
}

void Finance::_updateTaxRateNowLabel()
{
  INIT_WIDGET_FROM_UI( Label*, lbTaxRateNow )
  if( !lbTaxRateNow )
    return;

  int taxValue = statistic::getTaxValue( _city );
  std::string strCurretnTax = utils::format( 0xff, "%d%% %s %d %s",
                                                    _city->treasury().taxRate(), _("##may_collect_about##"),
                                                    taxValue, _("##denaries##") );
  lbTaxRateNow->setText( strCurretnTax );
}

void Finance::_decreaseTax()
{
  _city->treasury().setTaxRate( _city->treasury().taxRate() - 1 );
  _updateTaxRateNowLabel();
}

void Finance::_increaseTax()
{
  _city->treasury().setTaxRate( _city->treasury().taxRate() + 1 );
  _updateTaxRateNowLabel();
}

void Finance::_initReportRows()
{
  Point sp = startPoint;
  _drawReportRow( sp,                             _("##taxes##"),     econ::Issue::taxIncome );
  _drawReportRow( sp + offset,                    _("##trade##"),     econ::Issue::exportGoods );
  _drawReportRow( sp + offset * rowDonation,      _("##donations##"), econ::Issue::donation );
  _drawReportRow( sp + offset * rowDebt,          _("##debet##"),     econ::Issue::debet );

  sp += Point( 0, 6 );
  _drawReportRow( sp + offset * rowImports,       _("##import_fn##"), econ::Issue::importGoods );
  _drawReportRow( sp + offset * rowWages,         _("##wages##"),     econ::Issue::workersWages );
  _drawReportRow( sp + offset * rowConstructions, _("##buildings##"), econ::Issue::buildConstruction );
  _drawReportRow( sp + offset * rowCredit,        _("##percents##"),  econ::Issue::creditPercents );
  _drawReportRow( sp + offset * rowSalary,        _("##pn_salary##"), econ::Issue::playerSalary );
  _drawReportRow( sp + offset * rowSundries,      _("##other##"),     econ::Issue::sundries );
  _drawReportRow( sp + offset * rowEmpireTax,     _("##empire_tax##"),econ::Issue::empireTax );
  _drawReportRow( sp + offset * rowExpensive,     _("##credit##"),    econ::Issue::credit );

  sp += Point( 0, 6 );
  _drawReportRow( sp + offset * rowProfit,        _("##profit##"),    econ::Issue::cityProfit );

  sp += Point( 0, 6 );
  _drawReportRow( sp + offset * rowBalance,       _("##balance##"),   econ::Issue::balance );
}

void Finance::_initTaxManager()
{
  TexturedButton* btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  TexturedButton* btnDecreaseTax = new TexturedButton( this, Point( 185, 73 ), Size( 24 ), -1, 601 );
  TexturedButton* btnIncreaseTax = new TexturedButton( this, Point( 185+24, 73 ), Size( 24 ), -1, 605 );

  CONNECT( btnDecreaseTax, onClicked(), this, Finance::_decreaseTax );
  CONNECT( btnIncreaseTax, onClicked(), this, Finance::_increaseTax );
  CONNECT( btnHelp,        onClicked(), this, Finance::_showHelp );
}

void Finance::_updateRegPayers()
{
  unsigned int regTaxPayers = statistic::getTaxPayersPercent( _city );
  std::string strRegPaeyrs = utils::format( 0xff, "%d%% %s", regTaxPayers, _("##population_registered_as_taxpayers##") );
  INIT_WIDGET_FROM_UI( Label*, lbRegPayers )
  if( lbRegPayers )
    lbRegPayers->setText( strRegPaeyrs );
}

void Finance::_updateCityTreasure()
{
  INIT_WIDGET_FROM_UI( Label*, lbCityHave )
  if( lbCityHave )
    lbCityHave->setText( utils::format( 0xff, "%s %d %s", _("##city_have##"), _city->treasury().money(), _("##denaries##") ) );
}

}//end namespace advisorwnd

}//end namespace gui
