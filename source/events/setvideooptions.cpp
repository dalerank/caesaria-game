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

#include "setvideooptions.hpp"
#include "gui/video_options_window.hpp"
#include "game/settings.hpp"
#include "game/game.hpp"
#include "gfx/engine.hpp"
#include "core/utils.hpp"
#include "gui/environment.hpp"
#include "core/logger.hpp"

using namespace gui;

namespace events
{

GameEventPtr SetVideoSettings::create()
{
  GameEventPtr ret( new SetVideoSettings() );
  ret->drop();

  return ret;
}

void SetVideoSettings::_exec(Game& game, unsigned int)
{
  dialog::VideoOptions* dialog = new dialog::VideoOptions( game.gui()->rootWidget(),
                                                           game.engine()->modes(),
                                                           game.engine()->isFullscreen() );

  CONNECT( dialog, onSreenSizeChange(), this, SetVideoSettings::_setResolution );
  CONNECT( dialog, onFullScreenChange(), this, SetVideoSettings::_setFullscreen );
}

bool SetVideoSettings::_mayExec(Game&, unsigned int) const { return true; }

void SetVideoSettings::_setResolution(Size newSize)
{
  SETTINGS_SET_VALUE( resolution, newSize );
  game::Settings::save();
}

void SetVideoSettings::_setFullscreen(bool fullscreen)
{
  SETTINGS_SET_VALUE( fullscreen, fullscreen );
  game::Settings::save();
}

}
