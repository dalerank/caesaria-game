// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef SCREEN_MENU_HPP
#define SCREEN_MENU_HPP

#include "oc3_screen.hpp"
#include "oc3_scopedptr.hpp"
#include <string>

// displays the newGame/loadGame/quitGame menu
class ScreenMenu : public Screen
{
public:
  enum 
  { 
    startNewGame = 0,
    loadMap,
    loadSavedGame,
    closeApplication,
    unknownState = 0xff
  } Result;
    
  ScreenMenu();
  ~ScreenMenu();

  void handleEvent( NEvent& event);

  // draws the menu/menubar/etc... on the screen
  void draw();
  void initialize( GfxEngine& engine, GuiEnv& gui );

  const std::string& getMapName() const;

protected:
  int getResult() const;
  bool isStopped() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif