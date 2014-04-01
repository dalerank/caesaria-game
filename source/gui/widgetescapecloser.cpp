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

WidgetEscapeCloser::WidgetEscapeCloser(gui::Widget* parent)
  : Widget( parent, -1, Rect() )
{
  parent->installEventHandler( this );
}

void WidgetEscapeCloser::insertTo(Widget* parent)
{
  if( parent )
  {
    new WidgetEscapeCloser( parent );
  }
}

bool WidgetEscapeCloser::onEvent(const NEvent& event)
{
  if( event.EventType == sEventKeyboard && event.keyboard.pressed && event.keyboard.key == KEY_ESCAPE )
  {
    parent()->deleteLater();
    return true;
  }

  return false;
}

}//end namespace gui
