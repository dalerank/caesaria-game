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
#include "core/stringhelper.hpp"
#include "dialogbox.hpp"
#include "core/gettext.hpp"
#include "environment.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "gameautopause.hpp"
#include "widget_helper.hpp"

namespace gui
{

class CityOptionsWindow::Impl
{
public:
  GameAutoPause locker;
  PushButton* btnGodEnabled;
  PushButton* btnWarningsEnabled;
  PlayerCityPtr city;

  void update();
  void toggleGods();
  void toggleWarnings();
};

CityOptionsWindow::CityOptionsWindow(Widget* parent, PlayerCityPtr city )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->city = city;
  _d->locker.activate();
  setupUI( ":/gui/cityoptions.gui" );

  setCenter( parent->center() );
  PushButton* btnClose;

  GET_WIDGET_FROM_UI( btnClose )
  GET_DWIDGET_FROM_UI( _d, btnGodEnabled )
  GET_DWIDGET_FROM_UI( _d, btnWarningsEnabled )

  CONNECT( _d->btnGodEnabled, onClicked(), _d.data(), Impl::toggleGods );
  CONNECT( _d->btnWarningsEnabled, onClicked(), _d.data(), Impl::toggleWarnings );
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

void CityOptionsWindow::Impl::toggleWarnings()
{
  bool value = city->getOption( PlayerCity::warningsEnabled );
  city->setOption( PlayerCity::warningsEnabled, value > 0 ? 0 : 1 );
  update();
}

void CityOptionsWindow::Impl::update()
{
  if( btnGodEnabled )
  {
    btnGodEnabled->setText( city->getOption( PlayerCity::godEnabled ) > 0 ? _("##city_opts_god_on##") : _("##city_opts_god_off##") );
  }
}

}//end namespace gui
