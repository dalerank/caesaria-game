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

namespace gfx { class Batch; }

namespace gui
{

class Label : public Widget
{
public:
  typedef enum { bgWhite=0, bgBlack, bgBrown, bgSmBrown, bgNone,
                 bgWhiteFrame, bgBlackFrame,
                 bgWhiteBorderA, bgSimpleWhite, bgSimpleBlack } BackgroundStyle;
  //! constructor
  Label(Widget* parent);

  Label(Widget* parent, const Rect& rectangle, const std::string& text="", bool border=false,
        BackgroundStyle background = bgNone, int id=-1);

  Label(Widget* parent, const Rect& rectangle, const std::string& text, Font font);

  //! destructor
  virtual ~Label();

  //! draws the element and its children
  virtual void draw(gfx::Engine& painter);

  void setPadding(const Rect& margin);

  //! Get the font which is used right now for drawing
  virtual Font font() const;

  //! Sets whether to draw the background
  void setBackgroundStyle(BackgroundStyle style);

  //!
  void setBackgroundStyle(const std::string& style);

  virtual void beforeDraw( gfx::Engine& painter );

  //! Return background draw
  virtual BackgroundStyle backgroundStyle() const;

  //!
  virtual bool onEvent(const NEvent &event);

  //!
  bool isBorderVisible() const;

  //!
  virtual void canvasDraw(const std::string& text, const Point& point=Point(), Font font=Font(), NColor color=0);

  //!
  virtual void canvasDraw(const std::string& text, const Rect& rect, Font font=Font(), NColor color=0,
                          Alignment halign=align::automatic, Alignment valign=align::automatic );

  //!
  virtual void canvasDraw(const gfx::Picture& picture, const Point& point);

  //! Sets whether to draw the border
  void setBorderVisible(bool draw);

  //! Set whether the text in this label should be clipped if it goes outside bounds
  void setTextRestrainedInside(bool restrainedInside);

  //! Checks if the text in this label should be clipped if it goes outside bounds
  bool isTextRestrainedInside() const;

  //! Enables or disables word wrap for using the static text as
  //! multiline text control.
  virtual void setWordwrap(bool enable);

  //! Checks if word wrap is enabled
  virtual bool isWordwrapEnabled() const;

  //! Sets the new caption of this element.
  virtual void setText(const std::string& text);

  //! Returns the height of the text in pixels when it is drawn.
  int textHeight() const;

  //! Returns the width of the current text, in the current font
  int textWidth() const;

  //! Set whether the string should be interpreted as right-to-left (RTL) text
  /** \note This component does not implement the Unicode bidi standard, the
  text of the component should be already RTL if you call this. The
  main difference when RTL is enabled is that the linebreaks for multiline
  elements are performed starting from the end.
  */
  void setRightToLeft(bool rtl);

  //! Checks if the text should be interpreted as right-to-left text
  bool isRightToLeft() const;

  void setPrefixText(const std::string& prefix);

  virtual void setBackgroundPicture(const gfx::Picture& picture, Point offset=Point());
  
  virtual void setBackgroundPicture(const std::string& rcname);

  virtual void setIcon(const gfx::Picture& icon, Point offset=Point());

  //!
  virtual void setIcon(const std::string& rc, int index);

  virtual void setIconOffset( const Point& offset );

  virtual void setFont(const Font& font);

  //!
  virtual void setFont( const std::string& fontname );

  //!
  virtual void setFont(FontType type, NColor color=0);

  virtual void setAlpha(unsigned int value);

  virtual void setColor(NColor color);

  //!
  virtual void setColor(const std::string& color);

  virtual void setTextAlignment( Alignment horizontal, Alignment vertical );

  //!
  virtual void setTextAlignment( const std::string& horizontal, const std::string& vertical );

  virtual void setLineIntervalOffset( const int offset );

  virtual void setupUI(const VariantMap &ui);

  virtual void setTextOffset(Point offset);

  virtual void setTextOffset(int x, int y);

signals public:
  virtual Signal0<>& onClicked();
  virtual Signal1<Widget*>& onClickedA();

protected:
  virtual void _finalizeResize();
  virtual void _updateTexture( gfx::Engine& painter );
  virtual void _updateBackground(gfx::Engine& painter , bool& useAlpha4Text);
  virtual void _handleClick();

  gfx::Picture& _textPicture();
  gfx::Batch& _background();
  gfx::Pictures& _backgroundNb();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui

#endif//__CAESARIA_GUILABEL_H_INCLUDE_
