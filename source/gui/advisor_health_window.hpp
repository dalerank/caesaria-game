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
//
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_ADVISOR_HEALTH_WINDOW_H_INCLUDED__
#define __CAESARIA_ADVISOR_HEALTH_WINDOW_H_INCLUDED__

#include "advisor_base_window.hpp"
#include "core/scopedptr.hpp"
#include "core/signals.hpp"
#include "game/predefinitions.hpp"

namespace gui
{

namespace advisorwnd
{

class Health : public Base
{
public:
  Health( PlayerCityPtr city, Widget* parent );

  virtual void draw( gfx::Engine& painter );

private:
  /**
   * @brief Calls when player press help button
   */
  void _showHelp();
  void _updateAdvice();
  void _initUI();
  void _showDetailInfo(Widget* widget);
  void _moveCamera(Widget* widget);
};

}

}//end namespace gui

#endif //__CAESARIA_ADVISOR_HEALTH_WINDOW_H_INCLUDED__
