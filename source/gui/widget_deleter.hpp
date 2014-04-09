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

#ifndef _CAESARIA_WIDGET_DELETER_INCLUDE_
#define _CAESARIA_WIDGET_DELETER_INCLUDE_

#include "widgetanimator.hpp"

namespace gui
{

class WidgetDeleter : public WidgetAnimator
{
public:
  WidgetDeleter( Widget* node, size_t time );
  ~WidgetDeleter(void);

  virtual void draw( gfx::Engine& painter );

private:
  unsigned int _delay;
  unsigned int _startTime;
};

}//end namespace gui
#endif //_CAESARIA_WIDGET_DELETER_INCLUDE_
