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
#include "core/logger.hpp"
#include "gameautopause.hpp"
#include "widget_helper.hpp"
#include "widgetescapecloser.hpp"
#include "contextmenuitem.hpp"
#include "game/infoboxmanager.hpp"
#include "layers/layer.hpp"
#include "game/settings.hpp"
#include "texturedbutton.hpp"
#include "city/build_options.hpp"
#include "topmenu.hpp"
#include "core/variant_map.hpp"
#include "game/difficulty.hpp"
#include "core/metric.hpp"
#include "city/city_option.hpp"
#include "stretch_layout.hpp"
#include "widget_factory.hpp"

using namespace citylayer;

namespace gui
{

static void _setAutoText(Widget *widget, const std::string& text)
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

static void _setAutoText(Widget *widget, const std::string& text, bool enabled )
{
  _setAutoText( widget, fmt::format( "##{0}_{1}##", text, enabled ? "on" : "off" ) );
}

class OptionButton : public PushButton
{
public:
  std::string basicName;
  std::string group;
  std::string type;
  int index;

  typedef std::map<int,std::string> States;
  States states;

  OptionButton( Widget* parent )
   : PushButton( parent, Rect( 0, 0, 1, 1 ), "", -1, false, greyBorderLineSmall )
  {    
    index = 0;
    setMaxSize( Size( 0, 24 ) );
  }

  virtual void setupUI( const VariantMap& ui )
  {
    basicName = ui.get( "base" ).toString();

    setText( "##" + basicName + "_mode##" );

    VariantMap vstates = ui.get( "states" ).toMap();
    for( auto& st : vstates )
      states[ st.second.toInt() ] = st.first;

    VariantList vlink = ui.get( "link" ).toList();
    group = vlink.get( 0 ).toString();
    type = vlink.get( 1 ).toString();
  }

  void update(int value)
  {
    index = 0;
    for( auto& item : states )
    {
      if( item.first == value )
      {
        std::string text = fmt::format( "##{0}_{1}##", basicName, item.second );
        setText( _(text) );
        _setAutoText( this, text );
        break;
      }
      index++;
    }
  }

  Signal3<std::string,std::string,int> onChange;
protected:
  virtual void _btnClicked()
  {
    PushButton::_btnClicked();

    if( states.size() > 0 )
    {
      index = (index+1) % states.size();
      States::iterator it = states.begin();
      std::advance( it, index );

      emit onChange(group,type,it->first);
    }
  }
};

REGISTER_CLASS_IN_WIDGETFACTORY(OptionButton)

namespace dialog
{

class CityOptions::Impl
{
public:
  GameAutoPause locker;
  Widget* widget;  
  PushButton* btnToggleBatching;
  PushButton* btnDifficulty;
  PushButton* btnMetrics;
  PushButton* btnC3Gameplay;
  PushButton* btnRoadBlocks;

  Label* lbFireRisk;
  Label* lbCollapseRisk;
  TexturedButton* btnIncreaseFireRisk;
  TexturedButton* btnDecreaseFireRisk;
  TexturedButton* btnIncreaseCollapseRisk;
  TexturedButton* btnDecreaseCollapseRisk;

  PlayerCityPtr city;

  void update();
  Widget* findDebugMenu(Ui *ui);
  void increaseFireRisk();
  void decreaseFireRisk();
  void increaseCollapseRisk();
  void decreaseCollapseRisk();
  void toggleDifficulty();
  void toggleRoadBlocks();
  void toggleUseBatching();
  void toggleMetrics();
  void toggleC3gameplay();
  void enableC3gameplay();
  void toggleCityOption( PlayerCity::OptionType option );
  void changeCityOption( PlayerCity::OptionType option, int delta);
  void changeShowTooltips(std::string,std::string,int value);
  void chagetInfoboxLock(std::string,std::string,int value);
  void resolveOptionChange(std::string group, std::string name, int value);
  void changeDebugVisible(std::string group, std::string name, int value);
  void changeAndroidBarVisible(std::string group, std::string name, int value);
  void changeCcAi(std::string group, std::string name, int value);
};

CityOptions::CityOptions( Widget* parent, PlayerCityPtr city )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->city = city;
  _d->locker.activate();
  _d->widget = this;
  Window::setupUI( ":/gui/cityoptions.gui" );

  WidgetEscapeCloser::insertTo( this );

  setCenter( parent->center() );

