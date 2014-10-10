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

#include "eventconverter.hpp"
#include "time.hpp"
#include "logger.hpp"
#include <map>

struct SMouseMultiClicks
{
    SMouseMultiClicks()
        : DoubleClickTime(200), CountSuccessiveClicks(0), LastClickTime(0), LastMouseInputEvent(mouseEventCount)
    {}

    int DoubleClickTime;
    int CountSuccessiveClicks;
    int LastClickTime;
    Point LastClick;
    MouseEventType LastMouseInputEvent;
};

class EventConverter::Impl
{
public:
    int mouseX, mouseY;
    unsigned int mouseButtonStates;

    SMouseMultiClicks MouseMultiClicks;

    std::map< int, int > KeyMap;

    int checkSuccessiveClicks(int mouseX, int mouseY, MouseEventType inputEvent );
    void createKeyMap();
};


void EventConverter::Impl::createKeyMap()
{
    // I don't know if this is the best method  to create
    // the lookuptable, but I'll leave it like that until
    // I find a better version.
    // buttons missing

    KeyMap[ SDLK_BACKSPACE ] = KEY_BACK;
    KeyMap[ SDLK_TAB ] = KEY_TAB;
    KeyMap[ SDLK_CLEAR ] = KEY_CLEAR;
    KeyMap[ SDLK_RETURN ] = KEY_RETURN;

    // combined modifiers missing

    KeyMap[ SDLK_PAUSE ] = KEY_PAUSE;
    KeyMap[ SDLK_CAPSLOCK ] = KEY_CAPITAL;

    // asian letter keys missing

    KeyMap[ SDLK_ESCAPE ] = KEY_ESCAPE ;

    // asian letter keys missing

    KeyMap[ SDLK_SPACE ] = KEY_SPACE;
    KeyMap[ SDLK_PAGEUP ] = KEY_PRIOR;
    KeyMap[ SDLK_PAGEDOWN ] = KEY_NEXT;
    KeyMap[ SDLK_END ]= KEY_END;
    KeyMap[ SDLK_HOME ]= KEY_HOME;
    KeyMap[ SDLK_LEFT ]= KEY_LEFT;
    KeyMap[ SDLK_UP ]= KEY_UP;
    KeyMap[ SDLK_RIGHT ]= KEY_RIGHT;
    KeyMap[ SDLK_DOWN ]= KEY_DOWN;

    // select missing
    KeyMap[ SDLK_PRINT]=KEY_PRINT;
    // execute missing
    KeyMap[ SDLK_PRINT]= KEY_SNAPSHOT;

    KeyMap[ SDLK_INSERT]= KEY_INSERT;
    KeyMap[ SDLK_DELETE]= KEY_DELETE;
    KeyMap[ SDLK_HELP]= KEY_HELP;

    KeyMap[ SDLK_0 ]= KEY_KEY_0;
    KeyMap[ SDLK_1 ]= KEY_KEY_1;
    KeyMap[ SDLK_2 ]= KEY_KEY_2;
    KeyMap[ SDLK_3 ]= KEY_KEY_3;
    KeyMap[ SDLK_4 ]= KEY_KEY_4;
    KeyMap[ SDLK_5 ]= KEY_KEY_5;
    KeyMap[ SDLK_6 ]= KEY_KEY_6;
    KeyMap[ SDLK_7 ]= KEY_KEY_7;
    KeyMap[ SDLK_8 ]= KEY_KEY_8;
    KeyMap[ SDLK_9 ]= KEY_KEY_9;

    KeyMap[ SDLK_a ]= KEY_KEY_A;
    KeyMap[ SDLK_b ]= KEY_KEY_B;
    KeyMap[ SDLK_c ]= KEY_KEY_C;
    KeyMap[ SDLK_d ]= KEY_KEY_D;
    KeyMap[ SDLK_e ]= KEY_KEY_E;
    KeyMap[ SDLK_f ]= KEY_KEY_F;
    KeyMap[ SDLK_g ]= KEY_KEY_G;
    KeyMap[ SDLK_h ]= KEY_KEY_H;
    KeyMap[ SDLK_i ]= KEY_KEY_I;
    KeyMap[ SDLK_j ]= KEY_KEY_J;
    KeyMap[ SDLK_k ]= KEY_KEY_K;
    KeyMap[ SDLK_l ]= KEY_KEY_L;
    KeyMap[ SDLK_m ]= KEY_KEY_M;
    KeyMap[ SDLK_n ]= KEY_KEY_N;
    KeyMap[ SDLK_o ]= KEY_KEY_O;
    KeyMap[ SDLK_p ]= KEY_KEY_P;
    KeyMap[ SDLK_q ]= KEY_KEY_Q;
    KeyMap[ SDLK_r ]= KEY_KEY_R;
    KeyMap[ SDLK_s ]= KEY_KEY_S;
    KeyMap[ SDLK_t ]= KEY_KEY_T;
    KeyMap[ SDLK_u ]= KEY_KEY_U;
    KeyMap[ SDLK_v ]= KEY_KEY_V;
    KeyMap[ SDLK_w ]= KEY_KEY_W;
    KeyMap[ SDLK_x ]= KEY_KEY_X;
    KeyMap[ SDLK_y ]= KEY_KEY_Y;
    KeyMap[ SDLK_z ]= KEY_KEY_Z;

    KeyMap[ SDLK_LSUPER]= KEY_LWIN;
    KeyMap[ SDLK_RSUPER]= KEY_RWIN;
    // apps missing
    KeyMap[ SDLK_POWER]=  KEY_SLEEP; //??

    KeyMap[ SDLK_KP0]= KEY_NUMPAD0;
    KeyMap[ SDLK_KP1]= KEY_NUMPAD1;
    KeyMap[ SDLK_KP2]= KEY_NUMPAD2;
    KeyMap[ SDLK_KP3]= KEY_NUMPAD3;
    KeyMap[ SDLK_KP4]= KEY_NUMPAD4;
    KeyMap[ SDLK_KP5]= KEY_NUMPAD5;
    KeyMap[ SDLK_KP6]= KEY_NUMPAD6;
    KeyMap[ SDLK_KP7]= KEY_NUMPAD7;
    KeyMap[ SDLK_KP8]= KEY_NUMPAD8;
    KeyMap[ SDLK_KP9]= KEY_NUMPAD9;
    KeyMap[ SDLK_KP_MULTIPLY]= KEY_MULTIPLY;
    KeyMap[ SDLK_KP_PLUS]= KEY_ADD;
    //	KeyMap[ SDLK_KP_, KEY_SEPARATOR));
    KeyMap[ SDLK_KP_MINUS]= KEY_SUBTRACT;
    KeyMap[ SDLK_KP_PERIOD]= KEY_DECIMAL;
    KeyMap[ SDLK_KP_DIVIDE]= KEY_DIVIDE;

    KeyMap[ SDLK_F1]=  KEY_F1;
    KeyMap[ SDLK_F2]=  KEY_F2;
    KeyMap[ SDLK_F3]=  KEY_F3;
    KeyMap[ SDLK_F4]=  KEY_F4;
    KeyMap[ SDLK_F5]=  KEY_F5;
    KeyMap[ SDLK_F6]=  KEY_F6;
    KeyMap[ SDLK_F7]=  KEY_F7;
    KeyMap[ SDLK_F8]=  KEY_F8;
    KeyMap[ SDLK_F9]=  KEY_F9;
    KeyMap[ SDLK_F10]= KEY_F10;
    KeyMap[ SDLK_F11]= KEY_F11;
    KeyMap[ SDLK_F12]= KEY_F12;
    KeyMap[ SDLK_F13]= KEY_F13;
    KeyMap[ SDLK_F14]= KEY_F14;
    KeyMap[ SDLK_F15]= KEY_F15;
    // no higher F-keys

    KeyMap[ SDLK_NUMLOCK]= KEY_NUMLOCK;
    KeyMap[ SDLK_SCROLLOCK]= KEY_SCROLL;
    KeyMap[ SDLK_LSHIFT]=  KEY_LSHIFT;
    KeyMap[ SDLK_RSHIFT]=  KEY_RSHIFT;
    KeyMap[ SDLK_LCTRL]=   KEY_LCONTROL;
    KeyMap[ SDLK_RCTRL]=   KEY_RCONTROL;
    KeyMap[ SDLK_LALT]=    KEY_LWIN;
    KeyMap[ SDLK_RALT]=    KEY_RWIN;

    KeyMap[ SDLK_PLUS]=    KEY_PLUS;
    KeyMap[ SDLK_COMMA]=   KEY_COMMA;
    KeyMap[ SDLK_MINUS]=   KEY_MINUS;
    KeyMap[ SDLK_PERIOD]=  KEY_PERIOD;
}

