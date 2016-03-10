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
#include "game/datetimehelper.hpp"
#include "listbox.hpp"
#include "objects/house_level.hpp"
#include "core/event.hpp"
#include "environment.hpp"
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

struct RowInfo
{
  std::string title;
  econ::Issue::Type type;
};

class Finance::Impl
{
public:
  typedef enum { taxes=0,  trade=1, donations=2, debt=3, imports=4, wages=5,
                 constructions=6, credit=7, salaries=8,
                 sundries=9, empireTaxes=10, expensives=11,
                 profits=12, balance=13 } Row;

  std::map<int,RowInfo> names = { {taxes, {"##taxes##", econ::Issue::taxIncome}},
                             {trade,{"##trade##", econ::Issue::exportGoods}},
                             {donations,{"##donations##", econ::Issue::donation }},
                             {debt,{"##debet##", econ::Issue::debet }},
                             {imports,{"##import_fn##", econ::Issue::importGoods}},
                             {wages,{"##wages##",     econ::Issue::workersWages}},
                             {constructions,{"##buildings##", econ::Issue::buildConstruction}},
                             {credit,{"##percents##",  econ::Issue::creditPercents}},
                             {salaries,{"##pn_salary##", econ::Issue::playerSalary}},
                             {sundries,{"##other##",     econ::Issue::sundries}},
                             {empireTaxes,{"##empire_tax##",econ::Issue::empireTax}},
                             {expensives,{"##credit##",    econ::Issue::credit}},
                             {profits,{"##profit##",    econ::Issue::cityProfit}},
                             {balance,{"##balance##",   econ::Issue::balance}}
                           };
};



Finance::Finance(PlayerCityPtr city, Widget* parent)
  : Base( parent, city, advisor::finance ), _d( new Impl )
{
  setupUI( ":/gui/financeadv.gui" );

  _updateCityTreasure();
  _updateTaxRateNowLabel();
  _updateRegisteredPayers();
  _initReportRows();
  _initTaxManager();
}

class FinanceRow : public PushButton
{
public:
  FinanceRow( Widget* parent, const Rect& rect, const std::string& title,
              int type, PlayerCityPtr city, bool line )
    : PushButton( parent, rect )
  {
    setBackgroundStyle( PushButton::noBackground );
    text = title;
    dline = line;
    lastyear = city->treasury().getIssueValue( (econ::Issue::Type)type, econ::Treasury::lastYear );
    thisyear = city->treasury().getIssueValue( (econ::Issue::Type)type, econ::Treasury::thisYear );

    setFont( FONT_1 );
    Decorator::draw( border, Rect( 0, 0, width(), height() ), Decorator::brownBorder );
  }

  void _updateTexture()
  {
    PushButton::_updateTexture();

    canvasDraw( _(text), Point( 20, 0 ) );
    canvasDraw( utils::i2str( lastyear ), Point( 215, 0) );
    canvasDraw( utils::i2str( thisyear ), Point( 355, 0) );
  }

  void draw(Engine &painter)
  {
    PushButton::draw( painter );

    if( _state() == stHovered )
      painter.draw( border, absoluteRect().lefttop(), &absoluteClippingRectRef() );

    if( dline )
    {
      Rect p( absoluteRect().leftbottom() + Point( 200, -1 ), Size( 72, 1) );
      painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );

      p = Rect( absoluteRect().leftbottom() + Point( 340, -1 ), Size( 72, 1) );
      painter.drawLine( 0xff000000, p.lefttop(), p.righttop() );
    }
  }

  bool dline;
  int lastyear;
  int thisyear;
  std::string text;
  gfx::Pictures border;
};

void Finance::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

bool Finance::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui
      && event.gui.type == guiButtonClicked )
  {
    if( safety_cast<FinanceRow*>( event.gui.caller ) != 0 )
      return true;
  }

  return Base::onEvent(event);
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

void Finance::_addRow( const Rect& rect, int index, bool line )
{
  const auto& info = _d->names[ index ];
  auto& btn = add<FinanceRow>( rect + offset * index, info.title, info.type, _city, line );
  btn.setID( info.type );
  CONNECT_LOCAL( &btn, onClickedEx(), Finance::_showIssueDetails )
}

class FinanceDetails : public Window
{
public:
  FinanceDetails( Widget* parent, const Size& size, const std::string& title, PlayerCityPtr city, econ::Issue::Type type )
   : Window( parent, Rect( Point(), size), title )
  {
    setTextAlignment( align::center, align::center );
    setFont( FONT_3 );
    setTitleRect( Rect( 15, 15, width() - 15, 45 ) );
    add<ExitButton>( Point( width() - 37, 12 ) );

    ListBox& listbox = add<ListBox>( Rect( 15, 45, width()-15, height() - 15 ), -1, true, true );
    listbox.setItemsFont( Font::create( FONT_1 ) );
    listbox.setItemsHeight( 16 );

    const econ::DateIssues& history = city->treasury().getIssueHistory().issues();
    DateTime lasttime;
    int summ = 0;
    for( auto it = history.rbegin(); it != history.rend(); ++it )
    {
      const auto& issue = *it;
      if( issue.type == type )
      {
        summ += issue.money;
        if( lasttime != issue.time && summ != 0 )
        {
          lasttime = issue.time;
          summ = 0;
          std::string text = fmt::format( "{} {} {}", utils::date2str( issue.time, true ), issue.money, "##issue_description##" );
          listbox.addItem( text );
        }
      }
    }

    moveToCenter();
    setModal();
  }
};

void Finance::_showIssueDetails(Widget* widget)
{
  int id = widget->ID();
  const auto& info = _d->names[ id ];
  ui()->add<FinanceDetails>( Size(480, 640 ), _(info.title), _city, info.type );
}

void Finance::_initReportRows()
{
  Rect rect( startPoint, Size( 500, 20 ) );
  _addRow( rect, Impl::taxes );
  _addRow( rect, Impl::trade );
  _addRow( rect, Impl::donations );
  _addRow( rect, Impl::debt, true );

  rect += Point( 0, 6 );
  _addRow( rect, Impl::imports );
  _addRow( rect, Impl::wages );
  _addRow( rect, Impl::constructions );
  _addRow( rect, Impl::credit );
  _addRow( rect, Impl::salaries );
  _addRow( rect, Impl::sundries );
  _addRow( rect, Impl::empireTaxes );
  _addRow( rect, Impl::expensives, true );

  rect += Point( 0, 6 );
  _addRow( rect, Impl::profits );
  _addRow( rect, Impl::balance );
}

void Finance::_initTaxManager()
{
  add<HelpButton>( Point(12, height() - 39), "finance_advisor" );
  auto& btnDecreaseTax = add<TexturedButton>( Point( 185, 73 ), Size(24,24), -1, gui::button.arrowDown );
  auto& btnIncreaseTax = add<TexturedButton>( Point( 185+24, 73 ), Size(24, 24), -1, gui::button.arrowUp );

  CONNECT_LOCAL( &btnDecreaseTax, onClicked(), Finance::_decreaseTax );
  CONNECT_LOCAL( &btnIncreaseTax, onClicked(), Finance::_increaseTax );
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
