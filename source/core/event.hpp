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

#ifndef __CAESARIA_EVENT_H_INCLUDED__
#define __CAESARIA_EVENT_H_INCLUDED__

#include "core/position.hpp"

namespace gui
{
  class Widget;
}

enum KeyCode
{
    KEY_LBUTTON          = 0x01,  // Left mouse button
    KEY_MBUTTON          = 0x02,  // Right mouse button
    KEY_CANCEL           = 0x03,  // Control-break processing
    KEY_RBUTTON          = 0x04,  // Middle mouse button (three-button mouse)
    KEY_XBUTTON1         = 0x05,  // Windows 2000/XP: X1 mouse button
    KEY_XBUTTON2         = 0x06,  // Windows 2000/XP: X2 mouse button
    KEY_BACK             = 0x08,  // BACKSPACE key
    KEY_TAB              = 0x09,  // TAB key
    KEY_CLEAR            = 0x0C,  // CLEAR key
    KEY_RETURN           = 0x0D,  // ENTER key
    KEY_SHIFT            = 0x10,  // SHIFT key
    KEY_CONTROL          = 0x11,  // CTRL key
    KEY_MENU             = 0x12,  // ALT key
    KEY_PAUSE            = 0x13,  // PAUSE key
    KEY_CAPITAL          = 0x14,  // CAPS LOCK key
    KEY_KANA             = 0x15,  // IME Kana mode
    KEY_HANGUEL          = 0x15,  // IME Hanguel mode (maintained for compatibility use KEY_HANGUL)
    KEY_HANGUL           = 0x15,  // IME Hangul mode
    KEY_JUNJA            = 0x17,  // IME Junja mode
    KEY_FINAL            = 0x18,  // IME final mode
    KEY_HANJA            = 0x19,  // IME Hanja mode
    KEY_KANJI            = 0x19,  // IME Kanji mode
    KEY_ESCAPE           = 0x1B,  // ESC key
    KEY_CONVERT          = 0x1C,  // IME convert
    KEY_NONCONVERT       = 0x1D,  // IME nonconvert
    KEY_ACCEPT           = 0x1E,  // IME accept
    KEY_MODECHANGE       = 0x1F,  // IME mode change request
    KEY_SPACE            = 0x20,  // SPACEBAR
    KEY_PRIOR            = 0x21,  // PAGE UP key
    KEY_NEXT             = 0x22,  // PAGE DOWN key
    KEY_END              = 0x23,  // END key
    KEY_HOME             = 0x24,  // HOME key
    KEY_LEFT             = 0x25,  // LEFT ARROW key
    KEY_UP               = 0x26,  // UP ARROW key
    KEY_RIGHT            = 0x27,  // RIGHT ARROW key
    KEY_DOWN             = 0x28,  // DOWN ARROW key
    KEY_SELECT           = 0x29,  // SELECT key
    KEY_PRINT            = 0x2A,  // PRINT key
    KEY_EXECUT           = 0x2B,  // EXECUTE key
    KEY_SNAPSHOT         = 0x2C,  // PRINT SCREEN key
    KEY_INSERT           = 0x2D,  // INS key
    KEY_DELETE           = 0x2E,  // DEL key
    KEY_HELP             = 0x2F,  // HELP key
    KEY_KEY_0            = 0x30,  // 0 key
    KEY_KEY_1            = 0x31,  // 1 key
    KEY_KEY_2            = 0x32,  // 2 key
    KEY_KEY_3            = 0x33,  // 3 key
    KEY_KEY_4            = 0x34,  // 4 key
    KEY_KEY_5            = 0x35,  // 5 key
    KEY_KEY_6            = 0x36,  // 6 key
    KEY_KEY_7            = 0x37,  // 7 key
    KEY_KEY_8            = 0x38,  // 8 key
    KEY_KEY_9            = 0x39,  // 9 key
    KEY_KEY_A            = 0x41,  // A key
    KEY_KEY_B            = 0x42,  // B key
    KEY_KEY_C            = 0x43,  // C key
    KEY_KEY_D            = 0x44,  // D key
    KEY_KEY_E            = 0x45,  // E key
    KEY_KEY_F            = 0x46,  // F key
    KEY_KEY_G            = 0x47,  // G key
    KEY_KEY_H            = 0x48,  // H key
    KEY_KEY_I            = 0x49,  // I key
    KEY_KEY_J            = 0x4A,  // J key
    KEY_KEY_K            = 0x4B,  // K key
    KEY_KEY_L            = 0x4C,  // L key
    KEY_KEY_M            = 0x4D,  // M key
    KEY_KEY_N            = 0x4E,  // N key
    KEY_KEY_O            = 0x4F,  // O key
    KEY_KEY_P            = 0x50,  // P key
    KEY_KEY_Q            = 0x51,  // Q key
    KEY_KEY_R            = 0x52,  // R key
    KEY_KEY_S            = 0x53,  // S key
    KEY_KEY_T            = 0x54,  // T key
    KEY_KEY_U            = 0x55,  // U key
    KEY_KEY_V            = 0x56,  // V key
    KEY_KEY_W            = 0x57,  // W key
    KEY_KEY_X            = 0x58,  // X key
    KEY_KEY_Y            = 0x59,  // Y key
    KEY_KEY_Z            = 0x5A,  // Z key
    KEY_LWIN             = 0x5B,  // Left Windows key (Microsoft® Natural® keyboard)
    KEY_RWIN             = 0x5C,  // Right Windows key (Natural keyboard)
    KEY_APPS             = 0x5D,  // Applications key (Natural keyboard)
    KEY_SLEEP            = 0x5F,  // Computer Sleep key
    KEY_NUMPAD0          = 0x60,  // Numeric keypad 0 key
    KEY_NUMPAD1          = 0x61,  // Numeric keypad 1 key
    KEY_NUMPAD2          = 0x62,  // Numeric keypad 2 key
    KEY_NUMPAD3          = 0x63,  // Numeric keypad 3 key
    KEY_NUMPAD4          = 0x64,  // Numeric keypad 4 key
    KEY_NUMPAD5          = 0x65,  // Numeric keypad 5 key
    KEY_NUMPAD6          = 0x66,  // Numeric keypad 6 key
    KEY_NUMPAD7          = 0x67,  // Numeric keypad 7 key
    KEY_NUMPAD8          = 0x68,  // Numeric keypad 8 key
    KEY_NUMPAD9          = 0x69,  // Numeric keypad 9 key
    KEY_MULTIPLY         = 0x6A,  // Multiply key
    KEY_ADD              = 0x6B,  // Add key
    KEY_SEPARATOR        = 0x6C,  // Separator key
    KEY_SUBTRACT         = 0x6D,  // Subtract key
    KEY_DECIMAL          = 0x6E,  // Decimal key
    KEY_DIVIDE           = 0x6F,  // Divide key
    KEY_F1               = 0x70,  // F1 key
    KEY_F2               = 0x71,  // F2 key
    KEY_F3               = 0x72,  // F3 key
    KEY_F4               = 0x73,  // F4 key
    KEY_F5               = 0x74,  // F5 key
    KEY_F6               = 0x75,  // F6 key
    KEY_F7               = 0x76,  // F7 key
    KEY_F8               = 0x77,  // F8 key
    KEY_F9               = 0x78,  // F9 key
    KEY_F10              = 0x79,  // F10 key
    KEY_F11              = 0x7A,  // F11 key
    KEY_F12              = 0x7B,  // F12 key
    KEY_F13              = 0x7C,  // F13 key
    KEY_F14              = 0x7D,  // F14 key
    KEY_F15              = 0x7E,  // F15 key
    KEY_F16              = 0x7F,  // F16 key
    KEY_F17              = 0x80,  // F17 key
    KEY_F18              = 0x81,  // F18 key
    KEY_F19              = 0x82,  // F19 key
    KEY_F20              = 0x83,  // F20 key
    KEY_F21              = 0x84,  // F21 key
    KEY_F22              = 0x85,  // F22 key
    KEY_F23              = 0x86,  // F23 key
    KEY_F24              = 0x87,  // F24 key
    KEY_NUMLOCK          = 0x90,  // NUM LOCK key
    KEY_SCROLL           = 0x91,  // SCROLL LOCK key
    KEY_LSHIFT           = 0xA0,  // Left SHIFT key
    KEY_RSHIFT           = 0xA1,  // Right SHIFT key
    KEY_LCONTROL         = 0xA2,  // Left CONTROL key
    KEY_RCONTROL         = 0xA3,  // Right CONTROL key
    KEY_LALT             = 0xA4,  // Left ALT key
    KEY_RALT             = 0xA5,  // Right ALT key
    KEY_PLUS             = 0xBB,  // Plus Key   (+)
    KEY_COMMA            = 0xBC,  // Comma Key  (,)
    KEY_MINUS            = 0xBD,  // Minus Key  (-)
    KEY_PERIOD           = 0xBE,  // Period Key (.)
    KEY_ATTN             = 0xF6,  // Attn key
    KEY_CRSEL            = 0xF7,  // CrSel key
    KEY_EXSEL            = 0xF8,  // ExSel key
    KEY_EREOF            = 0xF9,  // Erase EOF key
    KEY_PLAY             = 0xFA,  // Play key
    KEY_ZOOM             = 0xFB,  // Zoom key
    KEY_PA1              = 0xFD,  // PA1 key
    KEY_OEM_CLEAR        = 0xFE,   // Clear key

