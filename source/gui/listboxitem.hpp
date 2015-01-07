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
    simple=0,
    //! Color of selected text
    hovered,
    //! Color of icon
    iconSimple,
    //! Color of selected icon
    iconHovered,
    //! Not used, just counts the number of available colors
    background,

    all,

    count
  };

  ListBoxItem();
  ListBoxItem( const ListBoxItem& other );

	ListBoxItem& operator=( const ListBoxItem& other );
  ~ListBoxItem();

  void setText( const std::string& text );
  const std::string& text() const;
	void setTag( int tag );
	int tag() const;
	gfx::Picture icon();
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

  Point textOffset() const;
  void setTextOffset( Point p );

  Point iconOffset() const;
  void setIconOffset( Point p );

  const gfx::Picture& picture() const;

  void setUrl( const std::string& url );
  const std::string& url() const;

  Variant data() const;
  void setData( const Variant& value );

  void setTextColor( ColorType type, NColor color);

  void updateText( const Point& p, Font f, const Size& s );
  void draw(const std::string& text, Font f , const Point& p);
  void clear();

  struct OverrideColor
  {
    OverrideColor() : Use(false), color( 0 ) {}
    bool Use;
    Font font;
    NColor color;
  };

	//!sets the item text alignment
	void setTextAlignment( Alignment horizontal, Alignment vertical );

	OverrideColor OverrideColors[ count ];

  static ListBoxItem& invalidItem();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif // _CAESARIA_LISTBOXITEM_H_INCLUDE_
