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

#ifndef __CAESARIA_GUILABEL_H_INCLUDE_
#define __CAESARIA_GUILABEL_H_INCLUDE_

#include "widget.hpp"
#include "gfx/picturesarray.hpp"
#include "core/signals.hpp"

namespace gui
{

class Label : public Widget
{
public:
  typedef enum { bgWhite=0, bgBlack, bgBrown, bgSmBrown, bgNone,
                 bgWhiteFrame, bgBlackFrame,
                 bgWhiteBorderA, bgSimpleWhite, bgSimpleBlack } BackgroundMode;
  //! constructor
  Label( Widget* parent );

  Label( Widget* parent, const Rect& rectangle, const std::string& text="", bool border=false,
         BackgroundMode background = bgNone, int id=-1);

  //! destructor
  virtual ~Label();

  //! draws the element and its children
  virtual void draw( gfx::Engine& painter );

  virtual void setPadding( const Rect& margin );

  //! Get the font which is used right now for drawing
  virtual Font font() const;

  //! Sets whether to draw the background
  virtual void setBackgroundMode( BackgroundMode mode );

  virtual void beforeDraw( gfx::Engine& painter );

  //! Return background draw
  virtual BackgroundMode backgroundMode() const;

  virtual bool onEvent(const NEvent &event);

  virtual bool isBorderVisible() const;

  //! Sets whether to draw the border
  virtual void setBorderVisible(bool draw);

  //! Set whether the text in this label should be clipped if it goes outside bounds
  virtual void setTextRestrainedInside(bool restrainedInside);

  //! Checks if the text in this label should be clipped if it goes outside bounds
  virtual bool isTextRestrainedInside() const;

  //! Enables or disables word wrap for using the static text as
  //! multiline text control.
  virtual void setWordwrap(bool enable);

  //! Checks if word wrap is enabled
  virtual bool isWordWrapEnabled() const;

  //! Sets the new caption of this element.
  virtual void setText(const std::string& text);

  //! Returns the height of the text in pixels when it is drawn.
  virtual int textHeight() const;

  //! Returns the width of the current text, in the current font
  virtual int textWidth() const;

  //! Set whether the string should be interpreted as right-to-left (RTL) text
  /** \note This component does not implement the Unicode bidi standard, the
  text of the component should be already RTL if you call this. The
  main difference when RTL is enabled is that the linebreaks for multiline
  elements are performed starting from the end.
  */
  virtual void setRightToLeft(bool rtl);

  //! Checks if the text should be interpreted as right-to-left text
  virtual bool isRightToLeft() const;

  virtual void setPrefixText( const std::string& prefix );

  virtual void setBackgroundPicture( const gfx::Picture& picture, Point offset=Point() );

  virtual void setIcon( const gfx::Picture& icon, Point offset=Point() );

  virtual void setFont( const Font& font );

  virtual void setAlpha( unsigned int value );

  virtual void setColor( NColor color );

  virtual void setTextAlignment( Alignment horizontal, Alignment vertical );

  virtual void setLineIntervalOffset( const int offset );

  virtual void setupUI( const VariantMap &ui );

  virtual void setTextOffset( Point offset );
    
signals public:
  virtual Signal0<>& onClicked();

protected:
  virtual void _resizeEvent();
  virtual void _updateTexture( gfx::Engine& painter );
  virtual void _updateBackground(gfx::Engine& painter , bool& useAlpha4Text);
  virtual void _handleClick();

  gfx::PictureRef& _textPictureRef();
  gfx::Pictures& _backgroundRef();

private:

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui

#endif//__CAESARIA_GUILABEL_H_INCLUDE_
