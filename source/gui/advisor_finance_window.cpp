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
  typedef enum { donations=2, debt=3, imports=4, wages=5,
                 constructions=6, credit=7, salaries=8,
                 sundries=9, empireTaxes=10, expensives=11,
                 profits=12, balance=13 } Row;
};



Finance::Finance(PlayerCityPtr city, Widget* parent, int id )
  : Base( parent, city, id ), _d( new Impl )
{
  setupUI( ":/gui/financeadv.gui" );

  _updateCityTreasure();
  _updateTaxRateNowLabel();
  _updateRegisteredPayers();
  _initReportRows();
  _initTaxManager();
}

void Finance::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );

  Rect p( startPoint + absoluteRect().lefttop() + offset * Impl::debt + Point( 200, 0 ), Size( 72, 1) );
  painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );

  p = Rect( startPoint + absoluteRect().lefttop() + offset * Impl::debt + Point( 340, 0 ), Size( 72, 1) );
  painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );

  p = Rect( startPoint + absoluteRect().lefttop() + offset * Impl::expensives + Point( 200, 10 ), Size( 72, 1) );
  painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );

  p =  Rect( startPoint + absoluteRect().lefttop() + offset * Impl::expensives + Point( 340, 10 ), Size( 72, 1) );
  painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );
}

void Finance::_drawReportRow(const Point& pos, const std::string& title, int type)
{
  Font font = Font::create( FONT_1 );

  int lyvalue = _city->treasury().getIssueValue( (econ::Issue::Type)type, econ::Treasury::lastYear );
  int tyvalue = _city->treasury().getIssueValue( (econ::Issue::Type)type, econ::Treasury::thisYear );

  Size size( 100, 20 );
  add<Label>( Rect( pos, size), _(title), font );
  add<Label>( Rect( pos + Point( 215, 0), size), utils::i2str( lyvalue ), font );
  add<Label>( Rect( pos + Point( 355, 0), size), utils::i2str( tyvalue ), font );
}

void Finance::_updateTaxRateNowLabel()
{
  INIT_WIDGET_FROM_UI( Label*, lbTaxRateNow )
  if( !lbTaxRateNow )
    return;

  int taxValue = _city->statistic().tax.possible();
  std::string strCurretnTax = fmt::format( "{}% {} {} {}",
                                           _city->treasury().taxRate(), _("##may_collect_about##"),
                                           taxValue, _("##denarii_short##") );
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
  _drawReportRow( sp,                                "##taxes##",     econ::Issue::taxIncome );
  _drawReportRow( sp + offset,                       "##trade##",     econ::Issue::exportGoods );
  _drawReportRow( sp + offset * Impl::donations,     "##donations##", econ::Issue::donation );
  _drawReportRow( sp + offset * Impl::debt,          "##debet##",     econ::Issue::debet );

  sp += Point( 0, 6 );
  _drawReportRow( sp + offset * Impl::imports,       "##import_fn##", econ::Issue::importGoods );
  _drawReportRow( sp + offset * Impl::wages,         "##wages##",     econ::Issue::workersWages );
  _drawReportRow( sp + offset * Impl::constructions, "##buildings##", econ::Issue::buildConstruction );
  _drawReportRow( sp + offset * Impl::credit,        "##percents##",  econ::Issue::creditPercents );
  _drawReportRow( sp + offset * Impl::salaries,      "##pn_salary##", econ::Issue::playerSalary );
  _drawReportRow( sp + offset * Impl::sundries,      "##other##",     econ::Issue::sundries );
  _drawReportRow( sp + offset * Impl::empireTaxes,   "##empire_tax##",econ::Issue::empireTax );
  _drawReportRow( sp + offset * Impl::expensives,    "##credit##",    econ::Issue::credit );

  sp += Point( 0, 6 );
  _drawReportRow( sp + offset * Impl::profits,       "##profit##",    econ::Issue::cityProfit );

  sp += Point( 0, 6 );
  _drawReportRow( sp + offset * Impl::balance,       "##balance##",   econ::Issue::balance );
}

void Finance::_initTaxManager()
{
  add<HelpButton>( Point( 12, height() - 39), "finance_advisor" );
  auto& btnDecreaseTax = add<TexturedButton>( Point( 185, 73 ), Size( 24 ), -1, gui::button.arrowDown );
  auto& btnIncreaseTax = add<TexturedButton>( Point( 185+24, 73 ), Size( 24 ), -1, gui::button.arrowUp );

  CONNECT( &btnDecreaseTax, onClicked(), this, Finance::_decreaseTax );
  CONNECT( &btnIncreaseTax, onClicked(), this, Finance::_increaseTax );
}

void Finance::_updateRegisteredPayers()
{
  unsigned int regTaxPayers = _city->statistic().tax.payersPercent();
  std::string strRegPaeyrs = fmt::format( "{}% {}", regTaxPayers, _("##population_registered_as_taxpayers##") );
  INIT_WIDGET_FROM_UI( Label*, lbRegPayers )
  if( lbRegPayers )
    lbRegPayers->setText( strRegPaeyrs );
}

void Finance::_updateCityTreasure()
{
  INIT_WIDGET_FROM_UI( Label*, lbCityHave )
  if( lbCityHave )
    lbCityHave->setText( fmt::format( "{} {} {}", _("##city_have##"), _city->treasury().money(), _("##denarii_short##") ) );
}

}//end namespace advisorwnd

}//end namespace gui
