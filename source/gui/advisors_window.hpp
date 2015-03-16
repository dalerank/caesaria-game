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

#ifndef __CAESARIA_ADVISORSWINDOW_H_INCLUDED__
#define __CAESARIA_ADVISORSWINDOW_H_INCLUDED__

#include "window.hpp"
#include "game/predefinitions.hpp"

namespace gui
{

class PushButton;

namespace advisorwnd
{

class Parlor : public Window
{
public:
  static Parlor* create(Widget* parent, int id, const Advisor type, PlayerCityPtr city  );

  // draw on screen
  virtual void draw( gfx::Engine& engine );

  void showAdvisor( const Advisor type );

  virtual bool onEvent(const NEvent& event);
protected:
  class Impl;
  ScopedPtr< Impl > _d;

  Parlor( Widget* parent, int id );
  PushButton* addButton(Advisor adv, const int picId, std::string="" );
};

}

}//end namespace gui
#endif //__CAESARIA_ADVISORSWINDOW_H_INCLUDED__
