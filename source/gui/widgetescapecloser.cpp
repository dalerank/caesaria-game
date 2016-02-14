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
#include "widget_factory.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(WidgetClosers)

WidgetClosers::WidgetClosers(Widget* parent)
  : Widget( parent, -1, Rect() )
{
  parent->installEventHandler( this );
}

void WidgetClosers::addCloseCode(int code)
{
  _codes.insert(code);
}

void WidgetClosers::insertTo(Widget* parent, int code1, int code2)
{
  if( parent )
  {
    auto& widget = parent->add<WidgetClosers>();
    widget.addCloseCode(code1);
    widget.addCloseCode(code2);
  }
}

bool WidgetClosers::onEvent(const NEvent& event)
{
  switch(event.EventType)
  {
  case sEventKeyboard:
    if (!event.keyboard.pressed && _codes.count(event.keyboard.key))
    {
      parent()->deleteLater();
      return true;
    }
  break;

  case sEventMouse:
    if ((event.mouse.type == NEvent::Mouse::mouseLbtnRelease && _codes.count( KEY_LBUTTON ))
        || (event.mouse.type == NEvent::Mouse::mouseMbtnRelease && _codes.count( KEY_MBUTTON ))
        || (event.mouse.type == NEvent::Mouse::mouseRbtnRelease && _codes.count( KEY_RBUTTON )))
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
