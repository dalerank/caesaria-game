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

using namespace citylayer;

namespace gui
{

namespace dialog
{

class CityOptions::Impl
{
public:
  GameAutoPause locker;
  PushButton* btnGodEnabled;
  PushButton* btnWarningsEnabled;
  PushButton* btnZoomEnabled;
  PushButton* btnDebugEnabled;
  PushButton* btnInvertZoom;
  PushButton* btnMmbMoving;
  PushButton* btnBarbarianMayAttack;
  PushButton* btnLegionMayAttack;
  PushButton* btnAnroidBarEnabled;
  Label* lbFireRisk;
  TexturedButton* btnIncreaseFireRisk;
  TexturedButton* btnDecreaseFireRisk;

  Label* lbCollapseRisk;
  TexturedButton* btnIncreaseCollapseRisk;
  TexturedButton* btnDecreaseCollapseRisk;

  PushButton* btnLockInfobox;
  PushButton* btnC3Gameplay;
  PushButton* btnShowTooltips;
  PushButton* btnDifficulty;
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
  void toggleAndroidBarEnabled();
  void toggleCityOption( PlayerCity::OptionType option );
  void changeCityOption(PlayerCity::OptionType option, int delta);
};

CityOptions::CityOptions(Widget* parent, PlayerCityPtr city )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->city = city;
  _d->locker.activate();
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

  INIT_WIDGET_FROM_UI( PushButton*, btnClose )
  CONNECT( btnClose, onClicked(), this, CityOptions::deleteLater );
  if( btnClose ) btnClose->setFocus();

  _d->update();
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
void CityOptions::Impl::toggleWarnings()  {  toggleCityOption( PlayerCity::warningsEnabled ); }

void CityOptions::Impl::toggleLeftMiddleMouse()
{
  bool value = DrawOptions::instance().isFlag( DrawOptions::mmbMoving );
  DrawOptions::instance().setFlag( DrawOptions::mmbMoving, !value );
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
  if( btnGodEnabled )
  {
    btnGodEnabled->setText( city->getOption( PlayerCity::godEnabled ) > 0
                              ? _("##city_opts_god_on##")
                              : _("##city_opts_god_off##") );
  }

  if( btnWarningsEnabled )
  {
    btnWarningsEnabled->setText( city->getOption( PlayerCity::warningsEnabled ) > 0
                              ? _("##city_warnings_on##")
                              : _("##city_warnings_off##") );
  }

  if( btnZoomEnabled )
  {
    btnZoomEnabled->setText( city->getOption( PlayerCity::zoomEnabled ) > 0
                              ? _("##city_zoom_on##")
                              : _("##city_zoom_off##") );
  }

  if( btnInvertZoom )
  {
    btnInvertZoom->setText( city->getOption( PlayerCity::zoomInvert ) > 0
                              ? _("##city_zoominv_on##")
                              : _("##city_zoominv_off##") );
  }

  if( btnDebugEnabled )
  {
    Widget* menu = findDebugMenu( btnDebugEnabled->ui() );
    btnDebugEnabled->setText( (menu ? menu->visible() : false)
                                ? _("##city_debug_on##")
                                : _("##city_debug_off##") );
  }

  if( btnMmbMoving )
  {
    bool value = DrawOptions::instance().isFlag( DrawOptions::mmbMoving ) > 0;
    btnMmbMoving->setText( value
                                ? _("##city_mmbmoving##")
                                : _("##city_lmbmoving##") );
  }

  if( btnLockInfobox )
  {
    bool value = SETTINGS_VALUE( lockInfobox );
    btnLockInfobox->setText( value
                                ? _("##city_lockinfo_on##")
                                : _("##city_lockinfo_off##") );
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

  if( btnBarbarianMayAttack )
  {
    int value = city->getOption( PlayerCity::barbarianAttack );
    btnBarbarianMayAttack->setText( value
                                    ? _("##city_barbarian_on##")
                                    : _("##city_barbarian_off##")  );
  }

  if( btnC3Gameplay )
  {
    bool value = city->getOption( PlayerCity::c3gameplay ) > 0;
    btnBarbarianMayAttack->setText( value
                                    ? _("##city_c3rules_on##")
                                    : _("##city_c3rules_off##")  );
  }

  if( btnShowTooltips )
  {
    bool value = SETTINGS_VALUE( tooltipEnabled );
    btnShowTooltips->setText( value
                                    ? _("##city_tooltips_on##")
                                    : _("##city_tooltips_off##")  );
  }

  if( btnDifficulty )
  {
    int value = city->getOption( PlayerCity::difficulty );
    std::string text = utils::format( 0xff, "##city_df_%s##", game::difficulty::name[ value ] );
    btnDifficulty->setText( _(text) );
  }

  if( btnLegionMayAttack )
  {
    int value = city->getOption( PlayerCity::legionAttack );
    btnLegionMayAttack->setText( value
                                    ? _("##city_chastener_on##")
                                    : _("##city_chastener_off##")  );
  }

  if( btnAnroidBarEnabled )
  {
    bool value = SETTINGS_VALUE( showTabletMenu );
    btnAnroidBarEnabled->setText( value
                                    ? _("##city_androidbar_on##")
                                    : _("##city_androidbar_off##")  );
  }
}

}//end namespace dialog

}//end namespace gui
