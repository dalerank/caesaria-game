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
class Ui;

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

  struct {
    //! relative rect of element
    Rect relative;

    //! absolute rect of element
    Rect absolute;

    //! absolute clipping rect of element
    Rect clipping;

    //! the rectangle the element would prefer to be,
    //! if it was not constrained by parent or max/min size
    Rect desired;

    //! for calculating the difference when resizing parent
    Rect lastParent;

    //! relative scale of the element inside its parent
    RectF scale;
  } rect;

  struct {
    Alignment horizontal;
    Alignment vertical;
  } textAlign;

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

    //! is a part of a larger whole and should not be serialized?
    bool internal;
  } flag;


  //! does this element ignore its parent's clipping rectangle?
  bool noClip;

  //! tab order
  int tabOrder;

  //! tab groups are containers like windows, use ctrl+tab to navigate
  bool isTabGroup;

  //runtime properties
  VariantMap properties;

  //! GUI Environment
  Ui* environment;
};

}//end namespace gui
#endif //__CAESARIA_WIDGET_PRIVATE_H_INCLUDE_
