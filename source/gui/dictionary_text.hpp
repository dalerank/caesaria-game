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

#ifndef __CAESARIA_DICTIONARY_TEXT_H_INCLUDE_
#define __CAESARIA_DICTIONARY_TEXT_H_INCLUDE_

#include "widget.hpp"
#include "gfx/picturesarray.hpp"
#include "core/signals.hpp"

namespace gui
{

class DictionaryText : public Widget
{
public:
  //! constructor
  DictionaryText( Widget* parent );

  DictionaryText( Widget* parent, const Rect& rectangle, const std::string& text="", bool border=false, int id=-1);

  //! destructor
  virtual ~DictionaryText();

  //! draws the element and its children
  virtual void draw( gfx::Engine& painter );

  virtual void setPadding( const Rect& margin );

  //! Get the font which is used right now for drawing
  virtual Font font() const;

  virtual void beforeDraw( gfx::Engine& painter );

  virtual bool onEvent(const NEvent &event);

  virtual bool isBorderVisible() const;

  //! Sets whether to draw the border
  virtual void setBorderVisible(bool draw);

  //! Set whether the text in this label should be clipped if it goes outside bounds
  virtual void setTextRestrainedInside(bool restrainedInside);

  //! Checks if the text in this label should be clipped if it goes outside bounds
  virtual bool isTextRestrainedInside() const;

  //! Sets the new caption of this element.
  virtual void setText(const std::string& text);

  //! Returns the height of the text in pixels when it is drawn.
  virtual int textHeight() const;

  //! Returns the width of the current text, in the current font
  virtual int textWidth() const;

  virtual void setBackgroundPicture( const gfx::Picture& picture, Point offset=Point() );

  virtual void setFont( const Font& font );

  virtual void setAlpha( unsigned int value );

  virtual void setTextAlignment( Alignment horizontal, Alignment vertical );

  virtual void setLineIntervalOffset( const int offset );

  virtual void setupUI( const VariantMap &ui );

  virtual void setTextOffset( Point offset );
    
signals public:
  Signal1<std::string>& onWordClick();

protected:
  virtual void _resizeEvent();
  virtual void _updateTexture( gfx::Engine& painter );
  virtual void _handleClick(const Point& p);

  gfx::PictureRef& _textPictureRef();
  gfx::Pictures& _backgroundRef();

private:
  void _init();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui

#endif//__CAESARIA_DICTIONARY_TEXT_H_INCLUDE_
