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
  class Lobby;
  class Level;
}

namespace gamestate
{

class State
{
public:
  State(Game* game);

  virtual ~State();

  virtual bool update(gfx::Engine* engine);

  scene::ScreenType getScreenType();

  scene::Base* toBase();

protected:
  Game* _game;
  scene::Base* _screen;
  scene::ScreenType _screenType;

  void _initialize(scene::Base* screen, scene::ScreenType screenType);
};

class InBriefing : public State
{
public:
  InBriefing(Game *game, gfx::Engine* engine, const std::string &file);

  virtual ~InBriefing();

private:
  scene::Briefing* _briefing;
};

class InMainMenu: public State
{
public:
  InMainMenu(Game *game, gfx::Engine* engine);

  ~InMainMenu();
private:
  scene::Lobby* startMenu;
};

} //end namespace gamestate

#endif //__CAESARIA_GAMESTATE_H_INCLUDED__
