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

#include "picture.hpp"
#include "scenario.hpp"
#include "gui_menu.hpp"
#include "gui_tilemap.hpp"
#include "gui_info_box.hpp"

#include "screen.hpp"


class ScreenGame: public Screen
{
public:
   ScreenGame();
   ~ScreenGame();

   void init();

   TilemapArea& getMapArea();
   void setScenario(Scenario &scenario);
   void setBuildMenu(BuildMenu *buildMenu);
   void setInGameMenu(InGameMenu *inGameMenu);
   void setInfoBox(GuiInfoBox *infoBox);

   virtual void afterFrame();
   virtual void handleEvent(SDL_Event &event);
   virtual void handleWidgetEvent(const WidgetEvent &event, Widget *widget);

   virtual void draw();

private:
   void setMenu(Menu &menu);

   void drawTilemap();
   void drawInterface();


   // stupid function, returns 0 if no shift pressed, 1 if shift pressed
   int isModShift();

private:
   GuiInfoBox* _infoBox;   // info box to display, if any
   MenuBar* _menuBar;   // menu bar to display
   Menu* _menu;         // menu to display
   BuildMenu* _buildMenu;   // build menu to display, if any
   InGameMenu* _inGameMenu;   // inGameMenu to display, if any
   Scenario* _scenario; // current game scenario
   TilemapArea _mapArea;  // visible map area

   GuiTilemap _guiTilemap;
};


#endif
