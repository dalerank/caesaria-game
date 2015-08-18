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

#include "city_options_window.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "world/empire.hpp"
#include "listbox.hpp"
#include "core/utils.hpp"
#include "dialogbox.hpp"
#include "core/gettext.hpp"
#include "environment.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "gameautopause.hpp"
#include "widget_helper.hpp"
#include "widgetescapecloser.hpp"
#include "contextmenuitem.hpp"
#include "game/infoboxmanager.hpp"
#include "layers/layer.hpp"
#include "game/settings.hpp"
#include "texturedbutton.hpp"
#include "topmenu.hpp"
#include "game/difficulty.hpp"
#include "core/metric.hpp"

using namespace citylayer;

namespace gui
{

struct OptionInfo
{
  bool alive;
  const char* btnName;
  int city_option;
  const char* settings_option;
  const char* text_on;
  const char* text_off;
};

#define INIT_OPTION(btnName, co, so, text ) { true, #btnName, co, so, "##" text "_on##", "##" text "_off##" }
static OptionInfo options[] =
{
  INIT_OPTION( btnGodEnabled, PlayerCity::godEnabled, "", "city_opts_god" ),
  INIT_OPTION( btnWarningsEnabled, PlayerCity::warningsEnabled, "", "city_warnings" ),
  INIT_OPTION( btnZoomEnabled, PlayerCity::zoomEnabled, "", "city_zoom" ),
  INIT_OPTION( btnInvertZoom, PlayerCity::zoomInvert, "", "city_zoominv" ),
  INIT_OPTION( btnLockInfobox, -1, game::Settings::lockInfobox, "city_lockinfo" ),
  INIT_OPTION( btnBarbarianMayAttack, PlayerCity::barbarianAttack, "",  "city_barbarian" ),
  INIT_OPTION( btnC3Gameplay, PlayerCity::c3gameplay, "", "city_c3rules" ),
  INIT_OPTION( btnShowTooltips, -1, game::Settings::tooltipEnabled, "city_tooltips" ),
  INIT_OPTION( btnLegionMayAttack, PlayerCity::legionAttack, "", "city_chastener" ),
  INIT_OPTION( btnAnroidBarEnabled, -1, game::Settings::showTabletMenu, "city_androidbar" ),
  INIT_OPTION( btnToggleCcUseAI, -1, game::Settings::ccUseAI, "city_ccuseai" ),
  INIT_OPTION( btnHighlightBuilding, PlayerCity::highlightBuilding, "", "city_highlight_bld" ),
  INIT_OPTION( btnDetroyEpidemicHouses, PlayerCity::destroyEpidemicHouses, "", "city_destroy_epdh" ),
  INIT_OPTION( btnForestFire, PlayerCity::forestFire, "", "city_forest_fire" ),
  INIT_OPTION( btnForestGrow, PlayerCity::forestGrow, "", "city_forest_grow" ),

  { false, "", 0, 0, "", "" }
};
#undef INIT_OPTION

namespace dialog
{

class CityOptions::Impl
{
public:
  GameAutoPause locker;
  Widget* widget;
  PushButton* btnGodEnabled;
  PushButton* btnWarningsEnabled;
  PushButton* btnZoomEnabled;
  PushButton* btnDebugEnabled;
  PushButton* btnInvertZoom;
  PushButton* btnMmbMoving;
  PushButton* btnBarbarianMayAttack;
  PushButton* btnLegionMayAttack;
  PushButton* btnAnroidBarEnabled;
  PushButton* btnToggleBatching;
  PushButton* btnHighlightBuilding;
  PushButton* btnToggleCcUseAI;
  PushButton* btnLockInfobox;
  PushButton* btnC3Gameplay;
  PushButton* btnShowTooltips;
  PushButton* btnDifficulty;
  PushButton* btnMetrics;
  PushButton* btnDetroyEpidemicHouses;
  PushButton* btnForestFire;
  PushButton* btnForestGrow;
  PushButton* btnBorderMoving;

