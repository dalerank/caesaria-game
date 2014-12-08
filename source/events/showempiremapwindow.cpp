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

#include "showempiremapwindow.hpp"
#include "gui/empiremap_window.hpp"
#include "game/game.hpp"
#include "gui/environment.hpp"
#include "world/empire.hpp"
#include "city/city.hpp"

using namespace constants;

namespace events
{


GameEventPtr ShowEmpireMap::create(bool show)
{
  ShowEmpireMap* ev = new ShowEmpireMap();
  ev->_show = show;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool ShowEmpireMap::_mayExec(Game& game, unsigned int time) const
{
  return true;
}

ShowEmpireMap::ShowEmpireMap() : _show( false )
{

}

void ShowEmpireMap::_exec(Game& game, unsigned int)
{
  List<gui::EmpireMapWindow*> wndList = game.gui()->rootWidget()->findChildren<gui::EmpireMapWindow*>();

  if( _show )
  {
    if( !wndList.empty() )
    {
      wndList.front()->bringToFront();
    }
    else
    {
      gui::EmpireMapWindow::create( game.city(), game.gui()->rootWidget(), -1 );
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
