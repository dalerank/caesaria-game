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
#include "gui/tutorial_window.hpp"
#include "gui/environment.hpp"

namespace events
{

GameEventPtr ShowTutorialWindow::create(std::string tutorial )
{
  ShowTutorialWindow* e = new ShowTutorialWindow();
  e->_tutorial = tutorial;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void ShowTutorialWindow::load(const VariantMap& opt)
{
  _tutorial = opt.get( "tutorial" ).toString();
}

void ShowTutorialWindow::exec(Game& game)
{
  if( _tutorial.empty() )
    return;

  gui::TutorialWindow* wnd = new gui::TutorialWindow( game.getGui()->getRootWidget(), _tutorial );
  wnd->show();
}

}
