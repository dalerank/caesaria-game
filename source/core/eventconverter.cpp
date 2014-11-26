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
#include "SDL_version.h"
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
    inline void init_key( int name, int value ) { KeyMap[ name ] = value; }     
};

void EventConverter::Impl::createKeyMap()
{
    // I don't know if this is the best method  to create
    // the lookuptable, but I'll leave it like that until
    // I find a better version.
    // buttons missing

    init_key( SDLK_BACKSPACE, KEY_BACK );
    init_key( SDLK_TAB, KEY_TAB );
    init_key( SDLK_CLEAR, KEY_CLEAR );
    init_key( SDLK_RETURN, KEY_RETURN );

    // combined modifiers missing
    init_key( SDLK_PAUSE, KEY_PAUSE );
    init_key( SDLK_CAPSLOCK, KEY_CAPITAL );

    init_key( SDLK_ESCAPE, KEY_ESCAPE );
    init_key( SDLK_SPACE, KEY_SPACE );
    init_key( SDLK_PAGEUP, KEY_PRIOR );
    init_key( SDLK_PAGEDOWN, KEY_NEXT );
    init_key( SDLK_END , KEY_END );
    init_key( SDLK_HOME , KEY_HOME );
    init_key( SDLK_LEFT , KEY_LEFT );
    init_key( SDLK_UP , KEY_UP );
    init_key( SDLK_RIGHT , KEY_RIGHT );
    init_key( SDLK_DOWN , KEY_DOWN );

    init_key( SDLK_PRINTSCREEN,KEY_PRINT );
    init_key( SDLK_PRINTSCREEN, KEY_SNAPSHOT );

    init_key( SDLK_INSERT, KEY_INSERT );
    init_key( SDLK_DELETE, KEY_DELETE );
    init_key( SDLK_HELP, KEY_HELP );

    init_key( SDLK_0 , KEY_KEY_0 );
    init_key( SDLK_1 , KEY_KEY_1 );
    init_key( SDLK_2 , KEY_KEY_2 );
    init_key( SDLK_3 , KEY_KEY_3 );
    init_key( SDLK_4 , KEY_KEY_4 );
    init_key( SDLK_5 , KEY_KEY_5 );
    init_key( SDLK_6 , KEY_KEY_6 );
    init_key( SDLK_7 , KEY_KEY_7 );
    init_key( SDLK_8 , KEY_KEY_8 );
    init_key( SDLK_9 , KEY_KEY_9 );

    init_key( SDLK_a , KEY_KEY_A );
    init_key( SDLK_b , KEY_KEY_B );
    init_key( SDLK_c , KEY_KEY_C );
    init_key( SDLK_d , KEY_KEY_D );
    init_key( SDLK_e , KEY_KEY_E );
    init_key( SDLK_f , KEY_KEY_F );
    init_key( SDLK_g , KEY_KEY_G );
    init_key( SDLK_h , KEY_KEY_H );
    init_key( SDLK_i , KEY_KEY_I );
    init_key( SDLK_j , KEY_KEY_J );
    init_key( SDLK_k , KEY_KEY_K );
    init_key( SDLK_l , KEY_KEY_L );
    init_key( SDLK_m , KEY_KEY_M );
    init_key( SDLK_n , KEY_KEY_N );
    init_key( SDLK_o , KEY_KEY_O );
    init_key( SDLK_p , KEY_KEY_P );
    init_key( SDLK_q , KEY_KEY_Q );
    init_key( SDLK_r , KEY_KEY_R );
    init_key( SDLK_s , KEY_KEY_S );
    init_key( SDLK_t , KEY_KEY_T );
    init_key( SDLK_u , KEY_KEY_U );
    init_key( SDLK_v , KEY_KEY_V );
    init_key( SDLK_w , KEY_KEY_W );
    init_key( SDLK_x , KEY_KEY_X );
    init_key( SDLK_y , KEY_KEY_Y );
    init_key( SDLK_z , KEY_KEY_Z ); 

    init_key( SDLK_LGUI, KEY_LWIN );
    init_key( SDLK_RGUI, KEY_RWIN );
    init_key( SDLK_POWER,  KEY_SLEEP );

    init_key( SDLK_KP_0, KEY_NUMPAD0 );
    init_key( SDLK_KP_1, KEY_NUMPAD1 );
    init_key( SDLK_KP_2, KEY_NUMPAD2 );
    init_key( SDLK_KP_3, KEY_NUMPAD3 );
    init_key( SDLK_KP_4, KEY_NUMPAD4 );
    init_key( SDLK_KP_5, KEY_NUMPAD5 );
    init_key( SDLK_KP_6, KEY_NUMPAD6 );
    init_key( SDLK_KP_7, KEY_NUMPAD7 );
    init_key( SDLK_KP_8, KEY_NUMPAD8 );
    init_key( SDLK_KP_9, KEY_NUMPAD9 );
    init_key( SDLK_KP_MULTIPLY, KEY_MULTIPLY );
    init_key( SDLK_KP_PLUS, KEY_ADD );
    init_key( SDLK_KP_MINUS, KEY_SUBTRACT );
    init_key( SDLK_KP_PERIOD, KEY_DECIMAL );
    init_key( SDLK_KP_DIVIDE, KEY_DIVIDE );

    init_key( SDLK_F1,  KEY_F1 );
    init_key( SDLK_F2,  KEY_F2 );
    init_key( SDLK_F3,  KEY_F3 );
    init_key( SDLK_F4,  KEY_F4 );
    init_key( SDLK_F5,  KEY_F5);
    init_key( SDLK_F6,  KEY_F6);
    init_key( SDLK_F7,  KEY_F7);
    init_key( SDLK_F8,  KEY_F8);
    init_key( SDLK_F9,  KEY_F9);
    init_key( SDLK_F10, KEY_F10);
    init_key( SDLK_F11, KEY_F11);
    init_key( SDLK_F12, KEY_F12);
    init_key( SDLK_F13, KEY_F13);
    init_key( SDLK_F14, KEY_F14);
    init_key( SDLK_F15, KEY_F15);
    // no higher F-keys

    init_key( SDLK_NUMLOCKCLEAR, KEY_NUMLOCK);
    init_key( SDLK_SCROLLLOCK, KEY_SCROLL);
    init_key( SDLK_LSHIFT,  KEY_LSHIFT);
    init_key( SDLK_RSHIFT,  KEY_RSHIFT);
    init_key( SDLK_LCTRL,   KEY_LCONTROL);
    init_key( SDLK_RCTRL,   KEY_RCONTROL);
    init_key( SDLK_LALT,    KEY_LALT);
    init_key( SDLK_RALT,    KEY_RALT);

    init_key( SDLK_PLUS,    KEY_PLUS);
    init_key( SDLK_COMMA,   KEY_COMMA);
    init_key( SDLK_MINUS,   KEY_MINUS);
    init_key( SDLK_PERIOD,  KEY_PERIOD);
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
    SDL_Keymod mods = SDL_GetModState();

    _d->mouseX = ret.mouse.x = sdlEvent.motion.x;
    _d->mouseY = ret.mouse.y = sdlEvent.motion.y;

    ret.mouse.control = mods & KMOD_CTRL;
    ret.mouse.shift = mods & KMOD_SHIFT;
    ret.mouse.buttonStates = _d->mouseButtonStates;
  }
  break;


