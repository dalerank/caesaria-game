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

#ifndef _CAESARIA_WINDOW_H_INCLUDE_
#define _CAESARIA_WINDOW_H_INCLUDE_

#include "widget.hpp"
#include "pushbutton.hpp"

namespace gui
{

class Window : public Widget
{
public:
  typedef enum { buttonClose=0, buttonMin, buttonMax, buttonCount } ButtonName;
  typedef enum { fdraggable=0x1, fbackgroundVisible=0x2, ftitleVisible=0x4 } FlagName;
  typedef enum { bgNone, bgWhiteFrame } BackgroundType;
  //! constructor
  Window( Widget* parent, const Rect& rectangle, const std::string& title, int id=-1, BackgroundType style=bgWhiteFrame );

  //! destructor
  virtual ~Window();

  //! called if an event happened.
  virtual bool onEvent(const NEvent& event);

  //! draws the element and its children
  virtual void draw( gfx::Engine& painter );

  //! Returns pointer to the close button
  virtual PushButton* button( ButtonName btn ) const;

  //!
  virtual void beforeDraw(gfx::Engine &painter);

  //! Set if the window background will be drawn
  virtual void setBackgroundVisible(bool draw);

  //! Get if the window background will be drawn
  virtual bool backgroundVisible() const;

  //! Set if the window titlebar will be drawn
  //! Note: If the background is not drawn, then the titlebar is automatically also not drawn
  virtual void setTitleVisible(bool draw);

  //! Get if the window titlebar will be drawn
  virtual bool titleVisible() const;

  virtual void setBackground( gfx::Picture texture );
  virtual void setBackground( BackgroundType type );

  virtual gfx::Picture background() const;

  virtual Rect clientRect() const;
  virtual void setModal();

  virtual void setWindowFlag( FlagName flag, bool enabled=true );

  virtual void setupUI(const VariantMap &ui);

  virtual void setupUI(const vfs::Path& path );

  virtual void setTextAlignment( Alignment horizontal, Alignment vertical );

  virtual void setText( const std::string& text );
  virtual void setTitleRect( const Rect& rect );

protected:
  void _createSystemButton( ButtonName btnName, const std::string& tooltip, bool visible );
  void _init();
  virtual void _finalizeResize();
  virtual void _updateBackground();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

class SimpleWindow : public Window
{
public:
  SimpleWindow(Widget* parent, const Rect& rect, const std::string& title="", const std::string& ui="");
};

}//end namespace gui

#endif //_CAESARIA_WINDOW_H_INCLUDE_
