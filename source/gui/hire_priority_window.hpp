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

#ifndef _CAESARIA_HIRE_PRIORITY_WINDOW_H_INCLUDE_
#define _CAESARIA_HIRE_PRIORITY_WINDOW_H_INCLUDE_

#include "window.hpp"
#include "core/signals.hpp"
#include "game/enums.hpp"
#include "city/industry.hpp"

namespace gui
{

namespace dialog
{

class HirePriority : public Window
{
public:
  HirePriority( Widget* parent, city::industry::Type type, int priority );
  virtual ~HirePriority();

  virtual bool onEvent(const NEvent &event);

public signals:
  Signal2<city::industry::Type, int>& onAcceptPriority();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace dialog

}//end namespace gui
#endif //_CAESARIA_HIRE_PRIORITY_WINDOW_H_INCLUDE_
