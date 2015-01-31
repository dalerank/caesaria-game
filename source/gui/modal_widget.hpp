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

#ifndef __CAESARIA_MODAL_WIDGET_H_INCLUDED__
#define __CAESARIA_MODAL_WIDGET_H_INCLUDED__

#include "widget.hpp"

namespace gui
{

class ModalScreen : public Widget
{
public:
  static void assignTo(Widget* widget);

  //! constructor
  ModalScreen( Widget* parent, int id=-1 );

  //! called if an event happened.
  virtual bool onEvent(const NEvent& event);

  //! Removes a child.
  virtual void removeChild(Widget* child);

  //! Adds a child
  virtual void addChild(Widget* child);

  //! draws the element and its children
virtual void draw( gfx::Engine& painter );

  //! Modalscreen is not a typical element, but rather acts like a state for it's children.
  //! isVisible is overriden to give this a useful behaviour, so that a modal will no longer
  //! be active when its parent is invisible or all its children are invisible.
  virtual bool visible() const;

  //! Modals are infinite so every point is inside
  virtual bool isPointInside(const Point& point) const;

protected:
  virtual bool _canTakeFocus(Widget* target) const;

  //! Updates the absolute position.
  virtual void _resizeEvent();

private:        
  unsigned int _mouseDownTime;
};

}//end namespace gui
#endif //__CAESARIA_MODAL_WIDGET_H_INCLUDED__
