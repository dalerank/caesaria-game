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

#include "winmission.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "gui/win_mission_window.hpp"
#include "city/victoryconditions.hpp"
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

void WinMission::_exec(Game& game, unsigned int)
{
  Logger::warning( "WinMission: exec ");
  PlayerCityPtr city = game.city();

  const city::VictoryConditions& wt = city->victoryConditions();
  std::string nextMission = wt.getNextMission();
  std::string newTitle = wt.getNewTitle();

  /*gui::WinMissionWindow* wnd = */new gui::WinMissionWindow( game.gui()->rootWidget(), newTitle, false );

  //CONNECT( wnd, onNextMission(), &game, Game::load);
}

bool WinMission::_mayExec(Game&, unsigned int ) const { return true; }

}
