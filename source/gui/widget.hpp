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

#ifndef __CAESARIA_WIDGET_H_INCLUDE_
#define __CAESARIA_WIDGET_H_INCLUDE_

#include <memory>

#include "core/referencecounted.hpp"
#include "core/list.hpp"
#include "core/rectangle.hpp"
#include "core/alignment.hpp"
#include "core/font.hpp"
#include "core/smartptr.hpp"
#include "core/variant.hpp"
#include "vfs/path.hpp"

namespace gfx
{
 class Engine;
}
struct NEvent;

namespace gui
{

class Ui;

class Widget : public virtual ReferenceCounted
{
public:       
  typedef List<Widget*> Widgets;
  typedef Widgets::iterator ChildIterator;
  typedef Widgets::const_iterator ConstChildIterator;

  typedef enum { RelativeGeometry=0, AbsoluteGeometry, ProportionalGeometry } GeometryType;
  enum { noId=-1 };

  Widget( Widget* parent, int id, const Rect& rectangle );

  std::string internalName() const;
  void setInternalName( const std::string& name );

  template< class T >
  List< T > findChildren()
  {
    List< T > ret;
    foreach( it, children() )
    {
      if( T elm = safety_cast< T >( *it ) )
          ret.push_back( elm );
    }

    return ret;
  }

  virtual bool isHovered() const;

  virtual bool isFocused() const;

  virtual void setFocus();
	virtual void removeFocus();

	virtual void beforeDraw( gfx::Engine& painter );

	virtual Rect clientRect() const;

	//! Sets another skin independent font.
	/** If this is set to zero, the button uses the font of the skin.
	\param font: New font to set. */
  //virtual void setFont( Font font, u32 nA=0 );

  //! Gets the override font (if any)
  /** \return The override font (may be 0) */
  //virtual Font getFont( u32 index=0 ) const;
  
  virtual Ui* ui();

  //! Sets text justification mode
  /** \param horizontal: EGUIA_UPPERLEFT for left justified (default),
	*					   ALIGN_LOWEERRIGHT for right justified, or ALIGN_CENTER for centered text.
	*	\param vertical: ALIGN_UPPERLEFT to align with top edge,
	*					 ALIGN_LOWEERRIGHT for bottom edge, or ALIGN_CENTER for centered text (default). 
	*/
  virtual void setTextAlignment( align::Type horizontal, align::Type vertical );

  virtual Alignment horizontalTextAlign() const;

  virtual Alignment verticalTextAlign() const;

  virtual void hide();

  virtual void show();

  virtual void setMaxWidth( unsigned int width );

  virtual void setWidth( unsigned int width );

  virtual void setHeight( unsigned int height );

  virtual unsigned int height() const;

  virtual unsigned int width() const;

  virtual int screenTop() const;

  virtual int screenLeft() const;

  virtual int bottom() const;

  virtual Point center() const;

  virtual void setBottom(int b);

  virtual int screenBottom() const;

  virtual int screenRight() const;

  virtual Point leftbottom() const;

  virtual Point lefttop() const;

  virtual Point righttop() const;

  virtual Point rightbottom() const;

  virtual Point localToScreen( const Point& localPoint ) const;

  virtual Rect localToScreen( const Rect& localPoint ) const;

  virtual Point screenToLocal( const Point& screenPoint ) const;

  virtual Size size() const;

  //! Updates the absolute position.
  virtual void updateAbsolutePosition();

  //! Returns true if a point is within this element.
  /** Elements with a shape other than a rectangle should override this method */
  virtual bool isPointInside(const Point& point) const;

  //! Adds a GUI element as new child of this element.
  virtual void addChild(Widget* child);

  //! Removes a child.
  virtual void removeChild(Widget* child);

  //! Removes this element from its parent.
  virtual void remove();

  //! Draws the element and its children.
  virtual void draw( gfx::Engine& painter );

  virtual void animate( unsigned int timeMs );

