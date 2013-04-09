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


#ifndef SCREEN_HPP
#define SCREEN_HPP

#include "gui_widget.hpp"
#include "custom_event.hpp"

class Screen: public WidgetListener
{
public:
   virtual ~Screen();

   virtual void handleEvent(SDL_Event &event);
   virtual void handleWidgetEvent(const WidgetEvent &event, Widget *widget);

   virtual void draw() = 0;

   // this method is executed after every frame. default: do nothing
   virtual void afterFrame();

   // runs the screen (main loop), returns _wevent
   WidgetEvent run();
   void stop();

   // draws the complete frame
   void drawFrame();

protected:
   Screen();

   WidgetEvent _wevent;  // event to pass to the main loop
   bool _isStopped;  // screen needs to stop its loop
   CustomEvent _eventProcessor;  // process click/click&drag...
};


#endif
