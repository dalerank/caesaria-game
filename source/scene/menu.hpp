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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_SCENE_STARTMENU_H_INCLUDE_
#define _CAESARIA_SCENE_STARTMENU_H_INCLUDE_

#include "base.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include <string>

class Game;

namespace scene
{

// displays the newGame/loadGame/quitGame menu
class StartMenu : public Base
{
public:
  typedef enum
  {
    startNewGame=0,
    loadMap,
    loadMission,
    loadSavedGame,
    closeApplication,
    reloadScreen,
    unlknowState=0xff
  } Result;

  StartMenu( Game& game, gfx::Engine& engine );
  virtual ~StartMenu();

  virtual void handleEvent( NEvent& event);

  virtual void draw();
  virtual void initialize();
  virtual void afterFrame();

  std::string mapName() const;
  std::string playerName() const;

  int result() const;
  bool isStopped() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace scene

#endif //_CAESARIA_SCENE_STARTMENU_H_INCLUDE_
