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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "setsoundoptions.hpp"
#include "gui/sound_options_window.hpp"
#include "sound/engine.hpp"
#include "game/settings.hpp"
#include "game/game.hpp"
#include "core/utils.hpp"
#include "gui/environment.hpp"
#include "core/logger.hpp"

using namespace gui;

namespace events
{

GameEventPtr SetSoundOptions::create()
{
  GameEventPtr ret( new SetSoundOptions() );
  ret->drop();

  return ret;
}

void SetSoundOptions::_exec(Game& game, unsigned int)
{
  audio::Engine& e = audio::Engine::instance();
  dialog::SoundOptions* dialog = new dialog::SoundOptions( game.gui()->rootWidget(),
                                                           e.volume( audio::gameSound ),
                                                           e.volume( audio::ambientSound ),
                                                           e.volume( audio::themeSound ) );

  CONNECT( dialog, onSoundChange(), &e, audio::Engine::setVolume );
  CONNECT( dialog, onClose(), this, SetSoundOptions::_saveSoundSettings );
}

bool SetSoundOptions::_mayExec(Game&, unsigned int) const { return true; }

void SetSoundOptions::_saveSoundSettings()
{
  audio::Engine& se = audio::Engine::instance();
  SETTINGS_SET_VALUE( soundVolume, se.volume( audio::gameSound ) );
  SETTINGS_SET_VALUE( ambientVolume, se.volume( audio::ambientSound ) );
  SETTINGS_SET_VALUE( musicVolume, se.volume( audio::themeSound ) );
  game::Settings::save();
}

}
