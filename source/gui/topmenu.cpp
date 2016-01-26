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
    Signal0<> onExit;
    Signal0<> onEnd;
    Signal0<> onSave;
    Signal0<> onLoad;
    Signal0<> onRestart;
    Signal0<> onShowVideoOptions;
    Signal0<> onShowSoundOptions;
    Signal0<> onShowGameSpeedOptions;
    Signal0<> onShowCityOptions;
    Signal1<bool> onToggleConstructorMode;
    Signal1<int> onShowExtentInfo;
    Signal1<Advisor> onRequestAdvisor;
  } signal;

slots public:
  void resolveSave();
  void updateDate();
  void showAboutInfo();
  void resolveAdvisorShow(int);
  void handleDebugEvent(int);
  void showShortKeyInfo();
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

void TopMenu::setFunds( int value )
{
  if( _d->lbFunds )
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

void TopMenu::Impl::showShortKeyInfo()
{
  Widget& shortKeyInfo = lbDate->ui()->add<Label>( Rect( 0, 0, 500, 300 ), "", false, Label::bgWhiteFrame );
  shortKeyInfo.setupUI( ":/gui/shortkeys.gui" );
  shortKeyInfo.add<ExitButton>( Point( shortKeyInfo.width() - 34, shortKeyInfo.height() - 34 ) );

  shortKeyInfo.moveTo( Widget::parentCenter );
  WidgetClose::insertTo( &shortKeyInfo, KEY_RBUTTON );
}

void TopMenu::Impl::resolveExtentInfo(Widget *sender)
{
  int tag = sender->getProperty( TEXT(ExtentInfo) );
  if( tag != extentinfo::none )
  {
    emit signal.onShowExtentInfo( tag );
  }
}

void TopMenu::Impl::initBackground( const Size& size )
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

void TopMenu::Impl::showAboutInfo()
{
  lbDate->ui()->add<SimpleWindow>( Rect( 0, 0, 500, 300 ), "", ":/gui/about.gui" );
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

  ContextMenuItem* tmp = addItem( _("##gmenu_file##"), -1, true, true, false, false );
  ContextMenu* file = tmp->addSubMenu();

  ContextMenuItem* restart = file->addItem( _("##gmenu_file_restart##"), -1, true, false, false, false );
  ContextMenuItem* load =    file->addItem( _("##mainmenu_loadgame##"),  -1, true, false, false, false );
  ContextMenuItem* save =    file->addItem( _("##gmenu_file_save##"),    -1, true, false, false, false );
  ContextMenuItem* mainMenu= file->addItem( _("##gmenu_file_mainmenu##"),-1, true, false, false, false );
  ContextMenuItem* exit =    file->addItem( _("##gmenu_exit_game##"),    -1, true, false, false, false );

  CONNECT( restart, onClicked(), &_d->signal.onRestart, Signal0<>::_emit );
  CONNECT( exit, onClicked(), &_d->signal.onExit, Signal0<>::_emit );
  CONNECT( save, onClicked(), &_d->signal.onSave, Signal0<>::_emit );
  CONNECT( load, onClicked(), &_d->signal.onLoad, Signal0<>::_emit );
  CONNECT( mainMenu, onClicked(), &_d->signal.onEnd, Signal0<>::_emit );

  tmp = addItem( _("##gmenu_options##"), -1, true, true, false, false );
  ContextMenu* options = tmp->addSubMenu();
  ContextMenuItem* screen = options->addItem( _("##screen_settings##"), -1, true, false, false, false );
  ContextMenuItem* sound = options->addItem( _("##sound_settings##"), -1, true, false, false, false );
  ContextMenuItem* speed = options->addItem( _("##speed_settings##"), -1, true, false, false, false );
  ContextMenuItem* cityopts = options->addItem( _("##city_settings##"), -1, true, false, false, false );
  ContextMenuItem* constrMode = options->addItem( _("##city_constr_mode##"), -1, true, false, false, false );
  constrMode->setAutoChecking( true );

  CONNECT( screen, onClicked(), &_d->signal.onShowVideoOptions,     Signal0<>::_emit );
  CONNECT( speed,  onClicked(), &_d->signal.onShowGameSpeedOptions, Signal0<>::_emit );
  CONNECT( sound,  onClicked(), &_d->signal.onShowSoundOptions,     Signal0<>::_emit );
  CONNECT( cityopts,  onClicked(), &_d->signal.onShowCityOptions,   Signal0<>::_emit );
  CONNECT( constrMode, onChecked(), &_d->signal.onToggleConstructorMode, Signal1<bool>::_emit );

  tmp = addItem( _("##gmenu_help##"), -1, true, true, false, false );
  ContextMenu* helpMenu = tmp->addSubMenu();
  ContextMenuItem* aboutItem = helpMenu->addItem( _("##gmenu_about##"), -1 );
  ContextMenuItem* shortkeysItem = helpMenu->addItem( _("##gmenu_shortkeys##"), -1 );
  CONNECT( aboutItem, onClicked(), _d.data(), Impl::showAboutInfo );
  CONNECT( shortkeysItem, onClicked(), _d.data(), Impl::showShortKeyInfo );

  tmp = addItem( _("##gmenu_advisors##"), -1, true, true, false, false );
  ContextMenu* advisersMenu = tmp->addSubMenu();
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

Signal0<>& TopMenu::onExit() {  return _d->signal.onExit; }
Signal0<>& TopMenu::onSave(){  return _d->signal.onSave; }
Signal0<>& TopMenu::onEnd(){  return _d->signal.onEnd; }
Signal1<Advisor>& TopMenu::onRequestAdvisor() {  return _d->signal.onRequestAdvisor; }
Signal1<int> &TopMenu::onShowExtentInfo() { return _d->signal.onShowExtentInfo; }
Signal0<>& TopMenu::onLoad(){  return _d->signal.onLoad; }
Signal0<>&TopMenu::onRestart() { return _d->signal.onRestart; }
Signal0<>& TopMenu::onShowVideoOptions(){  return _d->signal.onShowVideoOptions; }
Signal0<>&TopMenu::onShowSoundOptions(){ return _d->signal.onShowSoundOptions; }
Signal0<>& TopMenu::onShowGameSpeedOptions(){  return _d->signal.onShowGameSpeedOptions; }
Signal0<>&TopMenu::onShowCityOptions(){ return _d->signal.onShowCityOptions; }
void TopMenu::Impl::resolveAdvisorShow(int id) { emit signal.onRequestAdvisor( (advisor::Type)id ); }
Signal1<bool>&gui::TopMenu::onToggleConstructorMode() { return _d->signal.onToggleConstructorMode; }

}//end namespace gui
