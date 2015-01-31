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

#include "showtutorialwindow.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "core/variant_map.hpp"
#include "gui/tutorial_window.hpp"
#include "gui/environment.hpp"
#include "factory.hpp"

namespace events
{

REGISTER_EVENT_IN_FACTORY(ShowTutorial, "tutorial_window")

GameEventPtr ShowTutorial::create(std::string tutorial)
{
  ShowTutorial* e = new ShowTutorial();
  e->_tutorial = tutorial;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void ShowTutorial::load(const VariantMap& opt)
{
  _tutorial = opt.get( "tutorial" ).toString();
}

void ShowTutorial::_exec(Game& game, unsigned int)
{
  if( _tutorial.empty() )
    return;

  gui::TutorialWindow* wnd = new gui::TutorialWindow( game.gui()->rootWidget(), _tutorial );
  wnd->show();
}

bool ShowTutorial::_mayExec(Game&, unsigned int) const { return true; }

}