  Label* lbFireRisk;
  Label* lbCollapseRisk;
  TexturedButton* btnIncreaseFireRisk;
  TexturedButton* btnDecreaseFireRisk;
  TexturedButton* btnIncreaseCollapseRisk;
  TexturedButton* btnDecreaseCollapseRisk;

  PlayerCityPtr city;

  void update();
  void toggleDebug();
  void toggleGods();
  void toggleZoomEnabled();
  void invertZoom();
  void toggleWarnings();
  void toggleLeftMiddleMouse();
  void toggleLockInfobox();
  Widget* findDebugMenu(Ui *ui);
  void increaseFireRisk();
  void decreaseFireRisk();
  void increaseCollapseRisk();
  void decreaseCollapseRisk();
  void toggleBarbarianAttack();
  void toggleC3Gameplay();
  void toggleDifficulty();
  void toggleShowTooltips();
  void toggleLegionAttack();
  void toggleForestFire();
  void toggleForestGrow();
  void toggleAndroidBarEnabled();
  void toggleUseBatching();
  void toggleCcUseAI();
  void toggleMetrics();
  void toggleBorderMoving();
  void toggleHighlightBuilding();
  void toggleDestroyEpidemicHouses();
  void toggleCityOption( PlayerCity::OptionType option );
  void changeCityOption( PlayerCity::OptionType option, int delta);
  void setAutoText( Widget* widget, const std::string& text );
  void setAutoText( Widget *widget, const std::string& text, bool enabled );
};

CityOptions::CityOptions( Widget* parent, PlayerCityPtr city )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->city = city;
  _d->locker.activate();
  _d->widget = this;
  setupUI( ":/gui/cityoptions.gui" );

  WidgetEscapeCloser::insertTo( this );

  setCenter( parent->center() );

  GET_DWIDGET_FROM_UI( _d, btnGodEnabled )
  GET_DWIDGET_FROM_UI( _d, btnWarningsEnabled )
  GET_DWIDGET_FROM_UI( _d, btnZoomEnabled )
  GET_DWIDGET_FROM_UI( _d, btnInvertZoom )
  GET_DWIDGET_FROM_UI( _d, btnDebugEnabled )
  GET_DWIDGET_FROM_UI( _d, btnMmbMoving )
  GET_DWIDGET_FROM_UI( _d, btnLockInfobox )
  GET_DWIDGET_FROM_UI( _d, lbFireRisk )
  GET_DWIDGET_FROM_UI( _d, btnIncreaseFireRisk )
  GET_DWIDGET_FROM_UI( _d, btnDecreaseFireRisk )
  GET_DWIDGET_FROM_UI( _d, lbCollapseRisk )
  GET_DWIDGET_FROM_UI( _d, btnIncreaseCollapseRisk )
  GET_DWIDGET_FROM_UI( _d, btnDecreaseCollapseRisk )
  GET_DWIDGET_FROM_UI( _d, btnBarbarianMayAttack )
  GET_DWIDGET_FROM_UI( _d, btnLegionMayAttack )
  GET_DWIDGET_FROM_UI( _d, btnC3Gameplay)
  GET_DWIDGET_FROM_UI( _d, btnShowTooltips )
  GET_DWIDGET_FROM_UI( _d, btnDifficulty )
  GET_DWIDGET_FROM_UI( _d, btnAnroidBarEnabled )
  GET_DWIDGET_FROM_UI( _d, btnToggleBatching )
  GET_DWIDGET_FROM_UI( _d, btnToggleCcUseAI )
  GET_DWIDGET_FROM_UI( _d, btnMetrics )
  GET_DWIDGET_FROM_UI( _d, btnHighlightBuilding )
  GET_DWIDGET_FROM_UI( _d, btnDetroyEpidemicHouses )
  GET_DWIDGET_FROM_UI( _d, btnForestFire )
  GET_DWIDGET_FROM_UI( _d, btnForestGrow )
  GET_DWIDGET_FROM_UI( _d, btnBorderMoving )

