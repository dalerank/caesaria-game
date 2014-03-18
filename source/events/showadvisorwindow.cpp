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

#include "showadvisorwindow.hpp"
#include "gui/advisors_window.hpp"
#include "game/game.hpp"
#include "gui/environment.hpp"
#include "game/settings.hpp"
#include "city/city.hpp"

using namespace constants;

namespace events
{

GameEventPtr ShowAdvisorWindow::create(bool show, constants::advisor::Type advisor)
{
  ShowAdvisorWindow* ev = new ShowAdvisorWindow();
  ev->_show = show;
  ev->_advisor = advisor;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool ShowAdvisorWindow::_mayExec(Game& game, unsigned int time) const {  return true; }

void ShowAdvisorWindow::_exec(Game& game, unsigned int)
{
  Variant advEnabled = GameSettings::get( GameSettings::adviserEnabled );
  if( advEnabled.isValid() && !(bool)advEnabled )
  {
    events::GameEventPtr e = events::WarningMessageEvent::create( "##not_available##" );
    e->dispatch();
    return;
  }

  List<gui::AdvisorsWindow*> wndList = game.gui()->rootWidget()->findChildren<gui::AdvisorsWindow*>();

  if( _show )
  {
    if( !wndList.empty() )
    {
      wndList.front()->bringToFront();
      wndList.front()->showAdvisor( _advisor );
    }
    else
    {
      gui::AdvisorsWindow::create( game.gui()->rootWidget(), -1, _advisor, game.getCity() );
    }
  }
  else
  {
    if( !wndList.empty() )
    {
      wndList.front()->deleteLater();
    }
  }
}

} //end namespace events
