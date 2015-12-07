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

#ifndef __CAESARIA_WAREHOUSE_ORDERS_WINDOW_H_INCLUDED__
#define __CAESARIA_WAREHOUSE_ORDERS_WINDOW_H_INCLUDED__

#include "special_orders_window.hpp"
#include "objects/predefinitions.hpp"

namespace gui
{

class WarehouseSpecialOrdersWindow : public BaseSpecialOrdersWindow
{
public:
  static const int defaultHeight = 550;
  WarehouseSpecialOrdersWindow( Widget* parent, const Point& pos, WarehousePtr warehouse );
  ~WarehouseSpecialOrdersWindow();

private:
  __DECLARE_IMPL(WarehouseSpecialOrdersWindow)
  void _update();
  void _toggleTradeCenter();
  void _toggleDevastation();
};

}//end namesapce gui
#endif //__CAESARIA_WAREHOUSE_ORDERS_WINDOW_H_INCLUDED__
