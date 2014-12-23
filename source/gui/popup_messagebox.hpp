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

#ifndef __CAESARIA_POPUP_MESSAGEBOX_H_INCLUDED__
#define __CAESARIA_POPUP_MESSAGEBOX_H_INCLUDED__

#include "window.hpp"
#include "core/scopedptr.hpp"
#include "core/signals.hpp"

namespace gui
{

namespace messagebox
{

class Popup : public Window
{
public:
  Popup(Widget* parent,
                   const std::string& title,
                   const std::string& text,
                   const DateTime& time,
                   const std::string& receiver="",
                   int id=-1 );

  void draw( gfx::Engine& painter );

  static Popup* information( Widget* parent,
                             const std::string& title,
                                       const std::string& text,
                                       const DateTime& time );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace messagebox

}//end namespace gui

#endif //__CAESARIA_POPUP_MESSAGEBOX_H_INCLUDED__
