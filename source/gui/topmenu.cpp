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
#include "game/enums.hpp"
#include "game/gamedate.hpp"
#include "environment.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"
#include "texturedbutton.hpp"
#include "game/advisor.hpp"
#include "widgetescapecloser.hpp"
#include "listbox.hpp"

using namespace constants;
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
  ContextMenu* langSelect;
  Pictures background;

slots public:
  void resolveSave();
  void updateDate();
  void showAboutInfo();
  void resolveAdvisorShow(int);
  void handleDebugEvent(int);
  void showShortKeyInfo();
  void initBackground( const Size& size );

signals public:
  Signal0<> onExitSignal;
  Signal0<> onEndSignal;
  Signal0<> onSaveSignal;
  Signal0<> onLoadSignal;
  Signal0<> onRestartSignal;
  Signal0<> onShowVideoOptionsSignal;
  Signal0<> onShowSoundOptionsSignal;
  Signal0<> onShowGameSpeedOptionsSignal;
  Signal0<> onShowCityOptionsSignal;
  Signal1<advisor::Type> onRequestAdvisorSignal;
};

void TopMenu::draw(gfx::Engine& engine )
{
  if( !visible() )
    return;

  _d->updateDate();

  engine.draw( _d->background, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );

  MainMenu::draw( engine );
}

void TopMenu::setPopulation( int value )
{
  if( _d->lbPopulation )
    _d->lbPopulation->setText( utils::format( 0xff, "%s %d", _("##pop##"), value ) );
}

void TopMenu::setFunds( int value )
{
  if( _d->lbFunds )
    _d->lbFunds->setText( utils::format( 0xff, "%.2s %d", _("##denarii_short##"), value) );
}

void TopMenu::Impl::updateDate()
{
  if( !lbDate || saveDate.month() == game::Date::current().month() )
    return;

  lbDate->setText( util::date2str( game::Date::current() ) );
}

