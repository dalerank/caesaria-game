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
#include "layers/layer.hpp"
#include "game/settings.hpp"
#include "texturedbutton.hpp"
#include "topmenu.hpp"
#include "game/difficulty.hpp"

using namespace citylayer;

namespace gui
{

class CityOptionsWindow::Impl
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
  Label* lbFireRisk;
  TexturedButton* btnIncreaseFireRisk;
  TexturedButton* btnDecreaseFireRisk;
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
  void toggleBarbarianAttack();
  void toggleC3Gameplay();
  void toggleDifficulty();
  void toggleShowTooltips();
  void toggleLegionAttack();
  void toggleCityOption( PlayerCity::OptionType option );
};

CityOptionsWindow::CityOptionsWindow(Widget* parent, PlayerCityPtr city )
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
  GET_DWIDGET_FROM_UI( _d, btnBarbarianMayAttack )
  GET_DWIDGET_FROM_UI( _d, btnLegionMayAttack )
  GET_DWIDGET_FROM_UI( _d, btnC3Gameplay)
  GET_DWIDGET_FROM_UI( _d, btnShowTooltips )
  GET_DWIDGET_FROM_UI( _d, btnDifficulty )

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

  INIT_WIDGET_FROM_UI( PushButton*, btnClose )
  CONNECT( btnClose, onClicked(), this, CityOptionsWindow::deleteLater );
  if( btnClose ) btnClose->setFocus();

  _d->update();
}

CityOptionsWindow::~CityOptionsWindow() {}


void CityOptionsWindow::Impl::toggleDebug()
{
  Widget* menu = findDebugMenu( btnDebugEnabled->ui() );
  if( menu )
  {
    menu->setVisible( !menu->visible() );
  }
  update();
}

void CityOptionsWindow::Impl::increaseFireRisk()
{
  int value = city->getOption( PlayerCity::fireKoeff );
  city->setOption( PlayerCity::fireKoeff, math::clamp<int>( value + 10, 0, 9999 ) );
  update();
}

void CityOptionsWindow::Impl::decreaseFireRisk()
{
  int value = city->getOption( PlayerCity::fireKoeff );
  city->setOption( PlayerCity::fireKoeff, math::clamp<int>( value - 10, 0, 9999) );
  update();
}

void CityOptionsWindow::Impl::toggleCityOption(PlayerCity::OptionType option)
{
  int value = city->getOption( option );
  city->setOption( option, value > 0 ? 0 : 1 );
  update();
}

void CityOptionsWindow::Impl::toggleLockInfobox()
{
  bool value = SETTINGS_VALUE( lockInfobox );
  SETTINGS_SET_VALUE( lockInfobox, !value );
  update();
}

void CityOptionsWindow::Impl::toggleDifficulty()
{
  int value = city->getOption( PlayerCity::difficulty );
  value = (value+1)%game::difficulty::count;
  city->setOption( PlayerCity::difficulty, value );
  update();
}

void CityOptionsWindow::Impl::toggleShowTooltips()
{
  bool value = SETTINGS_VALUE( tooltipEnabled );
  SETTINGS_SET_VALUE( tooltipEnabled, !value );
  if( btnShowTooltips )
  {
    btnShowTooltips->ui()->setFlag( Ui::showTooltips, !value );
  }
  update();
}

void CityOptionsWindow::Impl::toggleLegionAttack() { toggleCityOption( PlayerCity::legionAttack ); }
void CityOptionsWindow::Impl::toggleGods() { toggleCityOption( PlayerCity::godEnabled ); }
void CityOptionsWindow::Impl::toggleBarbarianAttack() {  toggleCityOption( PlayerCity::barbarianAttack ); }
void CityOptionsWindow::Impl::toggleC3Gameplay()  {  toggleCityOption( PlayerCity::c3gameplay ); }
void CityOptionsWindow::Impl::toggleZoomEnabled() {  toggleCityOption( PlayerCity::zoomEnabled ); }
void CityOptionsWindow::Impl::invertZoom()  {  toggleCityOption( PlayerCity::zoomInvert ); }
void CityOptionsWindow::Impl::toggleWarnings()  {  toggleCityOption( PlayerCity::warningsEnabled ); }

void CityOptionsWindow::Impl::toggleLeftMiddleMouse()
{
  bool value = DrawOptions::instance().isFlag( DrawOptions::mmbMoving );
  DrawOptions::instance().setFlag( DrawOptions::mmbMoving, !value );
  update();
}

Widget* CityOptionsWindow::Impl::findDebugMenu( Ui* ui )
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

void CityOptionsWindow::Impl::update()
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
}

}//end namespace gui
