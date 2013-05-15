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


#ifndef __OPENCAESAR3_WIDGET_H_INCLUDE_
#define __OPENCAESAR3_WIDGET_H_INCLUDE_

#include <memory>

#include "oc3_referencecounted.hpp"
#include "oc3_list.hpp"
#include "oc3_rectangle.hpp"
#include "oc3_alignment.hpp"
#include "oc3_safetycast.hpp"

class GfxEngine;
class GuiEnv;
struct NEvent;
class WidgetPrivate;

class Widget : public virtual ReferenceCounted
{
public:       
    typedef List<Widget*> Widgets;
	typedef Widgets::iterator ChildIterator;
	typedef Widgets::const_iterator ConstChildIterator;

	typedef enum { RelativeGeometry=0, AbsoluteGeometry, ProportionalGeometry } GeometryType;

    Widget( Widget* parent, int id, const Rect& rectangle );

//    virtual f32 getOpacity( u32 index=0 ) const;
//    virtual void setOpacity( f32 nA, s32 index=0 );

    std::string getInternalName() const;
    void setInternalName( const std::string& name );

    template< class T >
    T findChild( const std::string& internalName, bool recursiveFind = false )
    {
        Widgets::const_iterator it = getChildren().begin();
        for( ; it != getChildren().end(); it++ )
        {
            if( (*it)->getInternalName() == internalName )
                return safety_cast< T >( *it );

            if( recursiveFind )
            {
                T chElm = (*it)->findChild< T >( internalName, recursiveFind );
                if( chElm )
                    return chElm;
            }
        }

        return 0;
    }

