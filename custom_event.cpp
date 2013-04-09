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



#include <custom_event.hpp>

#include <SDL_timer.h>
#include <exception.hpp>
#include <iostream>


CustomEvent::CustomEvent()
{
   reset();
   _delay = 200;
   _allowDrag = false;
}


CustomEvent::~CustomEvent() {}


void CustomEvent::reset()
{
   _clickButton = 0;
   _clickX = 0;
   _clickY = 0;
   _pressTime = 0;
   _isHold = false;
}

void CustomEvent::setAllowDrag(const bool value)
{
   _allowDrag = value;
   reset();
}

void CustomEvent::processEvent(SDL_Event &event)
{
   if (!_allowDrag && event.type == SDL_MOUSEBUTTONDOWN)
   {
      // this is a click
      _clickEvent.button = event.button.button;
      _clickEvent.x = event.button.x;
      _clickEvent.y = event.button.y;

      event.type = SDL_USEREVENT;
      event.user.type = SDL_USEREVENT;
      event.user.code = SDL_USER_MOUSECLICK;
      event.user.data1 = &_clickEvent;
      event.user.data2 = NULL;

      reset();
      // std::cout << "CLICK (" << _clickEvent.x << "," << _clickEvent.y << ")" << std::endl;
   }
   else if (_allowDrag && event.type == SDL_MOUSEMOTION)
   {
      int x = event.motion.x;
      int y = event.motion.y;

      if (_isHold)
      {
         // this is a click&drag
         _dragEvent.button = _clickButton;
         _dragEvent.state = 1;
         _dragEvent.x1 = _clickX;
         _dragEvent.y1 = _clickY;
         _dragEvent.x2 = x;
         _dragEvent.y2 = y;

         event.type = SDL_USEREVENT;
         event.user.type = SDL_USEREVENT;
         event.user.code = SDL_USER_MOUSEDRAG;
         event.user.data1 = &_dragEvent;
         event.user.data2 = NULL;
         // std::cout << "DRAG (" << _dragEvent.x1 << "," << _dragEvent.y1 << "), (" << _dragEvent.x2 << "," << _dragEvent.y2 << ")" << std::endl;
      }
   }
   else if (_allowDrag && event.type == SDL_MOUSEBUTTONDOWN)
   {
      _pressTime = SDL_GetTicks();

      _clickButton = event.button.button;
      _clickX = event.button.x;
      _clickY = event.button.y;

      event.type = SDL_NOEVENT; // capture the event
   }
   else if (_allowDrag && event.type == SDL_MOUSEBUTTONUP)
   {
      int x = event.button.x;
      int y = event.button.y;

      if (_isHold)
      {
         // this was a click&drag
         _dragEvent.button = _clickButton;
         _dragEvent.state = 2;
         _dragEvent.x1 = _clickX;
         _dragEvent.y1 = _clickY;
         _dragEvent.x2 = x;
         _dragEvent.y2 = y;

         event.type = SDL_USEREVENT;
         event.user.type = SDL_USEREVENT;
         event.user.code = SDL_USER_MOUSEDRAG;
         event.user.data1 = &_dragEvent;
         event.user.data2 = NULL;

         reset();
         // std::cout << "END DRAG (" << _dragEvent.x1 << "," << _dragEvent.y1 << "), (" << _dragEvent.x2 << "," << _dragEvent.y2 << ")" << std::endl;
      }
      else
      {
         // this was a simple click
         _clickEvent.button = _clickButton;
         _clickEvent.x = _clickX;
         _clickEvent.y = _clickY;

         event.type = SDL_USEREVENT;
         event.user.type = SDL_USEREVENT;
         event.user.code = SDL_USER_MOUSECLICK;
         event.user.data1 = &_clickEvent;
         event.user.data2 = NULL;

         reset();
         // std::cout << "CLICK (" << _clickEvent.x << "," << _clickEvent.y << ")" << std::endl;
      }
   }
}


void CustomEvent::timeStep(SDL_Event &event)
{
   event.type = SDL_NOEVENT;

   unsigned long time = SDL_GetTicks();

   if (_allowDrag && !_isHold && _pressTime != 0 && time > _pressTime+_delay)
   {
      // long press: this is the start of a click&drag
      _isHold = true;

      _dragEvent.button = _clickButton;
      _dragEvent.state = 0;
      _dragEvent.x1 = _clickX;
      _dragEvent.y1 = _clickY;
      _dragEvent.x2 = _clickX;
      _dragEvent.y2 = _clickY;

      event.type = SDL_USEREVENT;
      event.user.type = SDL_USEREVENT;
      event.user.code = SDL_USER_MOUSEDRAG;
      event.user.data1 = &_dragEvent;
      event.user.data2 = NULL;

      // std::cout << "BEGIN DRAG (" << _dragEvent.x1 << "," << _dragEvent.y1 << "), (" << _dragEvent.x2 << "," << _dragEvent.y2 << ")" << std::endl;
   }
}

