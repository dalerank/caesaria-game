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

#include "showfeastwindow.hpp"
#include "gui/video_options_window.hpp"
#include "game/settings.hpp"
#include "game/game.hpp"
#include "gfx/engine.hpp"
#include "core/stringhelper.hpp"
#include "gui/environment.hpp"
#include "core/logger.hpp"
#include "gui/film_widget.hpp"
#include "game/gamedate.hpp"

namespace events
{

GameEventPtr ShowFeastWindow::create(std::string text, std::string title, std::string receiver)
{
  ShowFeastWindow* e = new ShowFeastWindow();
  e->_text = text;
  e->_receiver = receiver;
  e->_title = title;
  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

bool ShowFeastWindow::_mayExec(Game& game, uint time) const
{
  return true;
}

void ShowFeastWindow::_exec(Game& game, uint)
{
  gui::GuiEnv* env = game.getGui();

  gui::FilmWidget* dlg = new gui::FilmWidget( env->getRootWidget(), "" );
  dlg->setText( _text );
  dlg->setTitle( _title );
  dlg->setReceiver( _receiver );
  dlg->setTime( GameDate::current() );

  game.pause();
  CONNECT( dlg, onClose(), &game, Game::play );
}

}
