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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_SCENE_BRIEFING_H_INCLUDE_
#define _CAESARIA_SCENE_BRIEFING_H_INCLUDE_

#include "base.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include <string>

class Game;

namespace scene
{

// displays the newGame/loadGame/quitGame menu
class Briefing : public Base
{
public:
  typedef enum
  {
    loadMission,
    mainMenu,
    unlknowState=0xff
  } Result;

  Briefing( Game& game, gfx::Engine& engine, std::string filename );
  virtual ~Briefing();

  virtual void handleEvent( NEvent& event);

  // draws the menu/menubar/etc... on the screen
  virtual void draw();
  virtual void initialize();

  std::string getMapName() const;

  virtual int result() const;
  virtual bool isStopped() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace scene

#endif //_CAESARIA_SCENE_BRIEFING_H_INCLUDE_
