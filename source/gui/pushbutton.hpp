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

#ifndef __CAESARIA_PUSHBUTTON_H_INCLUDE_
#define __CAESARIA_PUSHBUTTON_H_INCLUDE_

#include "widget.hpp"
#include "core/signals.hpp"
#include "gfx/picture.hpp"

class Font;
namespace gui
{

//Usual button class
class PushButton : public Widget
{
public:
  typedef enum { greyBorderLine, greyBorderLineSmall, whiteBorderUp,
                 blackBorderUp, noBackground, greyBorderLineBig,
                 flatBorderLine, greyBorderLineFit } BackgroundStyle;

  PushButton( Widget* );

  PushButton(Widget* parent,
              const RectF& rectangle,
              const std::string& caption="",
              int id=-1,
              bool noclip=false,
              const BackgroundStyle bgstyle=greyBorderLine);

  //! constructor
  PushButton( Widget* parent,
              const Rect& rectangle,
              const std::string& caption="",
              int id=-1,
              bool noclip=false,
              const BackgroundStyle bgtyle=greyBorderLine);

  //! destructor
  virtual ~PushButton();

  //! called if an event happened.
  virtual bool onEvent(const NEvent& event);

  //! prepare render state
  virtual void beforeDraw( gfx::Engine& painter );

  //! override render function
  virtual void draw( gfx::Engine& painter );

  virtual void debugDraw(gfx::Engine &painter);

  virtual void setText(const std::string& text );

  virtual void setPressed( bool pressed );
  virtual bool isPressed() const;

  virtual bool isBodyVisible() const;
  virtual bool isTextVisible() const;
  virtual void setTextVisible( bool value );

  virtual void drawIcon( gfx::Engine& painter );

  virtual void setPicture( const std::string& rcname, int index);
  virtual void setPicture( gfx::Picture picture, ElementState state );
  virtual void setPicture( const std::string& rcname, int index, ElementState state );

  virtual void setIcon( const std::string& rcname, int index, ElementState state );
  virtual void setIcon( const std::string& rcname, int index );
  virtual void setIcon( gfx::Picture pic );
  virtual void setIconMask(int mask);
  virtual void setIconOffset( Point offset );

  virtual void setBackgroundStyle( const BackgroundStyle style );
  virtual void setBackgroundStyle( const std::string& style );

  virtual void setFont( const Font& font, ElementState state );
  virtual void setFont( const Font& font );
  virtual void setFont( const std::string& fname, NColor color=NColor() );

  virtual Font font( ElementState state ) const;

  virtual bool isPushButton() const;
  virtual void setIsPushButton( bool value );

  virtual void setupUI(const VariantMap &ui);
  virtual void setupUI(const vfs::Path &ui);

  virtual void setTextOffset( const Point& offset );
  virtual void canvasDraw(const std::string &text, const Point &point, Font font=Font(), NColor color=NColor());

signals public:
  virtual Signal0<>& onClicked();
  virtual Signal1<Widget*>& onClickedEx();

protected:
  virtual void _finalizeResize();

  //! when left mouse button pressed down
  virtual bool _leftMouseBtnPressed( const NEvent& event );

  //! when left mouse button left up
  virtual bool _btnMouseUp( const NEvent& event );

  //! when some mouse button clicked
  virtual void _btnClicked();

  virtual ElementState _state();
  virtual void _updateBackground( ElementState state );
  virtual void _updateTexture();

  gfx::Picture& _textPicture();
  void _updateStyle();
private:
  __DECLARE_IMPL(PushButton)
};

}//end namespace gui
#endif //__CAESARIA_PUSHBUTTON_H_INCLUDE_
