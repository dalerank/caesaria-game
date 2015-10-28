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

class SpeedOptions::Impl
{
public:
  struct
  {
    int game;
    int scroll;
  } speed;

  int autosaveInterval;

  struct {
    Signal1<int> onGameSpeedChange;
    Signal1<int> onScrollSpeedChange;
    Signal1<int> onAutosaveIntervalShange;
  } signal;
};

SpeedOptions::SpeedOptions( Widget* parent,
                                    int gameSpeed,
                                    int scrollSpeed,
                                    int autosaveInterval)
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->speed.game = gameSpeed;
  _d->speed.scroll = scrollSpeed;
  _d->autosaveInterval = autosaveInterval;
  GameAutoPause::insertTo( this, true );

  setupUI( ":/gui/speedoptions.gui" );

  setCenter( parent->center() );

  _update();
}

SpeedOptions::~SpeedOptions( void ) {}

bool SpeedOptions::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && guiButtonClicked == event.gui.type )
  {
    int id = event.gui.caller->ID();
    switch( id )
    {
    case 1: case 2: _d->speed.game += (id == 1 ? -10 : +10 ); _update(); break;
    case 11: case 12: _d->speed.scroll += (id == 11 ? -10 : +10 ); _update(); break;
    case 21: case 22: _d->autosaveInterval += (id == 21 ? -1 : +1 ); _update(); break;

    case 1001:
    {
      emit _d->signal.onGameSpeedChange( _d->speed.game );
      emit _d->signal.onScrollSpeedChange( _d->speed.scroll );
      emit _d->signal.onAutosaveIntervalShange( _d->autosaveInterval );
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

Signal1<int>& SpeedOptions::onGameSpeedChange() {  return _d->signal.onGameSpeedChange;}
Signal1<int>& SpeedOptions::onScrollSpeedChange(){  return _d->signal.onScrollSpeedChange;}
Signal1<int>& SpeedOptions::onAutosaveIntervalChange(){ return _d->signal.onAutosaveIntervalShange; }

void SpeedOptions::_update()
{
  INIT_WIDGET_FROM_UI( Label*, lbGameSpeedPercent )
  INIT_WIDGET_FROM_UI( Label*, lbScrollSpeedPercent )
  INIT_WIDGET_FROM_UI( Label*, lbAutosaveInterval )

  math::clamp_to( _d->speed.game, 10, 300 );
  math::clamp_to( _d->speed.scroll, 10, 200 );
  math::clamp_to( _d->autosaveInterval, 1, 12 );

  if( lbGameSpeedPercent )   { lbGameSpeedPercent->setText( utils::i2str( _d->speed.game ) + "%" ); }
  if( lbScrollSpeedPercent ) { lbScrollSpeedPercent->setText( utils::i2str( _d->speed.scroll ) + "%" ); }
  if( lbAutosaveInterval )   { lbAutosaveInterval->setText( utils::i2str( _d->autosaveInterval ) + " m." ); }
}

}//end namespace dialog

}//end namespace gui