  CONNECT( _d->btnGodEnabled, onClicked(), _d.data(), Impl::toggleGods )
  CONNECT( _d->btnWarningsEnabled, onClicked(), _d.data(), Impl::toggleWarnings )
  CONNECT( _d->btnZoomEnabled, onClicked(), _d.data(), Impl::toggleZoomEnabled )
  CONNECT( _d->btnInvertZoom, onClicked(), _d.data(), Impl::invertZoom )
  CONNECT( _d->btnDebugEnabled, onClicked(), _d.data(), Impl::toggleDebug )
  CONNECT( _d->btnMmbMoving, onClicked(), _d.data(), Impl::toggleLeftMiddleMouse )
  CONNECT( _d->btnLockInfobox, onClicked(), _d.data(), Impl::toggleLockInfobox )
  CONNECT( _d->btnIncreaseFireRisk, onClicked(), _d.data(), Impl::increaseFireRisk )
  CONNECT( _d->btnDecreaseFireRisk, onClicked(), _d.data(), Impl::decreaseFireRisk )
  CONNECT( _d->btnBarbarianMayAttack, onClicked(), _d.data(), Impl::toggleBarbarianAttack )
  CONNECT( _d->btnLegionMayAttack, onClicked(), _d.data(), Impl::toggleLegionAttack )
  CONNECT( _d->btnC3Gameplay, onClicked(), _d.data(), Impl::toggleC3Gameplay )
  CONNECT( _d->btnShowTooltips, onClicked(), _d.data(), Impl::toggleShowTooltips )
  CONNECT( _d->btnDifficulty, onClicked(), _d.data(), Impl::toggleDifficulty )
  CONNECT( _d->btnAnroidBarEnabled, onClicked(), _d.data(), Impl::toggleAndroidBarEnabled )
  CONNECT( _d->btnIncreaseCollapseRisk, onClicked(), _d.data(), Impl::increaseCollapseRisk )
  CONNECT( _d->btnDecreaseCollapseRisk, onClicked(), _d.data(), Impl::decreaseCollapseRisk )
  CONNECT( _d->btnToggleBatching, onClicked(), _d.data(), Impl::toggleUseBatching )
  CONNECT( _d->btnToggleCcUseAI, onClicked(), _d.data(), Impl::toggleCcUseAI )
  CONNECT( _d->btnMetrics, onClicked(), _d.data(), Impl::toggleMetrics )
  CONNECT( _d->btnHighlightBuilding, onClicked(), _d.data(), Impl::toggleHighlightBuilding )
  CONNECT( _d->btnDetroyEpidemicHouses, onClicked(), _d.data(), Impl::toggleDestroyEpidemicHouses )
  CONNECT( _d->btnForestFire, onClicked(), _d.data(), Impl::toggleForestFire )
  CONNECT( _d->btnForestGrow, onClicked(), _d.data(), Impl::toggleForestGrow )
  CONNECT( _d->btnBorderMoving, onClicked(), _d.data(), Impl::toggleBorderMoving )

  INIT_WIDGET_FROM_UI( PushButton*, btnClose )
  CONNECT( btnClose, onClicked(), this, CityOptions::deleteLater );
  if( btnClose ) btnClose->setFocus();

