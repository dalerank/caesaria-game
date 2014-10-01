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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_LEGION_TARGET_WINDOW_H_INCLUDED__
#define __CAESARIA_LEGION_TARGET_WINDOW_H_INCLUDED__

#include "empiremap_window.hpp"
#include "core/signals.hpp"
#include "core/logger.hpp"

namespace gui
{

class LegionTargetWindow : public EmpireMapWindow
{
public:
  static LegionTargetWindow* create( PlayerCityPtr city, Widget* parent, int id );

  virtual ~LegionTargetWindow();

  // draw on screen
  virtual void draw( gfx::Engine& engine );

  //resolve event
  virtual bool onEvent(const NEvent& event);

public signals:
  Signal1<Point>& onSelectLocation();

protected:
  void _changePosition();

  class Impl;
  ScopedPtr< Impl > _d;

  LegionTargetWindow(  Widget* parent, int id, PlayerCityPtr city );
};

}//end namespace gui
#endif //__CAESARIA_LEGION_TARGET_WINDOW_H_INCLUDED__
