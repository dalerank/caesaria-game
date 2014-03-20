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
#include "game/settings.hpp"
#include "label.hpp"
#include "core/stringhelper.hpp"
#include "sound/constants.hpp"

namespace gui
{

class SoundOptionsWindow::Impl
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

public oc3_signals:
  Signal2<audio::SoundType, int> onSoundChangeSignal;
};

SoundOptionsWindow::SoundOptionsWindow(Widget* parent, int gameSound, int ambientSound, int themeSound )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->locker.activate();
  setupUI( GameSettings::rcpath( "/gui/soundoptions.gui" ) );

  setCenter( parent->center() );

  Impl::Sounds tmp = { gameSound, ambientSound, themeSound };
  _d->save = tmp;
  _d->current = tmp;

  _update();
}

SoundOptionsWindow::~SoundOptionsWindow( void ) {}

bool SoundOptionsWindow::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->getID();
    switch( id )
    {
    case 1: case 2: _d->current.game += (id == 1 ? -10 : +10 );       _update(); break;
    case 11: case 12: _d->current.ambient += (id == 11 ? -10 : +10 ); _update(); break;
    case 21: case 22: _d->current.theme += (id == 21 ? -10 : +10 );   _update(); break;
    case 1001: deleteLater(); break;
    case 1002:
    {
      _d->onSoundChangeSignal.emit( audio::gameSound, _d->save.game );
      _d->onSoundChangeSignal.emit( audio::ambientSound, _d->save.ambient );
      _d->onSoundChangeSignal.emit( audio::themeSound, _d->save.theme );
      deleteLater();
    }
    break;

    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal2<audio::SoundType, int>&SoundOptionsWindow::onSoundChange()
{
  return _d->onSoundChangeSignal;
}

void SoundOptionsWindow::_update()
{
  Label* lbGameSound = findChildA<Label*>( "lbGameSoundPercent", true, this );
  Label* lbAmbientSound = findChildA<Label*>( "lbAmbientSoundPercent", true, this );
  Label* lbThemeSound = findChildA<Label*>( "lbThemeSoundPercent", true, this );

  if( lbGameSound ) { lbGameSound->setText( StringHelper::format( 0xff, "%d%%", _d->current.game ) ); }
  if( lbAmbientSound ) { lbAmbientSound->setText( StringHelper::format( 0xff, "%d%%", _d->current.ambient ) ); }
  if( lbThemeSound ) { lbThemeSound->setText( StringHelper::format( 0xff, "%d%%", _d->current.theme ) ); }

  _d->onSoundChangeSignal.emit( audio::gameSound,_d->current.game );
  _d->onSoundChangeSignal.emit( audio::ambientSound, _d->current.ambient );
  _d->onSoundChangeSignal.emit( audio::themeSound,_d->current.theme );
}

}//end namespace gui
