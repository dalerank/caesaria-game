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


#include "screen_menu.hpp"

#include <iostream>

#include "gfx_engine.hpp"
#include "exception.hpp"
#include "pic_loader.hpp"


ScreenMenu::ScreenMenu()
{
   _bgPicture = NULL;
}

ScreenMenu::~ScreenMenu() {}

void ScreenMenu::init()
{
   setBgPicture(&PicLoader::instance().get_picture("title", 1));

   initMenu();
}

void ScreenMenu::setBgPicture(Picture *picture)
{
   _bgPicture = picture;

   // center the bgPicture on the screen
   GfxEngine &engine = GfxEngine::instance();
   int x = (engine.getScreenWidth() - _bgPicture->get_width()) / 2;
   int y = (engine.getScreenHeight() - _bgPicture->get_height()) / 2;
   _bgPicture->set_offset(x, -y);
}

void ScreenMenu::initMenu()
{
   _menu.init();

   // center the menu on the screen
   GfxEngine &engine = GfxEngine::instance();
   int x = (engine.getScreenWidth() - _menu.getWidth()) / 2;
   int y = (engine.getScreenHeight() - _menu.getHeight()) / 2;
   _menu.setPosition(x, y);
   _menu.setListener(this);
}


void ScreenMenu::draw()
{
   GfxEngine &engine = GfxEngine::instance();

   engine.drawPicture(*_bgPicture, 0, 0);
   _menu.draw(0, 0);
}


void ScreenMenu::handleEvent(SDL_Event &event)
{
   switch (event.type)
   {
   case SDL_MOUSEMOTION:
      _menu.handleEvent(event);
      break;
//   case SDL_MOUSEBUTTONDOWN:
   case SDL_USEREVENT:
      _menu.handleEvent(event);
      break;
   case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
      {
         handleWidgetEvent(WidgetEvent::QuitGameEvent(), NULL);
      }
      break;
   }

}


void ScreenMenu::handleWidgetEvent(const WidgetEvent &event, Widget *widget)
{
   _wevent = event;

   switch (event._eventType)
   {
   case WE_NewGame:
      stop();
      break;
   case WE_LoadGame:
      stop();
      break;
   case WE_QuitGame:
      stop();
      break;
   default:
      THROW("Unexpected widget event: " << event._eventType);
   }
}

