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

#ifndef __OPENCAESAR3_ADVISORSWINDOW_H_INCLUDED__
#define __OPENCAESAR3_ADVISORSWINDOW_H_INCLUDED__

#include "oc3_gui_widget.hpp"
#include "oc3_enums.hpp"
#include "oc3_predefinitions.hpp"

class PushButton;

class AdvisorsWindow : public Widget
{
public:
  /*typedef enum
  {
    ADV_EMPLOYERS=0,
    ADV_LEGION,
    ADV_EMPIRE,
    ADV_RATINGS,
    ADV_TRADING,
    ADV_POPULATION,
    ADV_HEALTH,
    ADV_EDUCATION,
    ADV_ENTERTAINMENT,
    ADV_RELIGION,
    ADV_FINANCE,
    ADV_MAIN,
    ADV_COUNT
  } Type;*/
  static AdvisorsWindow* create( Widget* parent, int id, const AdvisorType type, CityPtr city  );

  // draw on screen
  void draw( GfxEngine& engine );

  void showAdvisor( const AdvisorType type );

  bool onEvent(const NEvent& event);
protected:
  class Impl;
  ScopedPtr< Impl > _d;

  AdvisorsWindow( Widget* parent, int id );
  PushButton* addButton( const int pos, const int picId );
};

#endif