NEvent EventConverter::get( const SDL_Event& sdlEvent )
{
  NEvent ret;

  switch( sdlEvent.type )
  {
  case SDL_MOUSEMOTION:
  {
    ret.EventType = sEventMouse;
    ret.mouse.type = mouseMoved;
    Uint8 *keys = SDL_GetKeyState(NULL);

    _d->mouseX = ret.mouse.x = sdlEvent.motion.x;
    _d->mouseY = ret.mouse.y = sdlEvent.motion.y;

    ret.mouse.control = keys[SDLK_LCTRL];
    ret.mouse.shift = keys[SDLK_LSHIFT];
    ret.mouse.buttonStates = _d->mouseButtonStates;
  }
  break;

  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
  {    
    ret.EventType = sEventMouse;
    Uint8 *keys = SDL_GetKeyState(NULL);

    ret.mouse.x = sdlEvent.button.x;
    ret.mouse.y = sdlEvent.button.y;

    ret.mouse.control = keys[SDLK_LCTRL];
    ret.mouse.shift = keys[SDLK_LSHIFT];
    ret.mouse.type = mouseMoved;

    switch(sdlEvent.button.button)
    {
    case SDL_BUTTON_LEFT:
      if (sdlEvent.type == SDL_MOUSEBUTTONDOWN)
      {
        ret.mouse.type = mouseLbtnPressed;
        _d->mouseButtonStates |= mbsmLeft;
      }
      else
      {
        ret.mouse.type = mouseLbtnRelease;
        _d->mouseButtonStates &= !mbsmLeft;
      }
    break;

    case SDL_BUTTON_RIGHT:
      if (sdlEvent.type == SDL_MOUSEBUTTONDOWN)
      {
        ret.mouse.type = mouseRbtnPressed;
        _d->mouseButtonStates |= mbsmRight;
      }
      else
      {
        ret.mouse.type = mouseRbtnRelease;
        _d->mouseButtonStates &= !mbsmRight;
      }
    break;

    case SDL_BUTTON_MIDDLE:
      if (sdlEvent.type == SDL_MOUSEBUTTONDOWN)
      {
        ret.mouse.type = mouseMbtnPressed;
        _d->mouseButtonStates |= mbsmMiddle;
      }
      else
      {
        ret.mouse.control = keys[SDLK_LCTRL];
        ret.mouse.shift = keys[SDLK_LSHIFT];
        ret.mouse.type = mouseMbtnRelease;
        _d->mouseButtonStates &= !mbsmMiddle;
      }
    break;

    case SDL_BUTTON_WHEELUP:
      ret.mouse.type = mouseWheel;
      ret.mouse.wheel = 1.0f;
    break;

    case SDL_BUTTON_WHEELDOWN:
      ret.mouse.type = mouseWheel;
      ret.mouse.wheel = -1.0f;
    break;
    }

    ret.mouse.buttonStates = _d->mouseButtonStates;

    if( ret.mouse.type != mouseMoved )
    {
      if( ret.mouse.type >= mouseLbtnPressed && ret.mouse.type <= mouseMbtnPressed )
      {
        int clicks = _d->checkSuccessiveClicks(ret.mouse.x, ret.mouse.y, ret.mouse.type);
        if ( clicks == 2 )
        {
          ret.mouse.type = (MouseEventType)(mouseLbtDblClick + ret.mouse.type-mouseLbtnPressed);
        }
        else if ( clicks == 3 )
        {
          ret.mouse.type = (MouseEventType)(mouseLbtnTrplClick + ret.mouse.type-mouseLbtnPressed);
        }
      }
    }
  }
  break;

  case SDL_KEYDOWN:
  case SDL_KEYUP:
  {
    std::map< int, int >::iterator itSym = _d->KeyMap.find(sdlEvent.key.keysym.sym);

    KeyCode key = (KeyCode)0;
    if( itSym != _d->KeyMap.end() )
        key = (KeyCode)itSym->second;

    ret.EventType = sEventKeyboard;
    ret.keyboard.key = key;
    ret.keyboard.pressed = (sdlEvent.type == SDL_KEYDOWN);
    ret.keyboard.shift = (sdlEvent.key.keysym.mod & KMOD_SHIFT) != 0;
    ret.keyboard.control = (sdlEvent.key.keysym.mod & KMOD_CTRL ) != 0;
    ret.keyboard.symbol =  sdlEvent.key.keysym.unicode;
  }
  break;

  case SDL_QUIT:
      //Close = true;
  break;

//     case SDL_ACTIVEEVENT:
//         if ((sdlEvent.active.state == SDL_APPMOUSEFOCUS) ||
//             (sdlEvent.active.state == SDL_APPINPUTFOCUS))
//             WindowHasFocus = (SDL_event.active.gain==1);
//         else
//             if (SDL_event.active.state == SDL_APPACTIVE)
//                 WindowMinimized = (SDL_event.active.gain!=1);
//         break;

//     case SDL_VIDEORESIZE:
//         if ((sdlEvent.resize.w != (int)Width) || (sdlEvent.resize.h != (int)Height))
//         {
//             Width = SDL_event.resize.w;
//             Height = SDL_event.resize.h;
//             Screen = SDL_SetVideoMode( Width, Height, 0, SDL_Flags );             
//         }
//         break;

//     case SDL_USEREVENT:
//         ret.EventType = irr::EET_USER_EVENT;
//         ret.UserEvent.UserData1 = *(reinterpret_cast<s32*>(&SDL_event.user.data1));
//         ret.UserEvent.UserData2 = *(reinterpret_cast<s32*>(&SDL_event.user.data2));
// 
//         postEventFromUser(ret);
//         break;

  default:
      break;
  } // end switch

  return ret;
}

