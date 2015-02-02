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

#include "window_gamespeed_options.hpp"
#include "gfx/engine.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "label.hpp"
#include "listbox.hpp"
#include "core/utils.hpp"
#include "gameautopause.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"

namespace gui
{

namespace dialog
{

class GameSpeedOptions::Impl
{
public:
  GameAutoPause locker;
  Signal1<int> onGameSpeedChangeSignal;
  Signal1<int> onScrollSpeedChangeSignal;
  Signal1<int> onAutosaveIntervalShangeSignal;
  int speedValue, scrollValue, autosaveInterval;
};

GameSpeedOptions::GameSpeedOptions( Widget* parent,
                                    int gameSpeed,
                                    int scrollSpeed,
                                    int autosaveInterval)
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->speedValue = gameSpeed;
  _d->scrollValue = scrollSpeed;
  _d->autosaveInterval = autosaveInterval;
  _d->locker.activate();

  setupUI( ":/gui/speedoptions.gui" );

  setCenter( parent->center() );

  _update();
}

GameSpeedOptions::~GameSpeedOptions( void ) {}

bool GameSpeedOptions::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && guiButtonClicked == event.gui.type )
  {
    int id = event.gui.caller->ID();
    switch( id )
    {
    case 1: case 2: _d->speedValue += (id == 1 ? -10 : +10 ); _update(); break;
    case 11: case 12: _d->scrollValue += (id == 11 ? -10 : +10 ); _update(); break;
    case 21: case 22: _d->autosaveInterval += (id == 21 ? -1 : +1 ); _update(); break;

    case 1001:
    {
      emit _d->onGameSpeedChangeSignal( _d->speedValue );
      emit _d->onScrollSpeedChangeSignal( _d->scrollValue );
      emit _d->onAutosaveIntervalShangeSignal( _d->autosaveInterval );
      deleteLater();
    }
    break;

    case 1002:
      deleteLater();
    break;
    }
  }

  return Widget::onEvent( event );
}

Signal1<int>& GameSpeedOptions::onGameSpeedChange() {  return _d->onGameSpeedChangeSignal;}
Signal1<int>& GameSpeedOptions::onScrollSpeedChange(){  return _d->onScrollSpeedChangeSignal;}
Signal1<int>& GameSpeedOptions::onAutosaveIntervalChange(){ return _d->onAutosaveIntervalShangeSignal; }

void GameSpeedOptions::_update()
{
  INIT_WIDGET_FROM_UI( Label*, lbGameSpeedPercent )
  INIT_WIDGET_FROM_UI( Label*, lbScrollSpeedPercent )
  INIT_WIDGET_FROM_UI( Label*, lbAutosaveInterval )

  _d->speedValue = math::clamp( _d->speedValue, 10, 300 );
  _d->scrollValue = math::clamp( _d->scrollValue, 10, 200 );
  _d->autosaveInterval = math::clamp( _d->autosaveInterval, 1, 12 );

  if( lbGameSpeedPercent ) { lbGameSpeedPercent->setText( utils::i2str( _d->speedValue ) + "%" ); }
  if( lbScrollSpeedPercent ) { lbScrollSpeedPercent->setText( utils::i2str( _d->scrollValue ) + "%" ); }
  if( lbAutosaveInterval ) { lbAutosaveInterval->setText( utils::i2str( _d->autosaveInterval ) + " m." ); }
}

}//end namespace dialog

}//end namespace gui