  GET_DWIDGET_FROM_UI( _d, lbFireRisk )
  GET_DWIDGET_FROM_UI( _d, btnIncreaseFireRisk )
  GET_DWIDGET_FROM_UI( _d, btnDecreaseFireRisk )
  GET_DWIDGET_FROM_UI( _d, lbCollapseRisk )
  GET_DWIDGET_FROM_UI( _d, btnC3Gameplay )
  GET_DWIDGET_FROM_UI( _d, btnIncreaseCollapseRisk )
  GET_DWIDGET_FROM_UI( _d, btnDecreaseCollapseRisk )

  GET_DWIDGET_FROM_UI( _d, btnDifficulty )
  GET_DWIDGET_FROM_UI( _d, btnToggleBatching )
  GET_DWIDGET_FROM_UI( _d, btnMetrics )
  GET_DWIDGET_FROM_UI( _d, btnRoadBlocks )

  INIT_WIDGET_FROM_UI(OptionButton*, btnShowTooltips )
  INIT_WIDGET_FROM_UI(OptionButton*, btnAnroidBarEnabled )
  INIT_WIDGET_FROM_UI(OptionButton*, btnDebugEnabled )
  INIT_WIDGET_FROM_UI(OptionButton*, btnToggleCcUseAI )

  CONNECT( btnShowTooltips, onChange, _d.data(), Impl::changeShowTooltips )
  CONNECT( btnDebugEnabled, onChange, _d.data(), Impl::changeDebugVisible )
  CONNECT( btnAnroidBarEnabled, onChange, _d.data(), Impl::changeAndroidBarVisible )
  CONNECT( btnToggleCcUseAI, onChange, _d.data(), Impl::changeCcAi )

  CONNECT( _d->btnIncreaseFireRisk, onClicked(), _d.data(), Impl::increaseFireRisk )
  CONNECT( _d->btnDecreaseFireRisk, onClicked(), _d.data(), Impl::decreaseFireRisk )

  CONNECT( _d->btnDifficulty, onClicked(), _d.data(), Impl::toggleDifficulty )
  CONNECT( _d->btnIncreaseCollapseRisk, onClicked(), _d.data(), Impl::increaseCollapseRisk )
  CONNECT( _d->btnDecreaseCollapseRisk, onClicked(), _d.data(), Impl::decreaseCollapseRisk )
  CONNECT( _d->btnToggleBatching, onClicked(), _d.data(), Impl::toggleUseBatching )
  CONNECT( _d->btnMetrics, onClicked(), _d.data(), Impl::toggleMetrics )
  CONNECT( _d->btnRoadBlocks, onClicked(), _d.data(), Impl::toggleRoadBlocks )
  CONNECT( _d->btnC3Gameplay, onClicked(), _d.data(), Impl::toggleC3gameplay )

  INIT_WIDGET_FROM_UI( PushButton*, btnClose )
  CONNECT( btnClose, onClicked(), this, CityOptions::deleteLater );
  if( btnClose ) btnClose->setFocus();

  auto buttons = findChildren<OptionButton*>( true );
  for( auto btn : buttons )
    CONNECT( btn, onChange, _d.data(), Impl::resolveOptionChange );

