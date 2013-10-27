// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_ADVISOR_EMPEROR_WINDOW_H_INCLUDED__
#define __OPENCAESAR3_ADVISOR_EMPEROR_WINDOW_H_INCLUDED__

#include "widget.hpp"
#include "core/predefinitions.hpp"
#include "core/scopedptr.hpp"
#include "core/signals.hpp"
#include "core/event.hpp"

namespace gui
{

class AdvisorEmperorWindow : public Widget
{
public:
  AdvisorEmperorWindow(Widget* parent, int maxMoney, int id );

  void draw( GfxEngine& painter );

  bool onEvent(const NEvent &event);

public oc3_signals:
  Signal1<int>& onChangeSalary();
  Signal1<int>& onSendMoney();

protected:
  void _showChangeSalaryWindow();
  void _showSend2CityWindow();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //__OPENCAESAR3_ADVISOR_EMPEROR_WINDOW_H_INCLUDED__
