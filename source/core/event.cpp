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

#include "event.hpp"
#include <string.h>

NEvent NEvent::ev_gui(gui::Widget* caller, gui::Widget* elm, GuiEventType type)
{
  NEvent ret;
  ret.gui.caller = caller;
  ret.gui.element = elm;
  ret.gui.type = type;
  ret.EventType = sEventGui;

  return ret;
}

NEvent NEvent::ev_none()
{
  NEvent ret;
  memset( &ret, 0, sizeof(NEvent) );
  ret.EventType = sEventMax;

  return ret;
}
