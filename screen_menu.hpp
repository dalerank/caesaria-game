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

#include <picture.hpp>
#include <screen.hpp>
#include <gui_menu.hpp>


// displays the newGame/loadGame/quitGame menu
class ScreenMenu: public Screen
{
public:
   ScreenMenu();
   ~ScreenMenu();

   void init();

   virtual void handleEvent(SDL_Event &event);
   virtual void handleWidgetEvent(const WidgetEvent &event, Widget *widget);

   // draws the menu/menubar/etc... on the screen
   virtual void draw();

protected:
   void initMenu();
   void setBgPicture(Picture *picture);

private:

   Picture *_bgPicture;
   StartMenu _menu;         // menu to display
};


#endif
