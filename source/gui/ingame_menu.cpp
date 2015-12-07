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

#include "ingame_menu.hpp"
#include "core/hash.hpp"
#include "widgetescapecloser.hpp"
#include "widget_helper.hpp"
#include "environment.hpp"
#include "core/logger.hpp"

namespace gui
{

class IngameMenu::Impl
{
public:
  PushButton* btnSave;
  PushButton* btnLoad;
  PushButton* btnRestart;
  PushButton* btnMainMenu;
  PushButton* btnExit;

public signals:
  Signal0<> invalidSignal;
};

IngameMenu::IngameMenu(Widget *parent)
  : Window( parent, Rect( 0, 0, 500, 450 ), "", Hash( TEXT(IngameMenu) ) ),
    _d( new Impl )
{
  setupUI( ":/gui/ingamemenu_android.gui" );
  setCenter( parent->center() );
  WidgetEscapeCloser::insertTo( this );

  GET_DWIDGET_FROM_UI( _d, btnSave )
  GET_DWIDGET_FROM_UI( _d, btnLoad  )
  GET_DWIDGET_FROM_UI( _d, btnRestart )
  GET_DWIDGET_FROM_UI( _d, btnMainMenu )
  GET_DWIDGET_FROM_UI( _d, btnExit )

  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnContinue, onClicked(), IngameMenu::deleteLater );

  setModal();
}

IngameMenu::~IngameMenu() {}

Signal0<>& IngameMenu::onExit() { return _d->btnExit ? _d->btnExit->onClicked() : _d->invalidSignal; }
Signal0<>& IngameMenu::onLoad() { return _d->btnLoad ? _d->btnLoad->onClicked() : _d->invalidSignal; }
Signal0<>& IngameMenu::onSave() { return _d->btnSave ? _d->btnSave->onClicked() : _d->invalidSignal; }
Signal0<>& IngameMenu::onRestart() { return _d->btnRestart ? _d->btnRestart->onClicked() : _d->invalidSignal; }
Signal0<>& IngameMenu::onMenu() { return _d->btnMainMenu ? _d->btnMainMenu->onClicked() : _d->invalidSignal; }

}//end namespace gui
