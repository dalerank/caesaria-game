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


#ifndef CUSTOM_EVENT_HPP
#define CUSTOM_EVENT_HPP

#include <SDL_events.h>


typedef enum
{
   SDL_USER_NOEVENT = 0,
   SDL_USER_MOUSECLICK,
   SDL_USER_MOUSEDRAG,
   SDL_USER_MAX
} SDL_USER_EventType;


typedef struct SDL_USER_MouseClickEvent
{
   Uint8 button;  /** The mouse button index */
   Uint16 x, y;   /** The X/Y coordinates of the mouse at press time */
} SDL_USER_MouseClickEvent;


typedef struct SDL_USER_MouseDragEvent
{
   Uint8 button;  /** The mouse button index */
   int state;   /** 0: drag start (button hold), 1: drag (mouse move), 2: drag end (button release) */
   Uint16 x1, y1;   /** The X/Y coordinates of the mouse at press time */
   Uint16 x2, y2;   /** The X/Y coordinates of the mouse at release time */
} SDL_USER_MouseDragEvent;


/* Process mouse events to custom events for click/click&drag */
class CustomEvent
{
public:
   CustomEvent();
   ~CustomEvent();
   void setAllowDrag(const bool value);  // allow=false makes the gui more responsive

   void reset();
   void processEvent(SDL_Event &event);
   void timeStep(SDL_Event &event);

private:
   int _clickButton;  // button used for click
   bool _allowDrag;   // makes the gui more responsive when false
   bool _isHold;  // true if the button is hold since some time
   int _clickX;  // click position on click&drag
   int _clickY;  // click position on click&drag
   unsigned long _pressTime;  // time of the last click
   unsigned long _delay;  // delay for double click

   SDL_USER_MouseClickEvent _clickEvent;
   SDL_USER_MouseDragEvent _dragEvent;
};

// typedef struct SDL_UserEvent {
//    Uint8 type;    /** SDL_USEREVENT through SDL_NUMEVENTS-1 */
//    int code;      /** User defined event code */
//    void *data1;   /** User defined data pointer */
//    void *data2;   /** User defined data pointer */
// } SDL_UserEvent;
//
// typedef struct SDL_MouseMotionEvent {
//    Uint8 type;   /** SDL_MOUSEMOTION */
//    Uint8 which;  /** The mouse device index */
//    Uint8 state;  /** The current button state */
//    Uint16 x, y;  /** The X/Y coordinates of the mouse */
//    Sint16 xrel;  /** The relative motion in the X direction */
//    Sint16 yrel;  /** The relative motion in the Y direction */
// } SDL_MouseMotionEvent;
//
// typedef struct SDL_MouseButtonEvent {
//    Uint8 type;   /** SDL_MOUSEBUTTONDOWN or SDL_MOUSEBUTTONUP */
//    Uint8 which;  /** The mouse device index */
//    Uint8 button; /** The mouse button index */
//    Uint8 state;  /** SDL_PRESSED or SDL_RELEASED */
//    Uint16 x, y;  /** The X/Y coordinates of the mouse at press time */
// } SDL_MouseButtonEvent;


#endif
