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


#ifndef __OPENCAESAR3_WIDGET_PRIVATE_H_INCLUDE_
#define __OPENCAESAR3_WIDGET_PRIVATE_H_INCLUDE_

#include "oc3_widget.hpp"

class Widget::Impl
{
public:
	
	//FontsMap overrideFonts;
	//OpacityMap opacity;
	//ColorMap overrideColors;
	//ElementStyle* style;

  //! maximum and minimum size of the element
  Size maxSize, minSize;

  //! Pointer to the parent
  Widget* parent;

	//! List of all children of this element
	Widget::Widgets children;

	//! relative rect of element
	Rect relativeRect;

	//! absolute rect of element
	Rect absoluteRect;

	//! absolute clipping rect of element
	Rect absoluteClippingRect;

	//! the rectangle the element would prefer to be,
	//! if it was not constrained by parent or max/min size
	Rect desiredRect;

	//! for calculating the difference when resizing parent
	Rect lastParentRect;

	//! relative scale of the element inside its parent
	//RectF scaleRect;

	TypeAlign textHorzAlign, textVertAlign;

  std::string internalName;
};

#endif