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


#ifndef SCREEN_GAME_HPP
#define SCREEN_GAME_HPP

#include <list>
#include <vector>

#include "oc3_picture.hpp"
#include "oc3_scenario.hpp"
#include "oc3_gui_tilemap.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_screen.hpp"
#include "oc3_scopedptr.hpp"

class Menu;
class BuildMenu;

class ScreenGame: public Screen
{
public:
  ScreenGame();
  ~ScreenGame();

  void initialize( GfxEngine& engine, GuiEnv& gui );

  TilemapArea& getMapArea();
  void setScenario(Scenario &scenario);

  virtual void afterFrame();
  virtual void handleEvent( NEvent& event);

  virtual void draw();

protected:
  int getResult() const;

private:
   //void setMenu(Menu &menu);

   void drawTilemap();
   void drawInterface();


   // stupid function, returns 0 if no shift pressed, 1 if shift pressed
   int isModShift();
   void resolveCreateConstruction( int type );
   void resolveRemoveTool();
   void showTileInfo( Tile* tile );

   GuiInfoBox* _infoBox;   // info box to display, if any
   Scenario* _scenario; // current game scenario
   TilemapArea _mapArea;  // visible map area

   GuiTilemap _guiTilemap;

   class Impl;
   ScopedPtr< Impl > _d;
};


#endif
