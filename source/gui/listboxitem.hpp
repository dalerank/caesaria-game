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
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>..


#ifndef _CAESARIA_LISTBOXITEM_H_INCLUDE_
#define _CAESARIA_LISTBOXITEM_H_INCLUDE_

#include "widget.hpp"
#include "core/alignment.hpp"
#include "core/scopedptr.hpp"
#include "gfx/picture.hpp"

#include <string>

namespace gui
{

class ListBoxItem
{
public:
  enum ColorType
  {
    //! Color of text
    LBC_TEXT=0,
    //! Color of selected text
    LBC_TEXT_HIGHLIGHT,
    //! Color of icon
    LBC_ICON,
    //! Color of selected icon
    LBC_ICON_HIGHLIGHT,
    //! Not used, just counts the number of available colors
    LBC_BACKGROUND,

    LBC_ALL,

    LBC_COUNT
  };

  ListBoxItem();
  ListBoxItem( const ListBoxItem& other );

	ListBoxItem& operator=( const ListBoxItem& other );
  ~ListBoxItem();

  void setText( const std::string& text );
  const std::string& text() const;
	void setTag( int tag );
	int tag() const;
	gfx::Picture icon() const;
	void setIcon( gfx::Picture pic );

  bool isEnabled() const;
  void setEnabled( bool en );
  // A multicolor extension

  const Alignment& horizontalAlign() const;
  const Alignment& verticalAlign() const;

  bool isAlignEnabled() const;

  float currentHovered() const;
  void updateHovered( float delta );

  ElementState state() const;
  void setState( const ElementState& state );

  Point offset() const;
  void setOffset( Point p );

  Variant data() const;
  void setData( const Variant& value );

  struct ListItemOverrideColor
  {
    ListItemOverrideColor() : Use(false), color( 0 ) {}
    bool Use;
    Font font;
    int /*Color*/ color;
  };

	//!sets the item text alignment
	void setTextAlignment( Alignment horizontal, Alignment vertical );

  ListItemOverrideColor OverrideColors[ LBC_COUNT ];

	static ListBoxItem& getInvalidItem(); 

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif // _CAESARIA_LISTBOXITEM_H_INCLUDE_
