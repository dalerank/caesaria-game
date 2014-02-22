// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "setvideooptions.hpp"
#include "gui/video_options_window.hpp"
#include "game/settings.hpp"
#include "game/game.hpp"
#include "gfx/engine.hpp"
#include "core/stringhelper.hpp"
#include "gui/environment.hpp"
#include "core/logger.hpp"

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
  gui::VideoOptionsWindow* dialog = new gui::VideoOptionsWindow( game.getGui()->getRootWidget(),
                                                                 game.getEngine()->getAvailableModes(),
                                                                 game.getEngine()->isFullscreen() );

  CONNECT( dialog, onSreenSizeChange(), this, SetVideoSettings::_setResolution );
  CONNECT( dialog, onFullScreenChange(), this, SetVideoSettings::_setFullscreen );
}

bool SetVideoSettings::_mayExec(Game&, unsigned int) const { return true; }

void SetVideoSettings::_setResolution(Size newSize)
{
  GameSettings::set( GameSettings::resolution, newSize );
  GameSettings::save();
}

void SetVideoSettings::_setFullscreen(bool fullscreen)
{
  GameSettings::set( GameSettings::fullscreen, fullscreen );
  GameSettings::save();
}

}
