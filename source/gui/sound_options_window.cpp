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

#include "sound_options_window.hpp"
#include "gameautopause.hpp"
#include "core/event.hpp"
#include "label.hpp"
#include "widget_helper.hpp"
#include "core/utils.hpp"
#include "sound/constants.hpp"
#include "core/logger.hpp"
#include "widgetescapecloser.hpp"

namespace gui
{

namespace dialog
{

class SoundOptions::Impl
{
public:
  GameAutoPause locker;

  struct Sounds
  {
    int game,
        ambient,
        theme;
  };

  Sounds current, save;

public signals:
  Signal2<audio::SoundType, int> onSoundChangeSignal;
  Signal0<> onCloseSignal;
};

SoundOptions::SoundOptions(Widget* parent, int gameSound, int ambientSound, int themeSound )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->locker.activate();
  setupUI( ":/gui/soundoptions.gui" );

  setCenter( parent->center() );

  Impl::Sounds tmp = { gameSound, ambientSound, themeSound };
  _d->save = tmp;
  _d->current = tmp;

  _update();
  WidgetEscapeCloser::insertTo( this );

  INIT_WIDGET_FROM_UI( PushButton*, btnOk )
  if( btnOk ) btnOk->setFocus();
}

SoundOptions::~SoundOptions( void ) {}

bool SoundOptions::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->ID();
    switch( id )
    {
    case 1: case 2: _d->current.game += (id == 1 ? -10 : +10 );       _update(); break;
    case 11: case 12: _d->current.ambient += (id == 11 ? -10 : +10 ); _update(); break;
    case 21: case 22: _d->current.theme += (id == 21 ? -10 : +10 );   _update(); break;

    case 1001:
      emit _d->onCloseSignal();
      deleteLater();
    break;

    case 1002:
    {
      emit _d->onSoundChangeSignal( audio::gameSound, _d->save.game );
      emit _d->onSoundChangeSignal( audio::ambientSound, _d->save.ambient );
      emit _d->onSoundChangeSignal( audio::themeSound, _d->save.theme );
      emit _d->onCloseSignal();
      deleteLater();
    }
    break;

    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal2<audio::SoundType, int>& SoundOptions::onSoundChange() {  return _d->onSoundChangeSignal;}
Signal0<>& SoundOptions::onClose(){  return _d->onCloseSignal;}

void SoundOptions::_update()
{
  INIT_WIDGET_FROM_UI( Label*, lbGameSoundPercent )
  INIT_WIDGET_FROM_UI( Label*, lbAmbientSoundPercent )
  INIT_WIDGET_FROM_UI( Label*, lbThemeSoundPercent )

  _d->current.game = math::clamp( _d->current.game, 0, 100 );
  _d->current.ambient = math::clamp( _d->current.ambient, 0, 100 );
  _d->current.theme = math::clamp( _d->current.theme, 0, 100 );

  if( lbGameSoundPercent ) { lbGameSoundPercent->setText( utils::format( 0xff, "%d%%", _d->current.game ) ); }
  if( lbAmbientSoundPercent ) { lbAmbientSoundPercent->setText( utils::format( 0xff, "%d%%", _d->current.ambient ) ); }
  if( lbThemeSoundPercent ) { lbThemeSoundPercent->setText( utils::format( 0xff, "%d%%", _d->current.theme ) ); }

  emit _d->onSoundChangeSignal( audio::gameSound,_d->current.game );
  emit _d->onSoundChangeSignal( audio::ambientSound, _d->current.ambient );
  emit _d->onSoundChangeSignal( audio::themeSound,_d->current.theme );
}

}//end namespace dialog

}//end namespace gui
