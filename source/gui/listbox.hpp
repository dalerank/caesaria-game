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

#ifndef __CAESARIA_GUI_LISTBOX_H_INCLUDED__
#define __CAESARIA_GUI_LISTBOX_H_INCLUDED__

#include "gui/widget.hpp"
#include "core/flagholder.hpp"
#include "listboxitem.hpp"
#include "core/predefinitions.hpp"
#include "core/signals.hpp"
#include "core/stringarray.hpp"
#include "core/font.hpp"

namespace gui
{

typedef enum { selectOnMove=0, selectOnMDown,
               drawBackground, autoscroll, moveOverSelect,
               hightlightNotinfocused, count } Flag;

class ScrollBar;

class ListBox : public Widget, public FlagHolder<Flag>
{
public:
  typedef enum { selectOnMouseMove=true, selectOnClick=false } SelectMode;

  //! constructor
  ListBox(Widget* parent,
           const Rect& rectangle=Rect( 0, 0, 1, 1), int id=-1, bool clip=true,
           bool drawBack=false, bool mos=false);

  //! destructor
  virtual ~ListBox();

  //! returns amount of list items
  virtual unsigned int itemCount() const;

  //! returns string of a list item. the id may be a value from 0 to itemCount-1
  virtual ListBoxItem& item(unsigned int id);

  virtual ListBoxItem& selectedItem();

  //! clears the list
  virtual void clear();

  //! sets the selected item. Set this to -1 if no item should be selected
  virtual void setSelected(int id);

  //! sets the selected item. Set this to -1 if no item should be selected
  virtual void setSelected(const std::string &item);

  //! called if an event happened.
  virtual bool onEvent(const NEvent& event);

  //! draws the element and its children
  virtual void draw( gfx::Engine& painter );

  //! removes an item from the list
  virtual void removeItem(unsigned int id);

  //! get the the id of the item at the given absolute coordinates
  virtual int itemAt( Point pos ) const;

  //! set all item colors of specified type at given index to color
  virtual void setItemOverrideColor( unsigned int index, const int color, 
                                     ListBoxItem::ColorType colorType=ListBoxItem::all );

  //! set whether the listbox should scroll to newly selected items
  virtual void setAutoScrollEnabled(bool scroll);

  //! returns true if automatic scrolling is enabled, false if not.
  virtual bool isAutoScrollEnabled() const;

  //! clear all item colors at index
  virtual void resetItemOverrideColor(unsigned int index);

  //! clear item color at index for given colortype
  virtual void resetItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType);

  //! has the item at index its color overwritten?
  virtual bool hasItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType) const;

  //! return the overwrite color at given item index.
  virtual NColor getItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType) const;

  //! return the default color which is used for the given colorType
  virtual NColor itemDefaultColor( ListBoxItem::ColorType colorType) const;

  //! set default color which will used for the given colorType
  virtual void setItemDefaultColor( ListBoxItem::ColorType colorType, NColor color );

  virtual void setItemFont( Font font );

  //! set the item at the given index
  virtual void setItem( unsigned int index, std::string text);

  //! Insert the item at the given index
  //! Return the index on success or -1 on failure.
  virtual int insertItem(unsigned int index, std::string text);

  //! Swap the items at the given indices
  virtual void swapItems(unsigned int index1, unsigned int index2);

  //! set global itemHeight
  virtual void setItemHeight( int height );

  virtual int itemHeight() const;

  virtual void setItemAlignment( int index, Alignment horizontal, Alignment vertical );

  //! Sets whether to draw the background
  virtual void setDrawBackground(bool draw);

  //! adds an list item with an icon
  //! \param text Text of list entry
  //! \param icon Sprite index of the Icon within the current sprite bank. Set it to -1 if you want no icon
  //! \return
  //! returns the id of the new created item
  virtual ListBoxItem& addItem( const std::string& text, Font font=Font(), const int color=0 );
  virtual ListBoxItem& addItem( gfx::Picture pic );

  virtual void fitText( const std::string& text );

  virtual void addItems( const StringArray& strings );

  virtual Font font() const;

  virtual int selected();

  virtual void beforeDraw( gfx::Engine& painter );
  virtual void refresh();

  virtual void setItemTextOffset(Point p);

  virtual void setupUI(const VariantMap &ui);

signals public:
  Signal1<std::string>& onItemSelectedAgain();
  Signal1<const ListBoxItem&>& onItemSelected();

protected:
  //! Update the position and size of the listbox, and update the scrollbar
  virtual void _resizeEvent();
  virtual void _drawItemIcon(gfx::Engine& painter, ListBoxItem& item, const Point& pos, Rect* clipRect);
  virtual void _drawItemText(gfx::Engine& painter, ListBoxItem& item, const Point& pos, Rect* clipRect);
  virtual void _updateItemText(gfx::Engine& painter, ListBoxItem& item, const Rect& textRect, Font font, const Rect& frameRect );
  Rect _itemsRect();

private:
  void _selectNew(int ypos);
  void _recalculateScrollPos();

  void _indexChanged( unsigned int eventType );
  ElementState _getCurrentItemState( unsigned int index, bool hl );
  Font _getCurrentItemFont( const ListBoxItem& item, bool selected );
  NColor _getCurrentItemColor( const ListBoxItem& item, bool selected );

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //__CAESARIA_GUI_LISTBOX_H_INCLUDED__