    template< class T >
    List< T > findChildren()
    {
        List< T > ret;
        ConstChildIterator it = getChildren().begin();
        for( ; it != getChildren().end(); it++ )
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

  virtual void beforeDraw( GfxEngine& painter );

  virtual Rect getClientRect() const;

    //! Sets another skin independent font.
	/** If this is set to zero, the button uses the font of the skin.
	\param font: New font to set. */
    //virtual void setFont( Font font, u32 nA=0 );

    //! Gets the override font (if any)
    /** \return The override font (may be 0) */
    //virtual Font getFont( u32 index=0 ) const;
    
    virtual GuiEnv* getEnvironment();

    //! Sets text justification mode
    /** \param horizontal: EGUIA_UPPERLEFT for left justified (default),
	 *					   ALIGN_LOWEERRIGHT for right justified, or ALIGN_CENTER for centered text.
	 *	\param vertical: ALIGN_UPPERLEFT to align with top edge,
	 *					 ALIGN_LOWEERRIGHT for bottom edge, or ALIGN_CENTER for centered text (default). 
	 */
    virtual void setTextAlignment( TypeAlign horizontal, TypeAlign vertical );

	virtual TypeAlign getHorizontalTextAlign() const;

	virtual TypeAlign getVerticalTextAlign() const;

    virtual void styleChanged();

	virtual void hide();

	virtual void show();

    virtual void setMaxWidth( unsigned int width );

    virtual void setWidth( unsigned int width );

	virtual void setHeight( unsigned int height );

    virtual unsigned int getHeight() const;

	virtual unsigned int getWidth() const;

	virtual int getScreenTop() const;

	virtual int getScreenLeft() const;

    virtual int getBottom() const;
	virtual int getScreenBottom() const;

	virtual int getScreenRight() const;

	virtual unsigned int getArea() const;

	virtual Point convertLocalToScreen( const Point& localPoint ) const;

	virtual Rect convertLocalToScreen( const Rect& localPoint ) const;

	virtual Size getSize() const;

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
	virtual void draw( GfxEngine& painter );

  virtual void animate( unsigned int timeMs );
	
  //! Checks if an override color is enabled
	/** \return true if the override color is enabled, false otherwise */
	//virtual bool isColorEnabled( unsigned int index=0 ) const;

  //! Destructor
  virtual ~Widget();

	//! Moves this element in absolute point.
	virtual void setPosition(const Point& relativePosition);

	//! Moves this element on relative distance.
	virtual void move( const Point& offset );

	//! Returns true if element is visible.
	virtual bool isVisible() const;

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
	virtual int getTabOrder() const;

	//! Sets whether this element is a container for a group of elements which can be navigated using the tab key.
	/** For example, windows are tab groups.
	Groups can be navigated using ctrl+tab, providing isTabStop is true. */
	virtual void setTabGroup(bool isGroup);

	//! Returns true if this element is a tab group.
	virtual bool hasTabGroup() const;

	//! Returns the container element which holds all elements in this element's tab group.
	virtual Widget* getTabGroup();

	//! Returns true if element is enabled
	/** Currently elements do _not_ care about parent-states.
	So if you want to affect childs you have to enable/disable them all.
	The only exception to this are sub-elements which also check their parent.
	*/
	virtual bool isEnabled() const;

	//! Sets the enabled state of this element.
	virtual void setEnabled(bool enabled);

	//! Sets the new caption of this element.
    virtual void setText(const std::string& text);

	//! Returns caption of this element.
    virtual std::string getText() const;

	//! Sets the new caption of this element.
    virtual void setTooltipText(const std::string& text);

	//! Returns caption of this element.
    virtual std::string getTooltipText() const;

	//! Returns id. Can be used to identify the element.
	virtual int getID() const;

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
	virtual const Widgets& getChildren() const;

	//! Finds the first element with the given id.
	/** \param id: Id to search for.
	 *	\param searchchildren: Set this to true, if also children of this
	 *		element may contain the element with the searched id and they
	 *       should be searched too.
	 *	\return Returns the first element with the given id. If no element
	 *	with this id was found, 0 is returned. 
	 */
	virtual Widget* findChild(int id, bool searchchildren=false) const;

	//! Sets if the static text should use the overide color or the color in the gui skin.
	/** \param enable: If set to true, the override color, which can be set
	 */
	//virtual void setEnabledColor(bool enable, u32 index=0);

	//! Writes attributes of the scene node.
	/** Implement this to expose the attributes of your scene node for
	 *	scripting languages, editors, debuggers or xml serialization purposes. 
	 */
	//virtual void save( core::VariantArray* out ) const;

	//! Reads attributes of the scene node.
	/** Implement this to set the attributes of your scene node for
	 *	scripting languages, editors, debuggers or xml deserialization purposes. 
	 */
	//virtual void load( core::VariantArray* in );

	virtual void installEventHandler( Widget* elementHandler );

    //non overriding methods
	//! Returns parent of this element.
    Widget* getParent() const;

	//! 
    void deleteLater();

    //! Returns the relative rectangle of this element.
    Rect getRelativeRect() const;

	//! Sets the relative/absolute rectangle of this element.
	/** \param r The absolute position to set */
	void setGeometry(const Rect& r, GeometryType mode=RelativeGeometry );

    //! 
    void setLeft( int newLeft );

    //!
    int getLeft() const;

    //!
    int getRight() const;

    //!
    int getTop() const;

	//!
	void setTop( int newTop );

    //! Sets the relative rectangle of this element as a proportion of its parent's area.
    /** \note This method used to be 'void setRelativePosition(const core::rect<f32>& r)'
        \param r  The rectangle to set, interpreted as a proportion of the parent's area.
	Meaningful values are in the range [0...1], unless you intend this element to spill
	outside its parent. */
    //void setRelativeRectProportional(const RectF& r, GeometryType mode=ProportionalGeometry );

    //! Gets the absolute rectangle of this element
    Rect getAbsoluteRect() const;

    //! Returns the visible area of the element.
    Rect getAbsoluteClippingRect() const;

	//! Returns the visible area of the element.
	Rect& getAbsoluteClippingRectRef() const;

    //! Sets whether the element will ignore its parent's clipping rectangle
    /** \param noClip If true, the element will not be clipped by its parent's clipping rectangle. */
    void setNotClipped(bool noClip);

    //! Gets whether the element will ignore its parent's clipping rectangle
    /** \return true if the element is not clipped by its parent's clipping rectangle. */
    bool isNotClipped() const;

    //! Sets the maximum size allowed for this element
    /** If set to 0,0, there is no maximum size */
    void setMaxSize( const Size& size);

    Size getMaxSize() const;

    Size getMinSize() const;

    //! Sets the minimum size allowed for this element
    void setMinSize( const Size& size);

    //! The alignment defines how the borders of this element will be positioned when the parent element is resized.
    void setAlignment(TypeAlign left, TypeAlign right, TypeAlign top, TypeAlign bottom);

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

    //! Sets another color for the text.
    /** If set, the static text does not use the EGDC_BUTTON_TEXT color defined
     *  in the skin, but the set color instead. You don't need to call
     *  INrpLabel::enableOverrrideColor(true) after this, this is done
     *  by this function.
     *  If you set a color, and you want the text displayed with the color
     *  of the skin again, call IGUIStaticText::enableOverrideColor(false);
     *  \param color: New color of the text.
     *  \param na: index of overriding color
	 */
    //void setColor( const Color& color, u32 nA=0 );

    //! Returns an override color
    //Color getColor( u32 index=0 ) const;

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
    bool getNextWidget( int startOrder, bool reverse, bool group,
                        Widget*& first, Widget*& closest, bool includeInvisible=false) const;

protected:

	/*!
	 * This event handler can be reimplemented in a subclass to receive
	 * widget resize events which are passed in the \a event parameter.
	 * When resizeEvent_() is called, the widget already has its new
	 * geometry. 
	 */
	virtual void resizeEvent_();

protected:
    // not virtual because needed in constructor
    void addChild_(Widget* child);

	//FontsMap& getFonts_();

    // not virtual because needed in constructor
    void recalculateAbsolutePosition(bool recursive);	

    std::auto_ptr< WidgetPrivate > _d;

    //! maximum and minimum size of the element
    Size _maxSize, _minSize;

    //! is visible?
    bool _isVisible;

    //! is enabled?
    bool _isEnabled;

    //! is a part of a larger whole and should not be serialized?
    bool _isSubElement;

    //! does this element ignore its parent's clipping rectangle?
    bool _noClip;

    //! caption
    std::string _text;

	//! tooltip
    std::string _toolTipText;

    //! id
    int _id;

	//! tab stop like in windows
    bool _isTabStop;

    //! tab order
    int _tabOrder;

    //! tab groups are containers like windows, use ctrl+tab to navigate
    bool _isTabGroup;

    //! tells the element how to act when its parent is resized
    TypeAlign _alignLeft, _alignRight, _alignTop, _alignBottom;

    //! GUI Environment
    GuiEnv* _environment;
       
    Widget* _eventHandler;
};

enum ElementState
{
  stNormal=0, 
  stPressed, 
  stHovered, 
  stDisabled, 
  stChecked,
  StateCount
};

#endif
