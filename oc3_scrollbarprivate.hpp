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

#ifndef _OPENCAESAR3_SCROLLBAR_PRIVATE_H_INCLUDE_
#define _OPENCAESAR3_SCROLLBAR_PRIVATE_H_INCLUDE_

#include "oc3_picture.hpp"
#include "core/signals.hpp"
#include "oc3_rectangle.hpp"

class PushButton;

class ScrollBar::Impl
{
oc3_signals public:
	Signal1<int> onPositionChanged;

public:
	Rect textureRect;
	Rect	sliderTextureRect;
	Rect	backgroundRect;
	Rect	filledAreaRect;
	Rect	sliderRect;
	PushButton* upButton;
	PushButton* downButton;
  bool needRecalculateParams;
  Point cursorPos;

	Picture texture;
  Picture sliderPictureUp, sliderPictureDown;
	Picture sliderTexture;
};

#endif //_OPENCAESAR3_SCROLLBAR_PRIVATE_H_INCLUDE_
