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

#ifndef __CAESARIA_SPECIAL_ORDERS_WINDOW_H_INCLUDED__
#define __CAESARIA_SPECIAL_ORDERS_WINDOW_H_INCLUDED__

#include "window.hpp"
#include "objects/predefinitions.hpp"

namespace gui
{

class PushButton;

class BaseSpecialOrdersWindow : public Window
{
public:
  virtual void draw( gfx::Engine& engine );  // draw on screen

  virtual bool onEvent( const NEvent& event);

  virtual bool isPointInside(const Point& point) const;

  virtual void setTitle( const std::string& text );

  virtual ~BaseSpecialOrdersWindow();
protected:
  Widget* _ordersArea();
  BaseSpecialOrdersWindow(Widget* parent, const Point& pos , int height);

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namesapce gui
#endif //__CAESARIA_SPECIAL_ORDERS_WINDOW_H_INCLUDED__