int EventConverter::Impl::checkSuccessiveClicks(int mouseX, int mouseY, MouseEventType inputEvent )
{
    const int MAX_MOUSEMOVE = 3;

    int clickTime = DateTime::elapsedTime();

    if ( (clickTime-MouseMultiClicks.LastClickTime) < MouseMultiClicks.DoubleClickTime
        && abs(MouseMultiClicks.LastClick.x() - mouseX ) <= MAX_MOUSEMOVE
        && abs(MouseMultiClicks.LastClick.y() - mouseY ) <= MAX_MOUSEMOVE
        && MouseMultiClicks.CountSuccessiveClicks < 3
        && MouseMultiClicks.LastMouseInputEvent == inputEvent
        )
    {
        ++MouseMultiClicks.CountSuccessiveClicks;
    }
    else
    {
        MouseMultiClicks.CountSuccessiveClicks = 1;
    }

    MouseMultiClicks.LastMouseInputEvent = inputEvent;
    MouseMultiClicks.LastClickTime = clickTime;
    MouseMultiClicks.LastClick = Point( mouseX, mouseY ); 

    return MouseMultiClicks.CountSuccessiveClicks;
}

EventConverter& EventConverter::instance()
{
    static EventConverter _helper;

    return _helper;
}

EventConverter::EventConverter() : _d( new Impl )
{
  SDL_EnableUNICODE(1);
  _d->createKeyMap();
}

EventConverter::~EventConverter()
{
}