  //! Destructor
  virtual ~Widget();

  //! Moves this element in absolute point.
  virtual void setPosition(const Point& relativePosition);

  //! Moves this element on relative distance.
  virtual void move( const Point& offset );

  //! Returns true if element is visible.
  virtual bool visible() const;

  //! Sets the visible state of this element.
  virtual void setVisible( bool visible );

  //! Returns true if this element was created as part of its parent control
  virtual bool isSubElement() const;

  //! Sets whether this control was created as part of its parent.
  /** For example, it is true when a scrollbar is part of a listbox.
  SubElements are not saved to disk when calling guiEnvironment->saveGUI() */
  virtual void setSubElement(bool subElement);

  //! If set to true, the focus will visit this element when using the tab key to cycle through elements.
  /** If this element is a tab group (see isTabGroup/setTabGroup) then
  ctrl+tab will be used instead. */
  virtual void setTabStop(bool enable);

  //! Returns true if this element can be focused by navigating with the tab key
  virtual bool isTabStop() const;

  //! Sets the priority of focus when using the tab key to navigate between a group of elements.
  /** See setTabGroup, isTabGroup and getTabGroup for information on tab groups.
  Elements with a lower number are focused first */
  virtual void setTabOrder( int index );

  //! Returns the number in the tab order sequence
  virtual int tabOrder() const;

  //! Sets whether this element is a container for a group of elements which can be navigated using the tab key.
  /** For example, windows are tab groups.
  Groups can be navigated using ctrl+tab, providing isTabStop is true. */
  virtual void setTabgroup(bool isGroup);

  //! Returns true if this element is a tab group.
  virtual bool hasTabgroup() const;

  //! Returns the container element which holds all elements in this element's tab group.
  virtual Widget* tabgroup();

  //! Returns true if element is enabled
  /** Currently elements do _not_ care about parent-states.
  So if you want to affect childs you have to enable/disable them all.
  The only exception to this are sub-elements which also check their parent.
  */
  virtual bool enabled() const;

  //! Sets the enabled state of this element.
  virtual void setEnabled(bool enabled);

  //! Sets the new caption of this element.
  virtual void setText(const std::string& text);

  //! Returns caption of this element.
  virtual std::string text() const;

  //! Sets the new caption of this element.
  virtual void setTooltipText(const std::string& text);

  //! Returns caption of this element.
  virtual std::string tooltipText() const;

  //! Returns id. Can be used to identify the element.
  virtual int ID() const;

  //! Sets the id of this element
  virtual void setID(int id);

  //! Called if an event happened.
  virtual bool onEvent(const NEvent& event);

  //! Brings a child to front
  /** \return True if successful, false if not. */
  virtual bool bringChildToFront(Widget* element);

  //! Brings a widget to front
  /** \return true if successful, false if not. */
  virtual bool bringToFront();

  //! Moves a child to the back, so it's siblings are drawn on top of it
  /** \return True if successful, false if not. */
  virtual bool sendChildToBack(Widget* child);

  //! Moves widget to the back, so it's siblings are drawn on top of it
  /** \return True if successful, false if not. */
  virtual bool sendToBack();

  //! Returns list with children of this element
  virtual const Widgets& children() const;

  //! Finds the first element with the given id.
  /** \param id: Id to search for.
   *	\param searchchildren: Set this to true, if also children of this
   *		element may contain the element with the searched id and they
   *       should be searched too.
   *	\return Returns the first element with the given id. If no element
   *	with this id was found, 0 is returned.
   */
  virtual Widget* findChild(int id, bool searchchildren=false) const;

  //! Reads attributes of the scene node.
  /** Implement this to set the attributes of your scene node for
   *	scripting languages, editors, debuggers or xml deserialization purposes.
   */
  virtual void setupUI(const VariantMap& options );
  virtual void setupUI( const vfs::Path& filename );

  virtual void installEventHandler( Widget* elementHandler );

