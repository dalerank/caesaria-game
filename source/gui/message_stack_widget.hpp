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

#ifndef __CAESARIA_MESSAGE_STACK_WIDGET_H_INCLUDED__
#define __CAESARIA_MESSAGE_STACK_WIDGET_H_INCLUDED__

#include "widget.hpp"
#include "core/scopedptr.hpp"

namespace gui
{

class WindowMessageStack : public Widget
{
public:
  static const int defaultID;
  typedef enum { positive=0, info, warning } MsgLevel;

  static WindowMessageStack* create( Widget* parent );

  // draw on screen
  virtual void draw( gfx::Engine& engine );
  virtual void beforeDraw( gfx::Engine& painter);
  virtual bool onEvent(const NEvent& event);

  void addMessage( const std::string& text, MsgLevel lvl );
 
private:
  WindowMessageStack( Widget* parent, int id, const Rect& rectangle );

  void _update();
};

}//end namespace gui
#endif //__CAESARIA_MESSAGE_STACK_WIDGET_H_INCLUDED__
