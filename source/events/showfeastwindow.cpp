// This file is part of CaearIA.
//
// CaearIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaearIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaearIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "showfeastwindow.hpp"
#include "gui/video_options_window.hpp"
#include "game/settings.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "city/funds.hpp"
#include "gfx/engine.hpp"
#include "core/utils.hpp"
#include "gui/environment.hpp"
#include "core/logger.hpp"
#include "gui/film_widget.hpp"
#include "game/gamedate.hpp"

namespace events
{

GameEventPtr ShowFeastival::create(std::string text, std::string title, std::string receiver, std::string video)
{
  ShowFeastival* e = new ShowFeastival();
  e->_text = text;
  e->_receiver = receiver;
  e->_title = title;
  e->_video = video;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

bool ShowFeastival::_mayExec(Game& game, unsigned int time) const
{
  return true;
}

void ShowFeastival::_exec(Game& game, unsigned int)
{
  gui::Ui* env = game.gui();

  gui::FilmWidget* dlg = new gui::FilmWidget( env->rootWidget(), _video );
  dlg->setText( _text );
  dlg->setTitle( _title );
  dlg->setReceiver( _receiver );
  dlg->setTime( game::Date::current() );
}

}
