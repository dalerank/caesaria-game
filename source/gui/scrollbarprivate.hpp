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

#ifndef _CAESARIA_SCROLLBAR_PRIVATE_H_INCLUDE_
#define _CAESARIA_SCROLLBAR_PRIVATE_H_INCLUDE_

#include "gfx/picture.hpp"
#include "core/signals.hpp"
#include "core/rectangle.hpp"

namespace gui
{

class PushButton;

class ScrollBar::Impl
{
signals public:
	Signal1<int> onPositionChanged;

public:
	Rect	sliderTextureRect;
	Rect	backgroundRect;
	Rect	filledAreaRect;
  bool needRecalculateParams;
  Point cursorPos;
  unsigned int lastTimeChange;

  struct {
    gfx::Picture texture;
    Rect rect;
  } background;

  struct {
    struct {
      gfx::Picture img;
      PushButton* widget;
    } up;

    struct {
      gfx::Picture img;
      PushButton* widget;
    } down;
  } button;

  struct {
    gfx::Picture texture;
    Rect         rect;
  } slider;
};

}//end namespace gui

#endif //_CAESARIA_SCROLLBAR_PRIVATE_H_INCLUDE_
