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

#ifndef __OPENCAESAR3_LIST_BOX_H_INCLUDED__
#define __OPENCAESAR3_LIST_BOX_H_INCLUDED__

#include "oc3_widget.hpp"
#include "oc3_flagholder.hpp"
#include "oc3_listboxitem.hpp"
#include "oc3_predefinitions.hpp"

typedef enum { LBF_SELECT_ON_MOVE=0, LBF_SELECT_ON_MOUSE_DOWN,
               LBF_DRAWBACK, LBF_AUTOSCROLL, LBF_MOVEOVER_SELECT, 
               LBF_HIGHLIGHTWHEN_NOTFOCUSED, LBF_COUNT } LISTBOX_FLAG;

class ScrollBar;
class Font;

class ListBox : public Widget, public FlagHolder<LISTBOX_FLAG>
{
public:
	typedef enum { selectOnMove=true, selectOnClick=false } SelectMode;
	//! constructor
	ListBox( Widget* parent,
		const Rect& rectangle, int id=-1, bool clip=true,
		bool drawBack=false, bool moveOverSelect=false);

	//! destructor
	virtual ~ListBox();

	//! returns amount of list items
	virtual unsigned int getItemCount() const;

	//! returns string of a list item. the id may be a value from 0 to itemCount-1
  virtual ListBoxItem& getItem(unsigned int id);

	//! clears the list
	virtual void clear();

	//! sets the selected item. Set this to -1 if no item should be selected
  virtual void setSelected(int id);

	//! sets the selected item. Set this to -1 if no item should be selected
  virtual void setSelected(const std::string &item);

	//! called if an event happened.
	virtual bool onEvent(const NEvent& event);

	//! draws the element and its children
	virtual void draw( GfxEngine& painter );

	//! removes an item from the list
	virtual void removeItem(unsigned int id);

	//! get the the id of the item at the given absolute coordinates
  virtual int getItemAt( const Point& pos ) const;

  //! set item action when clicked
  virtual void SetItemAction( unsigned int index, int funcRef );

  //! set all item colors of specified type at given index to color
  virtual void setItemOverrideColor( unsigned int index, const int color, 
                                     ListBoxItem::ColorType colorType=ListBoxItem::LBC_ALL );

  //! set whether the listbox should scroll to newly selected items
	virtual void setAutoScrollEnabled(bool scroll);

	//! returns true if automatic scrolling is enabled, false if not.
	virtual bool isAutoScrollEnabled() const;

  //! Writes attributes of the element.
  virtual void save( VariantMap& out ) const;

	//! Reads attributes of the element
  virtual void load( const VariantMap& in );

	//! clear all item colors at index
  virtual void resetItemOverrideColor(unsigned int index);

	//! clear item color at index for given colortype
  virtual void resetItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType);

	//! has the item at index its color overwritten?
	virtual bool hasItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType) const;

	//! return the overwrite color at given item index.
	virtual int getItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType) const;

	//! return the default color which is used for the given colorType
	virtual int getItemDefaultColor( ListBoxItem::ColorType colorType) const;

	//! set the item at the given index
  virtual void setItem( unsigned int index, const std::string &text, int icon);

	//! Insert the item at the given index
	//! Return the index on success or -1 on failure.
  virtual int insertItem(unsigned int index, const std::string &text, int icon);

	//! Swap the items at the given indices
	virtual void swapItems(unsigned int index1, unsigned int index2);

	//! set global itemHeight
	virtual void setItemHeight( int height );

  //! Sets whether to draw the background
  virtual void setDrawBackground(bool draw);

    //! adds an list item with an icon
    //! \param text Text of list entry
    //! \param icon Sprite index of the Icon within the current sprite bank. Set it to -1 if you want no icon
    //! \return
    //! returns the id of the new created item
  virtual ListBoxItem& addItem( const std::string& text, const Font& font, const int color );

  virtual int getSelected();

  void beforeDraw( GfxEngine& painter );

protected:
	//! Update the position and size of the listbox, and update the scrollbar
	virtual void resizeEvent_();

private:
    void _SelectNew(int ypos);
    void _RecalculateScrollPos();

  void _IndexChanged( unsigned int eventType );
  ElementState _GetCurrentItemState( unsigned int index, bool hl );
  //Font _GetCurrentItemFont( const ListBoxItem& item, bool selected, ElementStyle* itemStyle );
  //Color _GetCurrentItemColor( const ListBoxItem& item, bool selected, ElementStyle* itemStyle );
  void _DrawItemIcon( const ListBoxItem& item, const Rect& rectangle, bool highlighted, bool selected, Rect* clip, const int color );
  Rect getItemTextRect_();
  void needUpdateTexture4Text_( unsigned int index );
  Font _GetCurrentItemFont( const ListBoxItem& item, bool selected );
  int _GetCurrentItemColor( const ListBoxItem& item, bool selected );
  void _updateTexture();
  class Impl;
	ScopedPtr< Impl > _d;
};


#endif
