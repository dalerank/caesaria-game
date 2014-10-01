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

#ifndef __CAESARIA_EDIT_BOX_H_INCLUDED__
#define __CAESARIA_EDIT_BOX_H_INCLUDED__

#include "widget.hpp"
#include "core/color.hpp"
#include "core/signals.hpp"
#include <string>

namespace gui
{

class EditBox : public Widget
{
public:

  EditBox( Widget* parent );

  //! constructor
  EditBox( Widget* parent, const Rect& rectangle, 
           const std::string& text="", const int id=-1, bool border=false );

  //! destructor
  virtual ~EditBox();

  //! Sets another skin independent font.
  virtual void setFont( const Font& font );

  //! Gets the override font (if any)
  /** \return The override font (may be 0) */
  virtual Font font() const;

  //! Get the font which is used right now for drawing
  /** Currently this is the override font when one is set and the
  font of the active skin otherwise */
  virtual Font activeFont();

  //! Sets another color for the text.
  virtual void setOverrideColor(NColor color );

  //! Gets the override color
  virtual NColor overrideColor() const;

  //! Sets if the text should use the overide color or the
  //! color in the gui skin.
  virtual void setEnabledColor( bool enable );

  //! Checks if an override color is enabled
  /** \return true if the override color is enabled, false otherwise */
  virtual bool isOverrideColorEnabled() const;

  virtual void setDrawBackground( bool drawBackground );

  //! Turns the border on or off
  virtual void setDrawBorder( bool border );

  //! Enables or disables word wrap for using the edit box as multiline text editor.
  virtual void setWordWrap( bool enable );

  //! Checks if word wrap is enabled
  //! \return true if word wrap is enabled, false otherwise
  virtual bool isWordwrapEnabled() const;

  //! Enables or disables newlines.
  /** \param enable: If set to true, the EGET_EDITBOX_ENTER event will not be fired,
  instead a newline character will be inserted. */
  virtual void setMultiline( bool enable );

  //! Checks if multi line editing is enabled
  //! \return true if mult-line is enabled, false otherwise
  virtual bool isMultilineEnabled() const;

  virtual void moveCursor( int index );

  //! Enables or disables automatic scrolling with cursor position
  //! \param enable: If set to true, the text will move around with the cursor position
  virtual void setAutoscroll( bool enable );

  //! Checks to see if automatic scrolling is enabled
  //! \return true if automatic scrolling is enabled, false if not
  virtual bool isAutoscrollEnabled() const;

  //! Gets the size area of the text in the edit box
  //! \return Returns the size in pixels of the text
  virtual Size textDimension();

  //! Sets text justification
  virtual void setTextAlignment( Alignment horizontal, Alignment vertical );

  //! called if an event happened.
  virtual bool onEvent( const NEvent& event );

  //! draws the element and its children
  virtual void draw( gfx::Engine& painter );

  //! Sets the new caption of this element.
  virtual void setText(const std::string& text);
  virtual std::string text() const;

  //! Sets the maximum amount of characters which may be entered in the box.
  //! \param max: Maximum amount of characters. If 0, the character amount is
  //! infinity.
  virtual void setMaxCharactersNumber( unsigned int max );

  //! Returns maximum amount of characters, previously set by setMax();
  virtual unsigned int maxCharactersNumber() const;

  //! Sets whether the edit box is a password box. Setting this to true will
  /** disable MultiLine, WordWrap and the ability to copy with ctrl+c or ctrl+x
  \param passwordBox: true to enable password, false to disable
  \param passwordChar: the character that is displayed instead of letters */
  virtual void setPasswordBox( bool passwordBox, char passwordChar = L'*' );

  //! Returns true if the edit box is currently a password box.
  virtual bool isPasswordBox() const;

  //! Writes attributes of the element.
  //virtual void save( VariantArray* out ) const;

  //! Reads attributes of the element
  virtual void setupUI(const VariantMap& ui);

  virtual void beforeDraw( gfx::Engine& painter );

signals public:
  Signal1<std::string>& onTextChanged();
  Signal0<>& onEnterPressed();

protected:
  //! Updates the position, splits text if required
  virtual void _resizeEvent();

  //! Breaks the single text line.
  void _breakText();

  //! returns the line number that the cursor is on
  int _getLineFromPos( int pos );
  //! adds a letter to the edit box
  void _inputChar( unsigned short c );
  //! calculates the current scroll position
  void _calculateScrollPos();

  //! send some gui event to parent
  void _sendGuiEvent( unsigned int type );
  //! set text markers
  void _setTextMarkers( int begin, int end );

  bool _processKey( const NEvent& event );
  bool _processMouse( const NEvent& event );

  void _drawHolderText( Font font, Rect* clip );

  void _init();

  void _setText( const std::wstring& text );

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif // __CAESARIA_EDIT_BOX_H_INCLUDED__

