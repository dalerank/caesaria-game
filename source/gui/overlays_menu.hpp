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

#ifndef __CAESARIA_OVERLAYS_MENU_H_INCLUDED__
#define __CAESARIA_OVERLAYS_MENU_H_INCLUDED__

#include "widget.hpp"
#include "game/enums.hpp"
#include "core/scopedptr.hpp"
#include "core/signals.hpp"

namespace gui
{

class OverlaysMenu : public Widget
{
public:
  OverlaysMenu( Widget* parent, const Rect& rectangle, int id);

  bool isPointInside(const Point& point) const;

  bool onEvent(const NEvent& event);

signals public:
  Signal1<int>& onSelectOverlayType();

private:
  void _addButtons( const int type );
  // add the button in the menu.
  void _addButton(const int buildingType, const Point& offset );

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //__CAESARIA_OVERLAYS_MENU_H_INCLUDED__
