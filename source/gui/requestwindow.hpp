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

#ifndef __CAESARIA_EMPERROR_REQUEST_WINDOW_H_INCLUDED__
#define __CAESARIA_EMPERROR_REQUEST_WINDOW_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"
#include "widget.hpp"
#include "core/signals.hpp"
#include "city/request.hpp"

namespace gui
{

class EmperrorRequestWindow : public Widget
{
public:
  static EmperrorRequestWindow* create(Widget* parent, CityRequestPtr request , bool mayExec);
  ~EmperrorRequestWindow();

  virtual void draw( GfxEngine& painter );

  virtual bool onEvent(const NEvent &event);

private:
  EmperrorRequestWindow( Widget* parent, CityRequestPtr request );

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //__CAESARIA_EMPERROR_REQUEST_WINDOW_H_INCLUDED__