#if SDL_MAJOR_VERSION>1
  case SDL_WINDOWEVENT:
  {
    ret.EventType = sAppEvent;
    ret.app.type = appEventCount;
    switch( sdlEvent.window.event )
    {
    //case SDL_WINDOWEVENT_LEAVE:
    case SDL_WINDOWEVENT_FOCUS_LOST:
      ret.app.type = appWindowFocusLeave;
    break;

    //case SDL_WINDOWEVENT_ENTER:
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      ret.app.type = appWindowFocusEnter ;
    break;

    default: break;
    }
  }
  break;
#endif

  /*case SDL_FINGERUP:
  case SDL_FINGERDOWN:
  {
     ret.EventType = sEventMouse;
     SDL_Keymod mods = SDL_GetModState();

     ret.mouse.x = sdlEvent.button.x;
     ret.mouse.y = sdlEvent.button.y;

     ret.mouse.control = mods & KMOD_CTRL;
     ret.mouse.shift = mods & KMOD_SHIFT;
     ret.mouse.type = mouseMoved;

     if(sdlEvent.type == SDL_FINGERDOWN)
     {
       ret.mouse.type = mouseLbtnPressed;
       _d->mouseButtonStates |= mbsmLeft;
     }
     else
     {
       ret.mouse.type = mouseLbtnRelease;
       _d->mouseButtonStates &= ~mbsmLeft;
     }
  }
  break;*/

  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP: 
  {    
    ret.EventType = sEventMouse;
    SDL_Keymod mods = SDL_GetModState();

    ret.mouse.x = sdlEvent.button.x;
    ret.mouse.y = sdlEvent.button.y;

    ret.mouse.control = mods & KMOD_CTRL;
    ret.mouse.shift = mods & KMOD_SHIFT;
    ret.mouse.type = mouseMoved;

    switch(sdlEvent.button.button)
    {
    case SDL_BUTTON_LEFT:
      if(sdlEvent.type == SDL_MOUSEBUTTONDOWN)
      {
        ret.mouse.type = mouseLbtnPressed;
        _d->mouseButtonStates |= mbsmLeft;
      }
      else
      {
        ret.mouse.type = mouseLbtnRelease;
        _d->mouseButtonStates &= ~mbsmLeft;
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
        _d->mouseButtonStates &= ~mbsmRight;
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
        ret.mouse.type = mouseMbtnRelease;
        _d->mouseButtonStates &= ~mbsmMiddle;
      }
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

  case SDL_MOUSEWHEEL:
  {
    SDL_MouseWheelEvent wheelEvent = sdlEvent.wheel;
    ret.EventType = sEventMouse;
    ret.mouse.type = mouseWheel;
    ret.mouse.x = _d->mouseX;
    ret.mouse.y = _d->mouseY;
    ret.mouse.wheel = wheelEvent.y > 0 ? 1.0f : -1.0f;
  }
  break;

  case SDL_TEXTINPUT:
  {
    ret.EventType = sTextInput;
    memcpy( ret.text.text, sdlEvent.text.text, 32 );
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
    ret.keyboard.symbol = 0;
  }
  break;


  case SDL_QUIT:
    ret.EventType = sEventQuit;
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
//  SDL_EnableUNICODE(1);
  _d->createKeyMap();
}

EventConverter::~EventConverter()
{
}


