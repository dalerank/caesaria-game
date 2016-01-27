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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_STEAM_HANDLER_INCLUDE_H_
#define _CAESARIA_STEAM_HANDLER_INCLUDE_H_

#include <string>
#include "gfx/picture.hpp"
#include "core/signals.hpp"

namespace steamapi
{

enum AchievementType
{
  achievementNewVillage = 0,
  achievementNewGraphics = 1,
  achievementFirstWin = 2,
  achievementClerkEscape = 3,
  achv_count
};

bool available();

bool checkSteamRunning();
bool connect();
void close();
void update();
void init();

void evaluateAchievement( AchievementType achivId );
void missionWin(const std::string& name);
void missionLose(const std::string& name);
bool isAchievementReached( AchievementType achivId );
bool isStatsReceived();
gfx::Picture achievementImage( AchievementType achivId );
std::string achievementCaption( AchievementType achivId );
std::string language();
std::string ld_prefix();
std::string userName();
const gfx::Picture& userImage();

}

#endif  //_CAESARIA_STEAM_HANDLER_INCLUDE_H_