  _d->update();
  setModal();
}

CityOptions::~CityOptions() {}

void CityOptions::setupUI(const VariantMap& ui)
{
  Window::setupUI( ui );
}

void CityOptions::Impl::changeCityOption( PlayerCity::OptionType option, int delta )
{
  int value = city->getOption( option );
  city->setOption( option, math::clamp<int>( value + delta, 0, 9999 ) );
  update();
}

void gui::dialog::CityOptions::Impl::chagetInfoboxLock(std::string, std::string, int value)
{
  infobox::Manager::instance().setBoxLock( value > 0 );
}

void CityOptions::Impl::resolveOptionChange(std::string group, std::string name, int value)
{
  if( group == "city" )
  {
     PlayerCity::OptionType option = city::findOption( name );
     if( option != -1 )
       city->setOption( option, value );
  }
  else if( group == "game" )
  {
     game::Settings::set( name, value );
  }
  else if( group == "draw" )
  {
    DrawOptions::Flag flag = DrawOptions::findFlag( name );
    DrawOptions::takeFlag( flag, value > 0 );
  }
  update();
}

void CityOptions::Impl::changeDebugVisible(std::string,std::string, int value)
{
  Widget* menu = findDebugMenu( widget->ui() );
  if( menu )
    menu->setVisible( value );
}

void CityOptions::Impl::changeAndroidBarVisible(std::string, std::string, int value)
{
  Widget* abar = widget->ui()->findWidget( Hash( "AndroidActionsBar" ) );
  if( abar )
    abar->setVisible( value );
}

void CityOptions::Impl::changeCcAi(std::string, std::string, int value)
{
  world::CityList cities = city->empire()->cities();

  for( auto rcity : cities )
    rcity->setModeAI( value ? world::City::indifferent : world::City::inactive );
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

void CityOptions::Impl::toggleC3gameplay()
{
  int value = city->getOption( PlayerCity::c3gameplay );
  value = !value;
  if( value )
  {
    auto dlg = dialog::Confirmation( widget->ui(), "Gameplay", "Will be enable C3 gameplay mode. Continue?", true );
    dlg->show();

    CONNECT( dlg, onOk(), this, Impl::enableC3gameplay )
  }
}

void CityOptions::Impl::enableC3gameplay() { city->setOption( PlayerCity::c3gameplay, true ); }

void CityOptions::Impl::changeShowTooltips(std::string,std::string,int value)
{
  widget->ui()->setFlag( Ui::showTooltips, value );
}

void CityOptions::Impl::toggleRoadBlocks()
{
  city::development::Options opts;
  opts = city->buildOptions();
  opts.toggleBuildingAvailable( object::roadBlock );
  city->setBuildOptions( opts );
  update();
}

void CityOptions::Impl::toggleUseBatching()
{
  bool value = gfx::Engine::instance().getFlag( gfx::Engine::batching ) > 0;
  gfx::Engine::instance().setFlag( gfx::Engine::batching, !value );
  update();
}

Widget* CityOptions::Impl::findDebugMenu( Ui* ui )
{
  auto topmenu = ui->rootWidget()->children().select<TopMenu>();
  for( auto w : topmenu )
    return w->findItem( "Debug" );

  return nullptr;
}

void CityOptions::Impl::update()
{
  auto buttons = widget->findChildren<OptionButton*>( true );
  for( auto btn : buttons )
  {
    int value = 0;
    if( btn->group == "city" )
    {
       PlayerCity::OptionType option = city::findOption( btn->type );
       if( option != -1 )
         value = city->getOption( option ) > 0 ? 1 : 0;
    }
    else if( btn->group == "game" )
    {
       value = game::Settings::get( btn->type );
    }
    else if( btn->group == "draw" )
    {
      DrawOptions::Flag flag = DrawOptions::findFlag( btn->type );
      value = DrawOptions::getFlag( flag );
    }
    else if( btn->group == "gfx" )
    {

    }

    btn->update( value );
  }

  if( lbFireRisk )
  {
    int value = city->getOption( PlayerCity::fireKoeff );
    lbFireRisk->setText( fmt::format( "{0} {1} %", "Fire risk", value ) );
  }

  if( lbCollapseRisk )
  {
    int value = city->getOption( PlayerCity::collapseKoeff );
    lbCollapseRisk->setText( utils::format( 0xff, "%s %d %%", "Collapse risk", value ) );
  }

  if( btnDifficulty )
  {
    int value = city->getOption( PlayerCity::difficulty );
    std::string text = fmt::format( "##city_df_%s##", game::difficulty::name[ value ] );
    _setAutoText( btnDifficulty, text );
  }

  if( btnC3Gameplay )
  {
    int value = city->getOption( PlayerCity::c3gameplay );
    std::string text = fmt::format( "##city_c3rules_%s##", value ? "on" : "off" );
    _setAutoText( btnDifficulty, text );
  }

  if( btnMetrics )
  {
    std::string text = fmt::format( "%s: %s" , _("##city_metric##"), _(metric::Measure::measureType()) );
    _setAutoText( btnMetrics, text );
  }

  if( btnToggleBatching )
  {
    bool value = gfx::Engine::instance().getFlag( gfx::Engine::batching ) > 0;
    _setAutoText( btnToggleBatching, "city_batching", value );
  }

  if( btnRoadBlocks )
  {
    const city::development::Options& opts = city->buildOptions();
    bool value = opts.isBuildingAvailable(object::roadBlock );
    _setAutoText( btnRoadBlocks, "city_roadblock", value );
  }
}

}//end namespace dialog

}//end namespace gui
