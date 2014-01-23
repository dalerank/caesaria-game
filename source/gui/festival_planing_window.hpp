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

#ifndef __CAESARIA_FESTIVAL_PLANING_WINDOW_H_INCLUDED__
#define __CAESARIA_FESTIVAL_PLANING_WINDOW_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"
#include "widget.hpp"
#include "core/signals.hpp"

namespace gui
{

class FestivalPlaningWindow : public Widget
{
public:
  static FestivalPlaningWindow* create( Widget* parent, PlayerCityPtr city, int id );
  ~FestivalPlaningWindow();

  virtual void draw( GfxEngine& painter );

  virtual bool onEvent(const NEvent &event);

public oc3_signals:
  Signal2<int,int>& onFestivalAssign();

private:
  FestivalPlaningWindow( Widget* parent, int id, const Rect& rectangle, PlayerCityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //__CAESARIA_FESTIVAL_PLANING_WINDOW_H_INCLUDED__
