#ifndef __OC3_MODAL_WIDGET_H_INCLUDED__
#define __OC3_MODAL_WIDGET_H_INCLUDED__

#include "widget.hpp"

namespace gui
{

class ModalScreen : public Widget
{
public:
	//! constructor
	ModalScreen( Widget* parent, int id=-1 );

	//! called if an event happened.
	virtual bool onEvent(const NEvent& event);

	//! Removes a child.
	virtual void removeChild(Widget* child);

	//! Adds a child
	virtual void addChild(Widget* child);

	//! draws the element and its children
	virtual void draw( GfxEngine& painter );

  //! Modalscreen is not a typical element, but rather acts like a state for it's children.
  //! isVisible is overriden to give this a useful behaviour, so that a modal will no longer
  //! be active when its parent is invisible or all its children are invisible.
	virtual bool isVisible() const;

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
#endif //__OC3_MODAL_WIDGET_H_INCLUDED__