  //non overriding methods
  //!!! Returns parent of this element.
  Widget* parent() const;

  //!
  void deleteLater();

  //! Returns the relative rectangle of this element.
  Rect relativeRect() const;

  //! Sets the relative/absolute rectangle of this element.
  /** \param r The absolute position to set */
  void setGeometry(const Rect& r, GeometryType mode=RelativeGeometry );

  void setGeometry(const RectF& r, GeometryType mode=ProportionalGeometry);

  //! 
  void setLeft( int newLeft );

  //!
  void setCenter( Point center );

  //!
  int left() const;

  //!
  int right() const;

  //!
  int top() const;

  //!
  void setTop( int newTop );

  //! Gets the absolute rectangle of this element
  Rect absoluteRect() const;

  //! Returns the visible area of the element.
  Rect absoluteClippingRect() const;

  //! Returns the visible area of the element.
  Rect& absoluteClippingRectRef() const;

  //! Sets whether the element will ignore its parent's clipping rectangle
  /** \param noClip If true, the element will not be clipped by its parent's clipping rectangle. */
  void setNotClipped(bool noClip);

  //! Gets whether the element will ignore its parent's clipping rectangle
  /** \return true if the element is not clipped by its parent's clipping rectangle. */
  bool isNotClipped() const;

  //! Sets the maximum size allowed for this element
  /** If set to 0,0, there is no maximum size */
  void setMaxSize( const Size& size);

  Size maxSize() const;

  Size minSize() const;

  //! Sets the minimum size allowed for this element
  void setMinSize( const Size& size);

  //! The alignment defines how the borders of this element will be positioned when the parent element is resized.
  void setAlignment(Alignment left, Alignment right, Alignment top, Alignment bottom);

  //! Returns the topmost GUI element at the specific position.
  /**
   * This will check this GUI element and all of its descendants, so it
   * may return this GUI element.  To check all GUI elements, call this
   * function on device->getGUIEnvironment()->getRootGUIElement(). Note
   * that the root element is the size of the screen, so doing so (with
   * an on-screen point) will always return the root element if no other
   * element is above it at that point.
   * \param point: The point at which to find a GUI element.
   * \return The topmost GUI element at that point, or 0 if there are
   * no candidate elements at this point.
   */
  Widget* getElementFromPoint(const Point& point);

  //! returns true if the given element is a child of this one.
  //! \param child: The child element to check
  bool isMyChild(Widget* child) const;

  //! searches elements to find the closest next element to tab to
  /** \param startOrder: The TabOrder of the current element, -1 if none
    *  \param reverse: true if searching for a lower number
    *  \param group: true if searching for a higher one
    *  \param first: element with the highest/lowest known tab order depending on search direction
    *  \param closest: the closest match, depending on tab order and direction
    *  \param includeInvisible: includes invisible elements in the search (default=false)
    *  \return true if successfully found an element, false to continue searching/fail 
	 */
  bool next( int startOrder, bool reverse, bool group,
                      Widget*& first, Widget*& closest, bool includeInvisible=false) const;

  void setParent( Widget* parent );

  void setRight(int newRight);

protected:

  /*!
   * This event handler can be reimplemented in a subclass to receive
   * widget resize events which are passed in the \a event parameter.
   * When _resizeEvent() is called, the widget already has its new
   * geometry.
   */
  virtual void _resizeEvent();

  Widgets& _getChildren();

protected:
  // not virtual because needed in constructor
  void _addChild(Widget* child);

  // not virtual because needed in constructor
  void _recalculateAbsolutePosition(bool recursive);

  __DECLARE_IMPL(Widget)

  //! GUI Environment
  Ui* _environment;
};

typedef SmartPtr< Widget > WidgetPtr;

enum ElementState
{
  stNormal=0, 
  stPressed, 
  stHovered, 
  stDisabled, 
  stChecked,
  StateCount
};

}//end namespace gui
#endif //__CAESARIA_WIDGET_H_INCLUDE_
