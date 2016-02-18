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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_SCRIPT_SESSION_INCLUDE_H_
#define _CAESARIA_SCRIPT_SESSION_INCLUDE_H_

#include "core/namedtype.hpp"
#include "core/size.hpp"
#include <string>

class Game;
class VariantList;
class VariantMap;
class StringArray;

namespace script
{

class Session
{
public:
  Session(Game* game) { _game = game; }
  void continuePlay(int years);
  void loadNextMission();
  void quitGame();
  void startCareer();
  void openUrl(const std::string& url);
  int lastChangesNum();
  StringArray getCredits();
  int videoModesCount();
  void playAudio(const std::string& filename, int volume, const std::string& mode);
  Size getVideoMode(int index);
  void setResolution(Size size);
  StringArray tradableGoods();
  VariantMap getGoodInfo(const std::string& goodName);
  Size getResolution();
  void saveSettings();

private:
  Game* _game;
};

} //end namespace script

#endif  //_CAESARIA_SCRIPT_SESSION_INCLUDE_H_
