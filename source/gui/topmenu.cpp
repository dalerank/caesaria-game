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

#include "topmenu.hpp"

#include "core/gettext.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "contextmenuitem.hpp"
#include "gfx/picturesarray.hpp"
#include "core/utils.hpp"
#include "game/datetimehelper.hpp"
#include "gfx/engine.hpp"
#include "game/gamedate.hpp"
#include "environment.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"
#include "window.hpp"
#include "texturedbutton.hpp"
#include "game/advisor.hpp"
#include "widgetescapecloser.hpp"
#include "gfx/decorator.hpp"
#include "listbox.hpp"
#include "core/metric.hpp"
#include "gfx/drawstate.hpp"
#include "city/config.hpp"
#include "core/osystem.hpp"

using namespace gfx;

namespace gui
{

namespace {
static const int dateLabelOffset = 155;
static const int populationLabelOffset = 344;
static const int fundLabelOffset = 464;
static const int panelBgStatus = 15;
}

class TopMenu::Impl
{
public:
  DateTime saveDate;
  Label* lbPopulation;
  Label* lbFunds;
  Label* lbDate;
  bool useIcon;
  bool constructorMode;
  ContextMenu* langSelect;

  struct {
    Batch batch;
    Pictures fallback;
    Rects rects;
  } bg;

  struct {
    Signal1<bool> onToggleConstructorMode;
    Signal1<int> onShowExtentInfo;
    Signal1<Advisor> onRequestAdvisor;
  } signal;

slots public:
  void resolveSave();
  void updateDate();
  void resolveAdvisorShow(int);
  void resolveExtentInfo(Widget* sender);
  void initBackground( const Size& size );
};

void TopMenu::draw(gfx::Engine& engine)
{
  if( !visible() )
    return;

  _d->updateDate();

  DrawState pipe( engine, absoluteRect().lefttop(), &absoluteClippingRectRef() );
  pipe.draw( _d->bg.batch )
      .fallback( _d->bg.fallback, _d->bg.rects );

  MainMenu::draw( engine );
}

void TopMenu::setPopulation( int value )
{
  if( _d->lbPopulation )
    _d->lbPopulation->setText( fmt::format( "{} {}", _d->useIcon ? "" : _("##pop##"), value ) );
}

void TopMenu::setProperty(const std::string & name, const Variant & value)
{
  if (name == "funds") {setFunds(value); return;}
  if (name == "population") { setPopulation(value); return; }
}

void TopMenu::setFunds( int value )
{
  if (_d->lbFunds)
    _d->lbFunds->setText( fmt::format( "{} {}", _d->useIcon ? "" : _("##denarii_short##"), value) );
}

void TopMenu::Impl::updateDate()
{
  if( !lbDate )
    return;

  if( !game::Date::isDayChanged() )
    return;

  std::string text;
  if( metric::Measure::mode() == metric::Measure::roman )
  {
    RomanDate rDate( game::Date::current() );
    text = utils::date2str( rDate, true );
  }
  else
  {
    text = utils::date2str( game::Date::current(), true );
  }

  lbDate->setText( text );
}

void TopMenu::Impl::resolveExtentInfo(Widget *sender)
{
  int tag = sender->getProperty( TEXT(ExtentInfo) );
  if( tag != extentinfo::none )
  {
    emit signal.onShowExtentInfo( tag );
  }
}

void TopMenu::Impl::initBackground(const Size& size)
{
  Pictures p_marble;
  p_marble.load( gui::rc.panel, 1, 12 );

  unsigned int i = 0;
  int x = 0;

  float ykoef = size.height() / (float)p_marble.front().height();
  while( x < size.width() )
  {
    const Picture& pic = p_marble[i%12];
    bg.fallback.push_back( pic );
    bg.rects.push_back( Rect( Point( x, 0), pic.size() * ykoef ) );
    x += pic.width() * ykoef;
    i++;
  }

  bool batchOk = bg.batch.load( bg.fallback, bg.rects );
  if( !batchOk )
    bg.batch.destroy();
}

TopMenu::TopMenu(Widget* parent, const int height , bool useIcon)
: MainMenu( parent, Rect( 0, 0, parent->width(), height ) ),
  _d( new Impl )
{
  setupUI( ":/gui/topmenu.gui" );
  setGeometry( Rect( 0, 0, parent->width(), height ) );

  _d->initBackground( size() );
  _d->useIcon = useIcon;
  _d->constructorMode = false;
  setInternalName(TEXT(TopMenu));

  GET_DWIDGET_FROM_UI( _d, lbPopulation )
  GET_DWIDGET_FROM_UI( _d, lbFunds )
  GET_DWIDGET_FROM_UI( _d, lbDate )

  if( _d->lbPopulation )
  {
    _d->lbPopulation->setPosition( Point( width() - populationLabelOffset, 0 ) );
    _d->lbPopulation->setIcon( useIcon ? "population" : "none", 1 );
    _d->lbPopulation->addProperty( TEXT(ExtentInfo), extentinfo::population );
  }

  if( _d->lbFunds )
  {
    _d->lbFunds->setPosition(  Point( width() - fundLabelOffset, 0) );
    _d->lbFunds->setIcon( useIcon ? "paneling" : "", 332 );
    _d->lbFunds->addProperty( TEXT(ExtentInfo), extentinfo::economy);
  }

  if( _d->lbDate )
  {
    _d->lbDate->setPosition( Point( width() - dateLabelOffset, 0) );
    _d->lbDate->addProperty( TEXT(ExtentInfo), extentinfo::celebrates );
    CONNECT( _d->lbDate, onClickedA(), _d.data(), Impl::resolveExtentInfo )
  }

  //CONNECT( constrMode, onChecked(), &_d->signal.onToggleConstructorMode, Signal1<bool>::_emit );

  auto tmp = addItem( _("##gmenu_advisors##"), -1, true, true, false, false );
  ContextMenu* advisersMenu = tmp->addSubmenu();
  advisersMenu->addItem( _("##visit_labor_advisor##"      ), advisor::employers );
  advisersMenu->addItem( _("##visit_military_advisor##"   ), advisor::military );
  advisersMenu->addItem( _("##visit_imperial_advisor##"   ), advisor::empire );
  advisersMenu->addItem( _("##visit_rating_advisor##"     ), advisor::ratings );
  advisersMenu->addItem( _("##visit_trade_advisor##"      ), advisor::trading );
  advisersMenu->addItem( _("##visit_population_advisor##" ), advisor::population );
  advisersMenu->addItem( _("##visit_health_advisor##"     ), advisor::health );
  advisersMenu->addItem( _("##visit_education_advisor##"  ), advisor::education );
  advisersMenu->addItem( _("##visit_religion_advisor##"   ), advisor::religion );
  advisersMenu->addItem( _("##visit_entertainment_advisor##"), advisor::entertainment );
  advisersMenu->addItem( _("##visit_financial_advisor##"  ), advisor::finance );
  advisersMenu->addItem( _("##visit_chief_advisor##"      ), advisor::main );

  CONNECT( advisersMenu, onItemAction(), _d.data(), Impl::resolveAdvisorShow );

  _d->updateDate();
}

Signal1<Advisor>& TopMenu::onRequestAdvisor() {  return _d->signal.onRequestAdvisor; }
Signal1<int> &TopMenu::onShowExtentInfo() { return _d->signal.onShowExtentInfo; }
void TopMenu::Impl::resolveAdvisorShow(int id) { emit signal.onRequestAdvisor( (advisor::Type)id ); }
Signal1<bool>&gui::TopMenu::onToggleConstructorMode() { return _d->signal.onToggleConstructorMode; }

}//end namespace gui
