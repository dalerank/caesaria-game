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

#include "winmission.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "gui/win_mission_window.hpp"
#include "city/win_targets.hpp"
#include "gui/environment.hpp"
#include "core/logger.hpp"

namespace events
{

GameEventPtr WinMission::create()
{
  WinMission* e = new WinMission();

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void WinMission::_exec(Game& game, uint)
{
  Logger::warning( "WinMission: exec ");
  PlayerCityPtr city = game.getCity();

  const CityWinTargets& wt = city->getWinTargets();
  std::string nextMission = wt.getNextMission();
  std::string newTitle = wt.getNewTitle();

  /*gui::WinMissionWindow* wnd = */new gui::WinMissionWindow( game.getGui()->getRootWidget(), newTitle, false );

  //CONNECT( wnd, onNextMission(), &game, Game::load);
}

bool WinMission::_mayExec(Game&, uint ) const { return true; }

}