  _d->update();
  setModal();
}

CityOptions::~CityOptions() {}

void CityOptions::Impl::toggleDebug()
{
  Widget* menu = findDebugMenu( btnDebugEnabled->ui() );
  if( menu )
  {
    menu->setVisible( !menu->visible() );
  }
  update();
}

void CityOptions::Impl::changeCityOption( PlayerCity::OptionType option, int delta )
{
  int value = city->getOption( option );
  city->setOption( option, math::clamp<int>( value + delta, 0, 9999 ) );
  update();
}

void CityOptions::Impl::setAutoText(Widget *widget, const std::string& text, bool enabled )
{
  setAutoText( widget, utils::format( 0xff, "##%s_%s##", text.c_str(), enabled ? "on" : "off" ) );
}

void CityOptions::Impl::setAutoText(Widget *widget, const std::string& text)
{
  if( widget )
  {
    widget->setText( _(text) );
    if( utils::endsWith( text, "##" ) )
    {
      std::string tlp = text.substr( 0, text.length() - 2 );
      tlp.append( "_tlp##" );
      widget->setTooltipText( _( tlp ) );
    }
  }
}

void CityOptions::Impl::increaseFireRisk() { changeCityOption( PlayerCity::fireKoeff, +10 ); }
void CityOptions::Impl::decreaseFireRisk() { changeCityOption( PlayerCity::fireKoeff, -10 ); }
void CityOptions::Impl::increaseCollapseRisk() { changeCityOption( PlayerCity::collapseKoeff, +10 ); }
void CityOptions::Impl::decreaseCollapseRisk() { changeCityOption( PlayerCity::collapseKoeff, -10 ); }

void CityOptions::Impl::toggleCityOption(PlayerCity::OptionType option)
{
  int value = city->getOption( option );
  city->setOption( option, value > 0 ? 0 : 1 );
  update();
}

void CityOptions::Impl::toggleLockInfobox()
{
  bool value = SETTINGS_VALUE( lockInfobox );  
  SETTINGS_SET_VALUE( lockInfobox, !value );
  infobox::Manager::instance().setBoxLock( !value );
  update();
}

void CityOptions::Impl::toggleDifficulty()
{
  int value = city->getOption( PlayerCity::difficulty );
  value = (value+1)%game::difficulty::count;
  city->setOption( PlayerCity::difficulty, value );
  update();
}

void CityOptions::Impl::toggleMetrics()
{
  int value = SETTINGS_VALUE( metricSystem );
  value = (value+1)%metric::Measure::count;
  metric::Measure::setMode( (metric::Measure::Mode)value );
  SETTINGS_SET_VALUE( metricSystem, value );
  update();
}

void CityOptions::Impl::toggleBorderMoving()
{
  bool value = DrawOptions::instance().isFlag( DrawOptions::borderMoving );
  DrawOptions::instance().setFlag( DrawOptions::borderMoving, !value );
  update();
}

void CityOptions::Impl::toggleAndroidBarEnabled()
{
  bool value = SETTINGS_VALUE( showTabletMenu );
  SETTINGS_SET_VALUE( showTabletMenu, !value );

  Widget* widget = btnAnroidBarEnabled->ui()->findWidget( Hash( "AndroidActionsBar" ) );
  if( widget )
    widget->setVisible( !value );

  update();
}

void CityOptions::Impl::toggleShowTooltips()
{
  bool value = SETTINGS_VALUE( tooltipEnabled );
  SETTINGS_SET_VALUE( tooltipEnabled, !value );
  if( btnShowTooltips )
  {
    btnShowTooltips->ui()->setFlag( Ui::showTooltips, !value );
  }
  update();
}

void CityOptions::Impl::toggleLegionAttack() { toggleCityOption( PlayerCity::legionAttack ); }
void CityOptions::Impl::toggleGods() { toggleCityOption( PlayerCity::godEnabled ); }
void CityOptions::Impl::toggleBarbarianAttack() {  toggleCityOption( PlayerCity::barbarianAttack ); }
void CityOptions::Impl::toggleC3Gameplay()  {  toggleCityOption( PlayerCity::c3gameplay ); }
void CityOptions::Impl::toggleZoomEnabled() {  toggleCityOption( PlayerCity::zoomEnabled ); }
void CityOptions::Impl::invertZoom()  {  toggleCityOption( PlayerCity::zoomInvert ); }
void CityOptions::Impl::toggleHighlightBuilding() { toggleCityOption( PlayerCity::highlightBuilding ); }
void CityOptions::Impl::toggleWarnings()  {  toggleCityOption( PlayerCity::warningsEnabled ); }
void CityOptions::Impl::toggleDestroyEpidemicHouses() { toggleCityOption( PlayerCity::destroyEpidemicHouses ); }
void CityOptions::Impl::toggleForestFire() { toggleCityOption( PlayerCity::forestFire ); }
void CityOptions::Impl::toggleForestGrow() { toggleCityOption( PlayerCity::forestGrow ); }

void CityOptions::Impl::toggleLeftMiddleMouse()
{
  bool value = DrawOptions::instance().isFlag( DrawOptions::mmbMoving );
  DrawOptions::instance().setFlag( DrawOptions::mmbMoving, !value );
  update();
}

void CityOptions::Impl::toggleUseBatching()
{
  bool value = gfx::Engine::instance().getFlag( gfx::Engine::batching ) > 0;
  gfx::Engine::instance().setFlag( gfx::Engine::batching, !value );
  update();
}

void CityOptions::Impl::toggleCcUseAI()
{
  bool value = SETTINGS_VALUE( ccUseAI );
  SETTINGS_SET_VALUE( ccUseAI, !value );

  world::EmpirePtr empire = city->empire();
  world::CityList cities = empire->cities();

  foreach( it, cities )
  {
    (*it)->setModeAI( !value ? world::City::indifferent : world::City::inactive );
  }

  update();
}

Widget* CityOptions::Impl::findDebugMenu( Ui* ui )
{
  const Widgets& children = ui->rootWidget()->children();
  foreach( it, children )
  {
    TopMenu* ret = safety_cast<TopMenu*>( *it );
    if( ret != 0 )
    {
      return ret->findItem( "Debug" );
    }
  }

  return 0;
}

void CityOptions::Impl::update()
{
  int index = 0;
  while( options[index].alive )
  {
    OptionInfo& info = options[index];
    PushButton* button = findChildA<PushButton*>( info.btnName, true, widget );

    if( button )
    {
      bool value = false;
      if( info.city_option >= 0 ) value = city->getOption( (PlayerCity::OptionType)info.city_option ) > 0;
      else if( strcmp( info.settings_option, "" ) != 0 ) value = game::Settings::get( info.settings_option );

      setAutoText( button, value ? info.text_on : info.text_off );
    }
    index++;
  }

  if( btnDebugEnabled )
  {
    Widget* menu = findDebugMenu( btnDebugEnabled->ui() );
    setAutoText( btnDebugEnabled, "city_debug", (menu ? menu->visible() : false) );
  }

  if( btnMmbMoving )
  {
    bool value = DrawOptions::instance().isFlag( DrawOptions::mmbMoving ) > 0;
    setAutoText( btnMmbMoving, value ? "##city_mmbmoving##" : "##city_lmbmoving##" );
  }

  if( lbFireRisk )
  {
    int value = city->getOption( PlayerCity::fireKoeff );
    lbFireRisk->setText( utils::format( 0xff, "%s %d %%", "Fire risk", value ) );
  }

  if( lbCollapseRisk )
  {
    int value = city->getOption( PlayerCity::collapseKoeff );
    lbCollapseRisk->setText( utils::format( 0xff, "%s %d %%", "Collapse risk", value ) );
  }

  if( btnDifficulty )
  {
    int value = city->getOption( PlayerCity::difficulty );
    std::string text = utils::format( 0xff, "##city_df_%s##", game::difficulty::name[ value ] );
    setAutoText( btnDifficulty, text );
  }

  if( btnMetrics )
  {
    std::string text = utils::format( 0xff, "%s: %s" , _("##city_metric##"), _(metric::Measure::measureType()) );
    setAutoText( btnMetrics, text );
  }


  if( btnToggleBatching )
  {
    bool value = gfx::Engine::instance().getFlag( gfx::Engine::batching ) > 0;
    setAutoText( btnToggleBatching, "city_batching", value );
  }

  if( btnBorderMoving )
  {
    bool value = gfx::Engine::instance().getFlag( gfx::Engine::batching ) > 0;
    setAutoText( btnBorderMoving, "city_border_moving", value );
  }
}

}//end namespace dialog

}//end namespace gui
