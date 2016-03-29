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

#ifndef _CAESARIA_WIDGETESCAPECLOSER_H_INCLUDE_
#define _CAESARIA_WIDGETESCAPECLOSER_H_INCLUDE_

#include "widget.hpp"
#include "core/event.hpp"
#include <set>

namespace gui
{

class WidgetClosers : public Widget
{
  std::set<int> _codes;
public:
  WidgetClosers(Widget* parent);
  void addCloseCode(int code);
  static void insertTo( Widget* parent, int code1=KEY_KEY_CODES_COUNT, int code2=KEY_ESCAPE );
  virtual bool onEvent(const NEvent &event);
};

}//end namesapce gui

#endif //_CAESARIA_WIDGETESCAPECLOSER_H_INCLUDE_