    KEY_KEY_CODES_COUNT  = 0xFF // this is not a key, but the amount of keycodes there are.
};

//! Enumeration for all events which are sendable by the gui system
enum GuiEventType
{
	//! A gui element has lost its focus.
	guiElementFocusLost = 0,

	//! A gui element has got the focus.
	/** If the event is absorbed then the focus will not be changed. */
	guiElementFocused,

	//! The mouse cursor hovered over a gui element.
	/** If an element has sub-elements you also get this message for the subelements */
	guiElementHovered,

	//! The mouse cursor left the hovered element.
	/** If an element has sub-elements you also get this message for the subelements */
	guiElementLeft,

	//! An element would like to close.
	/** Windows and context menus use this event when they would like to close,
	this can be cancelled by absorbing the event. */
	guiElementClosed,

	//! A button was clicked.
	guiButtonClicked,

	//! A scrollbar has changed its position.
	guiScrollbarChanged,

	//! A checkbox has changed its check state.
	guiCheckboxChanged,

	//! A new item in a listbox was selected.
	/** NOTE: You also get this event currently when the same item was clicked again after more than 500 ms. */
	guiListboxChanged,

	//! An item in the listbox was selected, which was already selected.
	/** NOTE: You get the event currently only if the item was clicked again within 500 ms or selected by "enter" or "space". */
	guiListboxSelectedAgain,

