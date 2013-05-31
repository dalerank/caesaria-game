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

#ifndef __OPENCAESAR3_OVERLAYS_MENU_H_INCLUDED__
#define __OPENCAESAR3_OVERLAYS_MENU_H_INCLUDED__

#include "oc3_widget.hpp"
#include "oc3_enums.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_signals.hpp"

class OverlaysMenu : public Widget
{
public:
  OverlaysMenu( Widget* parent, const Rect& rectangle, int id);

  bool isPointInside(const Point& point) const;

  bool onEvent(const NEvent& event);

oc3_signals public:
  Signal1<int>& onSelectOverlayType();

private:
  void _addButtons( const OverlayType type );
  // add the button in the menu.
  void _addButton( const OverlayType buildingType, const std::string& name, const Point& offset );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif