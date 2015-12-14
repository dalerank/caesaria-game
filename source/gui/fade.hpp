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

#ifndef __CAESARIA_FADE_H_INCLUDED__
#define __CAESARIA_FADE_H_INCLUDED__

#include "label.hpp"

namespace gui
{

class Fade : public Label
{
public:
  //
  Fade( Widget *parent, int alpha=0xff, const Rect& rect=Rect() );

  virtual ~Fade();
};

}//end namespace gui

#endif //__CAESARIA_FADE_H_INCLUDED__
