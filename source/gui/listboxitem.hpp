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


#ifndef NRPLISTBOXITEM_H
#define NRPLISTBOXITEM_H

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
  const std::string& getText() const;
	void setTag( int tag );
	int getTag() const;
  int getIcon() const;
  void setIcon( int icon );

  bool isEnabled() const;
  void setEnabled( bool en );
  // A multicolor extension

  const Alignment& getHorizontalAlign() const;
  const Alignment& getVerticalAlign() const;

  bool isAlignEnabled() const;

  float getCurrentHovered() const;
  void updateHovered( float delta );

  ElementState getState() const;
  void setState( const ElementState& state );

  struct ListItemOverrideColor
  {
    ListItemOverrideColor() : Use(false), color( 0 ) {}
    bool Use;
    Font font;
    int /*Color*/ color;
  };

	//!sets the item text alignment
	void setItemTextAlignment( Alignment horizontal, Alignment vertical );

  ListItemOverrideColor OverrideColors[ LBC_COUNT ];

	static ListBoxItem& getInvalidItem(); 

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif // NRPLISTBOXITEM_H
