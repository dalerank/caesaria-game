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

#ifndef __OPENCAESAR3_CONTEXT_MENU_H_INCLUDED__
#define __OPENCAESAR3_CONTEXT_MENU_H_INCLUDED__

#include "widget.hpp"
#include "core/signals.hpp"
#include "core/scopedptr.hpp"

class VariantMap;

namespace gui
{

class ContextMenuItem;

//! GUI Context menu interface.
class ContextMenu : public Widget
{
public:
  typedef enum
  {
      //! do nothing - menu stays open
      cmIgnore = 0,

      //! remove the gui element
      cmRemove = 1,

      //! call setVisible(false)
      cmHide = 2

      // note to implementors - this is planned as bitset, so continue with 4 if you need to add further flags.
  } CloseMode;

  //! constructor
  ContextMenu( Widget* parent, const Rect& rectangle, int id=-1,
                     bool getFocus = true, bool allowFocus = true);

  //! destructor
  virtual ~ContextMenu();

  //! set behaviour when menus are closed
  virtual void setCloseHandling( CloseMode onClose );

  //! get current behaviour when the menue will be closed
  virtual CloseMode getCloseHandling() const;

  //! Returns amount of menu items
  virtual unsigned int itemCount() const;

  virtual ContextMenuItem* addItem( const std::string& path, const std::string& text, int commandid,
                                    bool enabled=true, bool hasSubMenu=false,
                                    bool checked=false, bool autoChecking=false);

	//! Adds a menu item.
  virtual ContextMenuItem* addItem( const std::string& text, int commandid,
      		                          bool enabled=true, bool hasSubMenu=false, 
                                    bool checked=false, bool autoChecking=false);

  //! Insert a menu item at specified position.
  virtual ContextMenuItem* insertItem( unsigned int idx, const std::string& text, int commandId, bool enabled,
		                                   bool hasSubMenu, bool checked, bool autoChecking);

  //! Find a item which has the given CommandId starting from given index
  virtual ContextMenuItem* findItem( int commandId, unsigned int idxStartSearch ) const;

  virtual ContextMenuItem* findItem( const std::string& name ) const;

  //! Adds a separator item to the menu
  virtual void addSeparator();

  //! Returns text of the menu item.
  virtual ContextMenuItem* item( unsigned int idx ) const;

  //! Sets text of the menu item.
  virtual void updateItems();

  //! Removes a menu item
  virtual void removeItem( unsigned int idx );

  //! Removes all menu items
  virtual void removeAllItems();

  //! called if an event happened.
  virtual bool onEvent(const NEvent& event);

  //! draws the element and its children
  virtual void draw( gfx::Engine& painter );

  //! Returns the selected item in the menu
  virtual int selected() const;

  virtual ContextMenuItem* selectedItem() const;

  //! Sets the visible state of this element.
  virtual void setVisible( bool visible );

  //! When an eventparent is set it receives events instead of the usual parent element
  virtual void setEventParent( Widget *parent );

  //! Writes attributes of the element.
  virtual void save( VariantMap& out ) const;

  //! Reads attributes of the element
  virtual void load( const VariantMap& in );

  virtual void setAllowFocus( bool enabled );

  void beforeDraw( gfx::Engine& painter );

  float getOpacity( unsigned int index=0 ) const;

  int hovered() const;

signals public:
  virtual Signal1<int>& onItemAction();
	
protected:
  void _setHovered( int index );

  void _closeAllSubMenus();
  bool _hasOpenSubMenu() const;

  virtual void _recalculateSize();

  //! returns true, if an element was highlighted
  virtual bool _isHighlighted(const Point& p, bool canOpenSubMenu);

  //! sends a click Returns:
  //! 0 if click went outside of the element,
  //! 1 if a valid button was clicked,
  //! 2 if a nonclickable element was clicked
  virtual unsigned int _sendClick(const Point& p);

  void setItemVisible( unsigned int index, bool visible );

  class Impl;
	ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif // __OPENCAESAR3_CONTEXT_MENU_H_INCLUDED__

