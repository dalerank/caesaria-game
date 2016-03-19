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
// Copyright 2012-2016 dalerank, dalerankn8@gmail.com


#ifndef __CAESARIA_GAMESTATE_H_INCLUDED__
#define __CAESARIA_GAMESTATE_H_INCLUDED__

#include "game.hpp"
#include "scene/constants.hpp"

namespace scene
{
  class Briefing;
  class SplashScreen;
  class Lobby;
  class Level;
}

class ResourceLoader;

namespace gamestate
{

struct InitializeStep
{
  std::string name;
  Delegate2<bool&, std::string&> function;
};

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

class InSplash : public State
{
public:
  InSplash(Game *game);

  virtual bool update(gfx::Engine *engine);
  virtual ~InSplash();

  void loadResources(bool& isOk, std::string& result);
  void mountArchives(ResourceLoader& loader);
  void initSplashScreen(bool& isOk, std::string& result);
  void initCelebrations(bool& isOk, std::string& result);
  void loadPicInfo(bool& isOk, std::string& result);
  void initPictures(bool& isOk, std::string& result);
  void initNameGenerator(bool& isOk, std::string& result);
  void loadObjectsMetadata(bool& isOk, std::string& result);
  void loadWalkersMetadata(bool& isOk, std::string& result);
  void loadReligionConfig(bool& isOk, std::string& result);
  void fadeSplash(bool& isOk, std::string& result);
  void loadHouseSpecs(bool& isOk, std::string& result);
  void initScripts(bool& isOk, std::string& result);
  void updateSplashText(std::string text);
private:
  scene::SplashScreen* _splash;
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
  scene::Lobby* _startMenu;
};

} //end namespace gamestate

#endif //__CAESARIA_GAMESTATE_H_INCLUDED__
