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

#ifndef __CAESARIA_GRANARY_ORDERS_WINDOW_H_INCLUDED__
#define __CAESARIA_GRANARY_ORDERS_WINDOW_H_INCLUDED__

#include "special_orders_window.hpp"

namespace gui
{

class GranarySpecialOrdersWindow : public BaseSpecialOrdersWindow
{
public:
  static const int defaultHeight = 250;
  GranarySpecialOrdersWindow( Widget* parent, const Point& pos, GranaryPtr granary );
  ~GranarySpecialOrdersWindow();

  void toggleDevastation();
private:
  void _updateBtnDevastation();

  __DECLARE_IMPL(GranarySpecialOrdersWindow)
};

}//end namesapce gui
#endif //__CAESARIA_GRANARY_ORDERS_WINDOW_H_INCLUDED__
