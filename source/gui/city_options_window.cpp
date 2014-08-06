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

namespace gui
{

class CityOptionsWindow::Impl
{
public:
  GameAutoPause locker;
  PushButton* btnGodEnabled;
  PlayerCityPtr city;

  void update();
  void toggleGods();
};

CityOptionsWindow::CityOptionsWindow(Widget* parent, PlayerCityPtr city )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->city = city;
  _d->locker.activate();
  setupUI( ":/gui/cityoptions.gui" );

  setCenter( parent->center() );
  _d->btnGodEnabled = findChildA<PushButton*>( "btnGodEnabled", true, this );
  CONNECT( _d->btnGodEnabled, onClicked(), _d.data(), Impl::toggleGods );

  PushButton* btnClose = findChildA<PushButton*>( "btnClose", true, this );
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

void CityOptionsWindow::Impl::update()
{
  if( btnGodEnabled )
  {
    btnGodEnabled->setText( city->getOption( PlayerCity::godEnabled ) > 0 ? _("##city_opts_god_on##") : _("##city_opts_god_off##") );
  }
}

}//end namespace gui
