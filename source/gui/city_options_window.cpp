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
#include "spinbox.hpp"
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

namespace dialog
{

class CityOptions::Impl
{
public:
  Widget* widget;
  PushButton* btnMetrics;
  PushButton* btnRoadBlocks;

  PlayerCityPtr city;

  void update();
  Widget* findDebugMenu(Ui *ui);
  void toggleMetrics();
  void toggleC3gameplay();
  void enableC3gameplay();
  void changeShowTooltips(std::string,std::string,int value);
  void chagetInfoboxLock(std::string,std::string,int value);
  void changeDebugVisible(std::string group, std::string name, int value);
  void changeAndroidBarVisible(std::string group, std::string name, int value);
  void changeCcAi(std::string group, std::string name, int value);
};

CityOptions::CityOptions( Widget* parent, PlayerCityPtr city )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->city = city;
  _d->widget = this;
  GameAutoPauseWidget::insertTo( this );
  Window::setupUI( ":/gui/cityoptions.gui" );

  GET_DWIDGET_FROM_UI( _d, btnMetrics )

  CONNECT( _d->btnMetrics, onClicked(), _d.data(), Impl::toggleMetrics )

  _d->update();

  WidgetClosers::insertTo( this, KEY_RBUTTON );
  moveToCenter();
  setModal();
}

CityOptions::~CityOptions() {}

void CityOptions::setupUI(const VariantMap& ui)
{
  Window::setupUI( ui );
}

void gui::dialog::CityOptions::Impl::chagetInfoboxLock(std::string, std::string, int value)
{
  infobox::Manager::instance().setBoxLock( value > 0 );
}

void CityOptions::Impl::changeDebugVisible(std::string,std::string, int value)
{
  Widget* menu = findDebugMenu( widget->ui() );
  if( menu )
    menu->setVisible(value>0);
}

void CityOptions::Impl::changeAndroidBarVisible(std::string, std::string, int value)
{
  Widget* abar = widget->ui()->findWidget( Hash( "AndroidActionsBar" ) );
  if( abar )
    abar->setVisible(value>0);
}

void CityOptions::Impl::changeCcAi(std::string, std::string, int value)
{
  world::CityList cities = city->empire()->cities();

  for( auto rcity : cities )
    rcity->setModeAI( value ? world::City::indifferent : world::City::inactive );
}


void CityOptions::Impl::toggleMetrics()
{
  int value = SETTINGS_VALUE( metricSystem );
  value = (value+1)%metric::Measure::count;
  metric::Measure::setMode( (metric::Measure::Mode)value );
  SETTINGS_SET_VALUE( metricSystem, value );
  update();
}

void CityOptions::Impl::changeShowTooltips(std::string,std::string,int value)
{
  widget->ui()->setFlag( Ui::showTooltips, value );
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
  if( btnMetrics )
  {
    std::string text = fmt::format( "{}: {}" , _("##city_metric##"), _(metric::Measure::measureType()) );
    //_setAutoText( btnMetrics, text );
  }
}

}//end namespace dialog

}//end namespace gui
