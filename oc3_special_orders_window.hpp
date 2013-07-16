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


#ifndef __OPENCAESAR3_SPECIAL_ORDERS_WINDOW_H_INCLUDED__
#define __OPENCAESAR3_SPECIAL_ORDERS_WINDOW_H_INCLUDED__

#include "oc3_widget.hpp"
#include "oc3_predefinitions.hpp"

class PushButton;

class BaseSpecialOrdersWindow : public Widget
{
public:
  virtual void draw( GfxEngine& engine );  // draw on screen

  virtual bool onEvent( const NEvent& event);

  virtual bool isPointInside(const Point& point) const;

  virtual void setTitle( const std::string& text );

  ~BaseSpecialOrdersWindow();
protected:
  BaseSpecialOrdersWindow( Widget* parent, const Point& pos );

  class Impl;
  ScopedPtr< Impl > _d;
};

class GranarySpecialOrdersWindow : public BaseSpecialOrdersWindow
{
public:
  GranarySpecialOrdersWindow( Widget* parent, const Point& pos, GranaryPtr granary );

  void toggleDevastation();
private:
  void _updateBtnDevastation();

  GranaryPtr _granary;
  PushButton* _btnToggleDevastation;
};

class WarehouseSpecialOrdersWindow : public BaseSpecialOrdersWindow
{
public:
  WarehouseSpecialOrdersWindow( Widget* parent, const Point& pos, WarehousePtr warehouse );

  void toggleDevastation();
private:
  void _updateBtnDevastation();

  WarehousePtr _warehouse;
  PushButton* _btnToggleDevastation;
  PushButton* _btnTradeCenter;
};

#endif //__OPENCAESAR3_SPECIAL_ORDERS_WINDOW_H_INCLUDED__
