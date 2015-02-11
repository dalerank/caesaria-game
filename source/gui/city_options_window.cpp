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
#include "topmenu.hpp"

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
  PlayerCityPtr city;

  void update();
  void toggleDebug();
  void toggleGods();
  void toggleZoomEnabled();
  void invertZoom();
  void toggleWarnings();
  Widget* findDebugMenu(Ui *ui);
};

CityOptionsWindow::CityOptionsWindow(Widget* parent, PlayerCityPtr city )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->city = city;
  _d->locker.activate();
  setupUI( ":/gui/cityoptions.gui" );

  setCenter( parent->center() );

  GET_DWIDGET_FROM_UI( _d, btnGodEnabled )
  GET_DWIDGET_FROM_UI( _d, btnWarningsEnabled )
  GET_DWIDGET_FROM_UI( _d, btnZoomEnabled )
  GET_DWIDGET_FROM_UI( _d, btnInvertZoom )
  GET_DWIDGET_FROM_UI( _d, btnDebugEnabled )

  CONNECT( _d->btnGodEnabled, onClicked(), _d.data(), Impl::toggleGods );
  CONNECT( _d->btnWarningsEnabled, onClicked(), _d.data(), Impl::toggleWarnings );
  CONNECT( _d->btnZoomEnabled, onClicked(), _d.data(), Impl::toggleZoomEnabled )
  CONNECT( _d->btnInvertZoom, onClicked(), _d.data(), Impl::invertZoom )
  CONNECT( _d->btnDebugEnabled, onClicked(), _d.data(), Impl::toggleDebug )

  INIT_WIDGET_FROM_UI( PushButton*, btnClose )
  CONNECT( btnClose, onClicked(), this, CityOptionsWindow::deleteLater );

  _d->update();
}

CityOptionsWindow::~CityOptionsWindow() {}

void CityOptionsWindow::Impl::toggleGods()
{
  bool value = city->getOption( PlayerCity::godEnabled );
  city->setOption( PlayerCity::godEnabled, value > 0 ? 0 : 1 );
  update();
}

void CityOptionsWindow::Impl::toggleDebug()
{
  Widget* menu = findDebugMenu( btnDebugEnabled->ui() );
  if( menu )
  {
    menu->setVisible( !menu->visible() );
  }
  update();
}

void CityOptionsWindow::Impl::toggleZoomEnabled()
{
  bool value = city->getOption( PlayerCity::zoomEnabled );
  city->setOption( PlayerCity::zoomEnabled, value > 0 ? 0 : 1 );
  update();
}

void CityOptionsWindow::Impl::invertZoom()
{
  bool value = city->getOption( PlayerCity::zoomInvert );
  city->setOption( PlayerCity::zoomInvert, value > 0 ? 0 : 1 );
  update();
}

void CityOptionsWindow::Impl::toggleWarnings()
{
  bool value = city->getOption( PlayerCity::warningsEnabled );
  city->setOption( PlayerCity::warningsEnabled, value > 0 ? 0 : 1 );
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
}

}//end namespace gui
