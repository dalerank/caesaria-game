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

#include "oc3_alignment.hpp"
#include "oc3_gui_widget.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_picture.hpp"

#include <string>

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

  const TypeAlign& getHorizontalAlign() const;
  const TypeAlign& getVerticalAlign() const;

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
	void setItemTextAlignment( TypeAlign horizontal, TypeAlign vertical );

  ListItemOverrideColor OverrideColors[ LBC_COUNT ];

	static ListBoxItem& getInvalidItem(); 

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif // NRPLISTBOXITEM_H
