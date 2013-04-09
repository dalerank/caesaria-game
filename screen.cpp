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


#include "screen.hpp"

#include <iostream>

#include "gfx_engine.hpp"
#include "exception.hpp"


Screen::Screen()
{
  _eventProcessor.setAllowDrag(true);
}

Screen::~Screen() {}


void Screen::drawFrame()
{
  GfxEngine &engine = GfxEngine::instance();
  engine.init_frame();
  draw();
  engine.exit_frame();
}


void Screen::handleEvent(SDL_Event &event) {}

void Screen::handleWidgetEvent(const WidgetEvent &event, Widget *widget) {}

void Screen::afterFrame() {}

void Screen::stop()
{
   _isStopped = true;
}

WidgetEvent Screen::run()
{
   Uint32 lastclock = SDL_GetTicks();
   Uint32 currentclock = 0;
   Uint32 ref_delay = 1000 / 20;  // 20fps

   _isStopped = false;

   while (! _isStopped)
   {
      drawFrame();
      afterFrame();

      SDL_Event event;

      while ( SDL_PollEvent(&event) )
      {
         if (event.type == SDL_QUIT)
         {
            exit(0);
         }

         _eventProcessor.processEvent(event);
         handleEvent(event);
      }

      _eventProcessor.timeStep(event);
      handleEvent(event);
      
      // sets a fix frameRate
      currentclock = SDL_GetTicks();
      long delay = ref_delay - (currentclock - lastclock);
      if (delay < 0)
      {
         // std::cout << "frame takes too much time! " << currentclock - lastclock << std::endl;
         delay = 0;
      }
      // FIX: game spontanous hangs
      else if (delay > ref_delay)
      {
        delay = ref_delay;
      }
      lastclock = currentclock;
      SDL_Delay(delay);
   }

   return _wevent;
}

