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
#include "gui/predefinitions.hpp"
#include <string>

class Game;
class VariantList;
class VariantMap;
class Variant;
class StringArray;

namespace script
{

class Session
{
public:
  Session(Game* game) { _game = game; }
  void continuePlay(int years);
  void loadNextMission();
  void setMode(int mode);
  void setOption(const std::string& name,Variant v);
  void clearUi();
  void save(const std::string& path);
  void createIssue(const std::string& type, int value);
  bool getBuildflag(const std::string& type);
  void setBuildflag(const std::string& type, bool value);
  void loadLocalization(const std::string& name);
  void openUrl(const std::string& url);
  int lastChangesNum() const;
  StringArray getCredits() const;
  StringArray getFiles(const std::string& dir, const std::string& ext);
  StringArray getFolders(const std::string& dir, bool full);
  int videoModesCount() const;
  void playAudio(const std::string& filename, int volume, const std::string& mode);
  Size getVideoMode(int index) const;
  void setResolution(const Size& size);
  void showDlcViewer(const std::string& path);
  StringArray tradableGoods() const;
  VariantMap winConditions() const;
  VariantMap getGoodInfo(std::string goodName) const;
  Size getResolution() const;
  void setFont(const std::string& fontname);
  void setLanguage(const std::string& lang,const std::string& audio);

private:
  Game* _game;
};

} //end namespace script

#endif  //_CAESARIA_SCRIPT_SESSION_INCLUDE_H_
