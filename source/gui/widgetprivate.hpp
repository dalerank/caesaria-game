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

#ifndef __CAESARIA_WIDGET_PRIVATE_H_INCLUDE_
#define __CAESARIA_WIDGET_PRIVATE_H_INCLUDE_

#include "core/variant_map.hpp"
#include "core/alignment.hpp"
#include "core/list.hpp"
#include <set>

namespace gui
{

class Widget;

class Widget::Impl
{
public:
	std::set<Widget*> eventHandlers;

  //! maximum and minimum size of the element
  struct
  {
   Size maximimum;
   Size mininimum;
  } size;

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
	RectF scaleRect;

	Alignment textHorzAlign, textVertAlign;

  std::string internalName;

  std::string toolTipText;

  std::string text;

  //! tells the element how to act when its parent is resized
  struct {
    Alignment left,
              right,
              top,
              bottom;
  } align;

  //! id
  int id;

  struct {
    //! tab stop like in windows
    bool tabStop;

    //! is visible?
    bool visible;

    //! is enabled?
    bool enabled;
  } flag;

  //! is a part of a larger whole and should not be serialized?
  bool isSubElement;

  //! does this element ignore its parent's clipping rectangle?
  bool noClip;

  //! tab order
  int tabOrder;

  //! tab groups are containers like windows, use ctrl+tab to navigate
  bool isTabGroup;

  //runtime properties
  VariantMap properties;
};

}//end namespace gui
#endif //__CAESARIA_WIDGET_PRIVATE_H_INCLUDE_
