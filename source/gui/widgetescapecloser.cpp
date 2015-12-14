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

#include "widgetescapecloser.hpp"
#include "core/event.hpp"

namespace gui
{

WidgetClose::WidgetClose(Widget* parent)
  : Widget( parent, -1, Rect() )
{
  parent->installEventHandler( this );
}

void WidgetClose::insertTo(Widget* parent, KeyCode code1, KeyCode code2)
{
  if( parent )
  {
    auto& widget = parent->add<WidgetClose>();
    widget._codes.insert( code1 );
    widget._codes.insert( code2 );
  }
}

bool WidgetClose::onEvent(const NEvent& event)
{
  switch(event.EventType)
  {
  case sEventKeyboard:
    if( !event.keyboard.pressed && _codes.count( event.keyboard.key ) )
    {
      parent()->deleteLater();
      return true;
    }
  break;

  case sEventMouse:
    if( (event.mouse.type == mouseLbtnRelease && _codes.count( KEY_LBUTTON ))
        || (event.mouse.type == mouseMbtnRelease && _codes.count( KEY_MBUTTON ))
        || (event.mouse.type == mouseRbtnRelease && _codes.count( KEY_RBUTTON ))  )
    {
      parent()->deleteLater();
      return true;
    }
  break;

  default: break;
  }

  return false;
}

}//end namespace gui
