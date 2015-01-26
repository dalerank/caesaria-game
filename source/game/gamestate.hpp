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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com


#ifndef __CAESARIA_GAMESTATE_H_INCLUDED__
#define __CAESARIA_GAMESTATE_H_INCLUDED__

#include "game.hpp"
#include "scene/constants.hpp"

namespace scene
{
  class Briefing;
  class StartMenu;
  class Level;
}

namespace gamestate
{

class BaseState
{
public:
  BaseState(Game* game);

  virtual ~BaseState();

  virtual bool update(gfx::Engine* engine);

  scene::ScreenType getScreenType();

  scene::Base* toBase();

protected:
  Game* _game;
  scene::Base* _screen;
  scene::ScreenType _screenType;

  void _initialize(scene::Base* screen, scene::ScreenType screenType);
};

class MissionSelect : public BaseState
{
public:
  MissionSelect(Game *game, gfx::Engine* engine, const std::string &file);

  virtual ~MissionSelect();

private:
  scene::Briefing* _briefing;
};

class ShowMainMenu: public BaseState
{
public:
  ShowMainMenu(Game *game, gfx::Engine* engine);

  ~ShowMainMenu();
private:
  scene::StartMenu* startMenu;
};

class GameLoop: public BaseState
{
public:
  GameLoop(Game *game, gfx::Engine* engine,
                   unsigned int& saveTime,
                   unsigned int& timeX10,
                   unsigned int& timeMultiplier,
                   unsigned int& manualTicksCounterX10,
                   std::string& nextFilename,
                   std::string& restartFilename );

  virtual bool update(gfx::Engine* engine);

  ~GameLoop();
private:
  scene::Level* _level;
  unsigned int& _saveTime;
  unsigned int& _timeX10;
  unsigned int& _timeMultiplier;
  unsigned int& _manualTicksCounterX10;
  std::string& _nextFilename;
  std::string& _restartFilename;
};

} //end namespace gamestate

#endif //__CAESARIA_GAMESTATE_H_INCLUDED__