void TopMenu::Impl::showShortKeyInfo()
{
  Widget* parent = lbDate->ui()->rootWidget();
  Widget* bg = new Label( parent, Rect( 0, 0, 500, 300 ), "", false, Label::bgWhiteFrame );
  bg->setupUI( ":/gui/shortkeys.gui" );
  bg->setCenter( parent->center() );

  TexturedButton* btnExit = new TexturedButton( bg, Point( bg->width() - 34, bg->height() - 34 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  WidgetEscapeCloser::insertTo( bg );

  CONNECT( btnExit, onClicked(), bg, Label::deleteLater );
}

void TopMenu::Impl::initBackground( const Size& size )
{
  Pictures p_marble;
  for (int i = 1; i<=12; ++i)
  {
    p_marble.push_back( Picture::load( ResourceGroup::panelBackground, i));
  }

  background.clear();

  unsigned int i = 0;
  int x = 0;

  while( x < size.width())
  {
    background.append( p_marble[i%12], Point( x, 0 ) );
    x += p_marble[i%12].width();
    i++;
  }
}

void TopMenu::Impl::showAboutInfo()
{
  Widget* parent = lbDate->ui()->rootWidget();
  Widget* bg = new Label( parent, Rect( 0, 0, 500, 300 ), "", false, Label::bgWhiteFrame );
  bg->setupUI( ":/gui/about.gui" );
  bg->setCenter( parent->center() );

  TexturedButton* btnExit = new TexturedButton( bg, Point( bg->width() - 34, bg->height() - 34 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  WidgetEscapeCloser::insertTo( bg );

  CONNECT( btnExit, onClicked(), bg, Label::deleteLater );
}

TopMenu::TopMenu( Widget* parent, const int height ) 
: MainMenu( parent, Rect( 0, 0, parent->width(), height ) ),
  _d( new Impl )
{
  setupUI( ":/gui/topmenu.gui" );
  setGeometry( Rect( 0, 0, parent->width(), height ) );

  _d->initBackground( size() );

  GET_DWIDGET_FROM_UI( _d, lbPopulation )
  GET_DWIDGET_FROM_UI( _d, lbFunds )
  GET_DWIDGET_FROM_UI( _d, lbDate )

  if( _d->lbPopulation )
    _d->lbPopulation->setPosition( Point( width() - populationLabelOffset, 0 ) );

  if( _d->lbFunds )
    _d->lbFunds->setPosition(  Point( width() - fundLabelOffset, 0) );

  if( _d->lbDate )
    _d->lbDate->setPosition( Point( width() - dateLabelOffset, 0) );

  ContextMenuItem* tmp = addItem( _("##gmenu_file##"), -1, true, true, false, false );
  ContextMenu* file = tmp->addSubMenu();

  ContextMenuItem* restart = file->addItem( _("##gmenu_file_restart##"), -1, true, false, false, false );
  ContextMenuItem* load = file->addItem( _("##mainmenu_loadgame##"), -1, true, false, false, false );
  ContextMenuItem* save = file->addItem( _("##gmenu_file_save##"), -1, true, false, false, false );
  ContextMenuItem* mainMenu = file->addItem( _("##gmenu_file_mainmenu##"), -1, true, false, false, false );
  ContextMenuItem* exit = file->addItem( _("##gmenu_exit_game##"), -1, true, false, false, false );

  CONNECT( restart, onClicked(), &_d->onRestartSignal, Signal0<>::_emit );
  CONNECT( exit, onClicked(), &_d->onExitSignal, Signal0<>::_emit );
  CONNECT( save, onClicked(), &_d->onSaveSignal, Signal0<>::_emit );
  CONNECT( load, onClicked(), &_d->onLoadSignal, Signal0<>::_emit );
  CONNECT( mainMenu, onClicked(), &_d->onEndSignal, Signal0<>::_emit );

  tmp = addItem( _("##gmenu_options##"), -1, true, true, false, false );
  ContextMenu* options = tmp->addSubMenu();
  ContextMenuItem* screen = options->addItem( _("##screen_settings##"), -1, true, false, false, false );
  ContextMenuItem* sound = options->addItem( _("##sound_settings##"), -1, true, false, false, false );
  ContextMenuItem* speed = options->addItem( _("##speed_settings##"), -1, true, false, false, false );
  ContextMenuItem* cityopts = options->addItem( _("##city_settings##"), -1, true, false, false, false );

  CONNECT( screen, onClicked(), &_d->onShowVideoOptionsSignal,     Signal0<>::_emit );
  CONNECT( speed,  onClicked(), &_d->onShowGameSpeedOptionsSignal, Signal0<>::_emit );
  CONNECT( sound,  onClicked(), &_d->onShowSoundOptionsSignal,     Signal0<>::_emit );
  CONNECT( cityopts,  onClicked(), &_d->onShowCityOptionsSignal,   Signal0<>::_emit );

  tmp = addItem( _("##gmenu_help##"), -1, true, true, false, false );
  ContextMenu* helpMenu = tmp->addSubMenu();
  ContextMenuItem* aboutItem = helpMenu->addItem( _("##gmenu_about##"), -1 );
  ContextMenuItem* shortkeysItem = helpMenu->addItem( _("##gmenu_shortkeys##"), -1 );
  CONNECT( aboutItem, onClicked(), _d.data(), Impl::showAboutInfo );
  CONNECT( shortkeysItem, onClicked(), _d.data(), Impl::showShortKeyInfo );

  tmp = addItem( _("##gmenu_advisors##"), -1, true, true, false, false );
  ContextMenu* advisersMenu = tmp->addSubMenu();
  advisersMenu->addItem( _("##visit_labor_advisor##"), advisor::employers );
  advisersMenu->addItem( _("##visit_military_advisor##"   ), advisor::military );
  advisersMenu->addItem( _("##visit_imperial_advisor##"     ), advisor::empire );
  advisersMenu->addItem( _("##visit_rating_advisor##"    ), advisor::ratings );
  advisersMenu->addItem( _("##visit_trade_advisor##"    ), advisor::trading);
  advisersMenu->addItem( _("##visit_population_advisor##" ), advisor::population );
  advisersMenu->addItem( _("##visit_health_advisor##"     ), advisor::health );
  advisersMenu->addItem( _("##visit_education_advisor##"  ), advisor::education );
  advisersMenu->addItem( _("##visit_religion_advisor##"   ), advisor::religion );
  advisersMenu->addItem( _("##visit_entertainment_advisor##"), advisor::entertainment );
  advisersMenu->addItem( _("##visit_financial_advisor##"    ), advisor::finance );
  advisersMenu->addItem( _("##visit_chief_advisor##"       ), advisor::main );

  CONNECT( advisersMenu, onItemAction(), _d.data(), Impl::resolveAdvisorShow );

  _d->updateDate();
}

Signal0<>& TopMenu::onExit() {  return _d->onExitSignal; }
Signal0<>& TopMenu::onSave(){  return _d->onSaveSignal; }
Signal0<>& TopMenu::onEnd(){  return _d->onEndSignal; }
Signal1<advisor::Type>& TopMenu::onRequestAdvisor() {  return _d->onRequestAdvisorSignal; }
Signal0<>& TopMenu::onLoad(){  return _d->onLoadSignal; }
Signal0<>&TopMenu::onRestart() { return _d->onRestartSignal; }
Signal0<>& TopMenu::onShowVideoOptions(){  return _d->onShowVideoOptionsSignal; }
Signal0<>&TopMenu::onShowSoundOptions(){ return _d->onShowSoundOptionsSignal; }
Signal0<>& TopMenu::onShowGameSpeedOptions(){  return _d->onShowGameSpeedOptionsSignal; }
Signal0<>&TopMenu::onShowCityOptions(){ return _d->onShowCityOptionsSignal; }
void TopMenu::Impl::resolveAdvisorShow(int id) { emit onRequestAdvisorSignal( (advisor::Type)id ); }

}//end namespace gui
