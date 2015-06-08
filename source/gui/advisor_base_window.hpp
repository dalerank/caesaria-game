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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_ADVISORBASEWINDOW_H_INCLUDED__
#define __CAESARIA_ADVISORBASEWINDOW_H_INCLUDED__

#include "window.hpp"
#include "game/predefinitions.hpp"
#include "core/scopedptr.hpp"
#include "core/signals.hpp"

namespace gui
{

namespace advisorwnd
{

static const int defaultWidth = 640;
static const int defaultHeight = 484;

class Base : public Window
{
public:
  Base( Widget* parent, PlayerCityPtr city, int id=-1 )
    : Window( parent, Rect( 0, 0, 1, 1 ), "", id )
  {
    _city = city;
    setPosition( Point( (parent->width() - defaultWidth ), (parent->height() - defaultHeight) )/2 );
  }

protected:
  PlayerCityPtr _city;
};

}

}//end namespace gui
#endif //__CAESARIA_ADVISORBASEWINDOW_H_INCLUDED__