	//! In an editbox 'ENTER' was pressed
	guiEditboxEnter,

	//! The text in an editbox was changed. This does not include automatic changes in text-breaking.
	guiEditboxChanged,

  //! The marked area in an editbox was changed.
  guiEditboxMarkingChanged,

	//! The tab was changed in an tab control
	guiTabChanged,

	//! A menu item was selected in a (context) menu
	guiMenuItemSelected,

	//! The selection in a combo box has been changed
	guiComboboxChanged,

	//! The value of a spin box has changed
	guiSpinboxChanged,

	guiTableHeaderChanged,
	guiTableCellChange,
	guiTableCellSelected,
	guiTableCellDblclick,

	//! No real event. Just for convenience to get number of events
	guiEventCount
};

//! Enumeration for all event types there are.
enum SysEventType
{
  //! An event of the graphical user interface.
  sEventGui = 0,

  //! A mouse input event.
  sEventMouse,

  //! A key input event.
  /** Like mouse events, keyboard events are created by the device and passed to
  handleEvent. They take the same path as mouse events. */
  sEventKeyboard,

  //! A keyboard symbol input event.
  sTextInput,

  sAppEvent,

  sEventUser,

  sEventQuit = 0x400,
  //! This enum is never used, it only forces the compiler to
  //! compile these enumeration values to 32 bit.
  sEventMax = 0x7fffffff
};

//! Enumeration for all mouse input events
enum MouseEventType
{
  //! Left mouse button was pressed down.
  mouseLbtnPressed = 0,

  //! Middle mouse button was pressed down.
  mouseMbtnPressed, //1

  //! Right mouse button was pressed down.
  mouseRbtnPressed, //2

  //! Left mouse button was left up.
  mouseLbtnRelease, //3

  //! Middle mouse button was left up.
  mouseMbtnRelease, //4

  //! Right mouse button was left up.
  mouseRbtnRelease, //5

  //! The mouse cursor changed its position.
  mouseMoved, //6

  //! The mouse wheel was moved. Use Wheel value in event data to find out
  //! in what direction and how fast.
  mouseWheel,

  //! Left mouse button double click.
  mouseLbtDblClick,

  //! Right mouse button double click.
  mouseRbtnDblClick,

  //! Middle mouse button double click.
  mouseMbtnDblClick,

  //! Left mouse button triple click.
  mouseLbtnTrplClick,

  //! Right mouse button triple click.
  mouseRbtnTrplClick,

  //! Middle mouse button triple click.
  mouseMbtnTrplClick,

  //! No real event. Just for convenience to get number of events
  mouseEventCount
};

//! Masks for mouse button states
enum MouseBtnStateMask
{
  mbsmLeft    = 0x01,
  mbsmMiddle  = 0x02,
  mbsmRight   = 0x04,

  //! currently only on windows
  mbsmExtra1  = 0x08,

  //! currently only on windows
  mbsmExtra2  = 0x10,

  mbsmMax = 0x7fffffff
};

enum AppEventType
{
  appWindowFocusLeave = 0,
  appWindowFocusEnter,
  appEventCount
};

//! NEvent hold information about an event
struct NEvent
{
  //! Any kind of GUI event.
  struct _GuiEvent
  {    
    gui::Widget* caller; //! IGUIElement who called the event
    gui::Widget* element; //! If the event has something to do with another element, it will be held here.
    GuiEventType type; //! Type of GUI Event
  };

  struct _AppEvent
  {
    AppEventType type;
  };

  //! Any kind of mouse event.
  struct _MouseEvent
  {    
    int x;  //! X position of mouse cursor
    int y;  //! Y position of mouse cursor

    //! mouse wheel delta, usually 1.0 or -1.0.
    /** Only valid if event was CAESARIA_MOUSE_WHEEL */
    float wheel;

    bool shift:1; //! True if shift was also pressed
    bool control:1;     //! True if ctrl was also pressed

    //! A bitmap of button states. You can use isButtonPressed() to determine
    //! if a button is pressed or not.
    //! Currently only valid if the event was EMIE_MOUSE_MOVED
    unsigned int buttonStates;

    const Point pos() { return Point( x, y ); }
    const Point pos() const { return Point( x, y ); }
    bool isLeftPressed() const { return 0 != ( buttonStates & mbsmLeft ); }     //! Is the left button pressed down?
    bool isRightPressed() const { return 0 != ( buttonStates & mbsmRight ); }   //! Is the right button pressed down?
    bool isMiddlePressed() const { return 0 != ( buttonStates & mbsmMiddle ); } //! Is the middle button pressed down?

    MouseEventType type;     //! Type of mouse event
  };

  //! Any kind of keyboard event.
  struct _KeyboardEvent
  {    
    int symbol; //! Character corresponding to the key (0, if not a character)
    KeyCode key; //! Key which has been pressed or released
    bool pressed:1; //! If not true, then the key was left up
    bool shift:1; //! True if shift was also pressed
    bool control:1; //! True if ctrl was also pressed
  };

  struct _InputEvent
  {
    char text[32];
  };

  //! Any kind of user event.
  struct _UserEvent
  {    
    unsigned int data1; //! Some user specified data as int
    unsigned int data2; //! Another user specified data as int
  };

  SysEventType EventType;
  union
  {
    struct _GuiEvent gui;
    struct _MouseEvent mouse;
    struct _KeyboardEvent keyboard;
    struct _InputEvent text;
    struct _UserEvent user;
    struct _AppEvent app;
  };

  static NEvent Gui( gui::Widget* caller, gui::Widget* elm, GuiEventType type )
  {
    NEvent ret;
    ret.gui.caller = caller;
    ret.gui.element = elm;
    ret.gui.type = type;
    ret.EventType = sEventGui;

    return ret;
  }
};

#endif //__CAESARIA_EVENT_H_INCLUDED__

