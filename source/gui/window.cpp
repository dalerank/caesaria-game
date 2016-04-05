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

#include "window.hpp"
#include "label.hpp"
#include "core/logger.hpp"
#include "gfx/engine.hpp"
#include "modal_widget.hpp"
#include "texturedbutton.hpp"
#include "widgetescapecloser.hpp"

#include "gfx/decorator.hpp"
#include "gfx/drawstate.hpp"
#include "gfx/picturesarray.hpp"
#include "widget_factory.hpp"
#include <GameVfs>
#include <GameCore>

using namespace gfx;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(Window)

class WindowBackgroundHelper : public EnumsHelper<Window::BackgroundType>
{
public:
  WindowBackgroundHelper()
    : EnumsHelper<Window::BackgroundType>(Window::bgNone)
  {
    append( Window::bgWhiteFrame, "whiteFrame" );
    append( Window::bgNone, "none" );
  }
};

class BatchState
{
public:
  Batch body;
  Pictures fallback;

  void reset() { fallback.clear(); }

  void fill( const Rect& area, const Point& lefttop, Decorator::Mode style, bool negativeY )
  {
    bool errorsOnBatch = false;
    body.destroy();
    Decorator::draw( fallback, area, style, nullptr, negativeY  );
    errorsOnBatch = !body.load( fallback, lefttop );

    if( errorsOnBatch )
    {
      Decorator::reverseYoffset( fallback );
      body.destroy();
    }
    else
      fallback.clear();
  }
};

class Window::Impl
{
public:
  std::vector< PushButton* > buttons;
  Rect headerRect, captionRect;

  Label* title;

  struct
  {
    BatchState batch;

    Window::BackgroundType type;
    Picture image;
    bool dirty;

    void set(Window::BackgroundType t)
    {
      image = Picture::getInvalid();
      type  = t;
      dirty = true;
    }
  } background;

  struct {
    bool active;
    Point startPosition;
  } drag;

  struct {
    NColor caption;
    NColor current;
  } colors;

  struct {
    Signal1<Widget*> onCloseEx;
    Signal2<Widget*, NEvent> onKeyPressed;
  } signal;

  FlagHolder<Window::FlagName> flags;
};

//! constructor
Window::Window(Widget* parent)
  : Window( parent, Rect( 0, 0, 1, 1), "" )
{

}

Window::Window( Widget* parent, const Rect& rectangle, const std::string& title, int id, BackgroundType type )
  : Widget( parent, id, rectangle ),
    _d( new Impl )
{
  setWindowFlag( fdraggable, true );
  setWindowFlag( fbackgroundVisible, true );
  setWindowFlag( ftitleVisible, true );
#ifdef _DEBUG
  setDebugName( "Window");
#endif
  _d->title = nullptr;
  _d->background.image = Picture::getInvalid();
  _d->drag.active = false;
  _d->buttons.resize( buttonCount, nullptr );

  _init();

  // this element is a tab group
  setBackground( type );
  setTabgroup( true );
  setTabstop( true );
  setTaborder(-1);
  setText( title );
}

Signal1<Widget*>& Window::onCloseEx() { return _d->signal.onCloseEx; }

void Window::setText(const std::string& text )
{
  Widget::setText( text );
  if( _d->title )
    _d->title->setText( text );
}

void Window::setTitleRect(const Rect& rect)
{
  if( _d->title )
    _d->title->setGeometry( rect );
}

void Window::addCloseCode(int code)
{
  auto list = findChildren<WidgetClosers*>();
  WidgetClosers* closers = nullptr;
  if (list.empty())
    closers = &add<WidgetClosers>();
  else
    closers = list.front();

  if (closers != nullptr)
  {
    closers->addCloseCode(code);
    auto modalScreen = safety_cast<ModalScreen*>(parent());
    if (modalScreen)
      modalScreen->installEventHandler(closers);
  }
}

Signal2<Widget*, NEvent>& Window::onKeyPressedEx()
{
  return _d->signal.onKeyPressed;
}

void Window::_createSystemButton( ButtonName btnName, const std::string& tooltip, bool visible )
{
  PushButton*& btn = _d->buttons[ btnName ];
  if( !btn )
  {
    btn = &add<PushButton>( Rect( 0, 0, 10,10 ) );
    btn->setTooltipText( tooltip );
    btn->setVisible(visible);
    btn->setSubElement(true);
    btn->setTabstop(false);
    btn->setAlignment(align::lowerRight, align::lowerRight, align::upperLeft, align::upperLeft);
  }
}

void Window::_init()
{
  _createSystemButton( buttonClose, "Close",   false );
  _createSystemButton( buttonMin,   "Min",     false );
  _createSystemButton( buttonMax,   "Restore", false );

  if( !_d->title )
  {
    _d->title = &add<Label>( Rect( 15, 15, width()-15, 15+24 ), text(), false );
    _d->title->setTextAlignment( align::center, align::center );
    _d->title->setFont( "FONT_4" );
    _d->title->setSubElement( true );
  }

  _d->title->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::upperLeft );
}

void Window::_setSystemButtonsVisible(bool visible)
{
  button(buttonClose)->setVisible(visible);
  button(buttonMin)->setVisible(visible);
  button(buttonMax)->setVisible(visible);
}

void Window::_finalizeResize()
{
  Widget::_finalizeResize();
  if( _d->background.type != bgNone  )
  {
    setBackground( _d->background.type );
  }
}

void Window::_updateBackground()
{
  _d->background.batch.reset();
  switch( _d->background.type )
  {
  case bgWhiteFrame:
  {
    _d->background.batch.fill( Rect( 0, 0, width(), height()),
                               absoluteRect().lefttop(),
                               Decorator::whiteFrame, Decorator::normalY );
  }
  break;

  default: break;
  }
}

Widget* Window::_titleWidget() const { return _d->title; }

Window::~Window()
{
  emit _d->signal.onCloseEx(this);
  Logger::info( "Window ID={} was removed", ID() );
}

//! called if an event happened.
bool Window::onEvent(const NEvent& event)
{
  if( enabled() )
  {
    switch(event.EventType)
    {
    case sEventGui:
      if (event.gui.type == guiElementFocusLost)
      {
        _d->drag.active = false;
      }
      else if (event.gui.type == guiElementFocused)
      {
          if( ((event.gui.caller == this) || isMyChild(event.gui.caller)))
            bringToFront();
      }
      else if (event.gui.type == guiButtonClicked)
      {
        if (event.gui.caller == _d->buttons[ buttonClose ] )
        {
            // send close event to parent
            // if the event was not absorbed
            if( !parent()->onEvent( NEvent::ev_gui( this, 0, guiElementClosed ) ) )
                deleteLater();
            return true;
        }
      }
    break;

    case sEventMouse:
      switch(event.mouse.type)
      {
      case NEvent::Mouse::btnLeftPressed:
        _d->drag.startPosition = event.mouse.pos();
        _d->drag.active = _d->flags.isFlag( fdraggable );
        bringToFront();

      return true;

      case NEvent::Mouse::mouseRbtnRelease:
      case NEvent::Mouse::mouseLbtnRelease:
      {
        if( _d->drag.active )
        {
          _d->drag.active = false;
          return true;
        }
      }
      break;

      case NEvent::Mouse::moved:
        if ( !event.mouse.isLeftPressed() )
          _d->drag.active = false;

        if (_d->drag.active)
        {
          // gui window should not be dragged outside its parent
          const Rect& parentRect = parent()->absoluteRect();
          if( (event.mouse.x < parentRect.left() +1 ||
            event.mouse.y < parentRect.top() +1 ||
            event.mouse.x > parentRect.right() -1 ||
            event.mouse.y > parentRect.bottom() -1))
            return true;

          move( event.mouse.pos() - _d->drag.startPosition );
          _d->drag.startPosition = event.mouse.pos();

          return true;
        }
      break;

      default:
      break;
      }
    break;

    case sEventKeyboard:
      emit _d->signal.onKeyPressed(this, event);
    break;

    default:
      break;
    }
  }

  return Widget::onEvent(event);
}


void Window::beforeDraw( Engine& painter )
{
  if( _d->background.dirty )
  {
    _d->background.dirty = false;
    _updateBackground();
  }

  Widget::beforeDraw( painter );
}

//! draws the element and its children
void Window::draw( Engine& painter )
{
  if( visible() )
  {
    if( _d->flags.isFlag( fbackgroundVisible ) )
    {
      DrawState pipe( painter, absoluteRect().lefttop(), &absoluteClippingRectRef() );
      pipe.draw( _d->background.image )
          .fallback( _d->background.batch.body )
          .fallback( _d->background.batch.fallback );
    }
  }

  Widget::draw( painter );
}

//! Returns pointer to the maximize button
PushButton* Window::button(ButtonName btn) const
{
  if( btn < buttonClose || btn > buttonMax )
    return 0;

  return _d->buttons[ btn ];
}

//! Set if the window background will be drawn
void Window::setBackgroundVisible(bool draw) {	_d->flags.setFlag( fbackgroundVisible, draw ); }

//! Get if the window background will be drawn
bool Window::backgroundVisible() const {	return _d->flags.isFlag( fbackgroundVisible ); }

//! Set if the window titlebar will be drawn
void Window::setTitleVisible(bool draw)
{
  _d->flags.setFlag( ftitleVisible, draw );
  _d->title->setVisible( draw );
}

//! Get if the window titlebar will be drawn
bool Window::titleVisible() const {	return _d->flags.isFlag( ftitleVisible );}
Rect Window::clientRect() const{	return Rect(0, 0, 0, 0);}

void Window::setBackground( Picture texture )
{
  _d->background.image = texture;
  _d->background.type = bgNone;
  _d->background.batch.body.destroy();
}

void Window::setBackground(Window::BackgroundType type)
{
  _d->background.set( type );
}

void Window::setModal()
{
  ModalScreen* mdScr = new ModalScreen(parent());
  mdScr->addChild(this);

  auto list = findChildren<WidgetClosers*>();
  for (auto closer : list)
    mdScr->installEventHandler(closer);
}

void Window::setFont(const Font& font)
{
  if (_d->title)
  {
    int h = font.getTextSize("A").height();
    _d->title->setHeight(h);
    _d->title->setFont(font);
  }
}

void Window::setFont(const std::string& fname)
{
  Widget::setFont(fname);
}

Picture Window::background() const {return _d->background.image; }

void Window::setWindowFlag( FlagName flag, bool enabled/*=true */ )
{
  _d->flags.setFlag( flag, enabled );
}

void Window::setWindowFlag(const std::string& flagname, bool enabled)
{
  if( flagname == TEXT(fdraggable) ) setWindowFlag(fdraggable,enabled);
  else if( flagname == TEXT(fbackgroundVisible) ) setWindowFlag(fbackgroundVisible,enabled);
  else if( flagname == TEXT(ftitleVisible) ) setWindowFlag(ftitleVisible,enabled);
  else
  {
    Logger::warning( "!!! Cant find flag with name " + flagname );
  }
}

void Window::setupUI(const VariantMap &ui)
{
  Widget::setupUI( ui );

  StringArray buttons = ui.get( "buttons" ).toStringArray();
  if( buttons.empty() || buttons.front() == "off" )
  {
    for( auto& button : _d->buttons )
       button->hide();
  }

  _d->flags.setFlag( fdraggable, !ui.get( "static", false ).toBool() );

  WindowBackgroundHelper helper;
  std::string modeStr = ui.get( "bgtype" ).toString();
  if (!modeStr.empty())
  {
    Window::BackgroundType mode = helper.findType(modeStr);
    setBackground(mode);
  }
}

void Window::setupUI(const vfs::Path& path)
{
  Widget::setupUI(path);
}

void Window::setTextAlignment(const std::string& horizontal, const std::string& vertical)
{
  Widget::setTextAlignment(horizontal, vertical);
  if( _d->title )
    _d->title->setTextAlignment(horizontal, vertical);
}

void Window::setTextAlignment(Alignment horizontal, Alignment vertical)
{
  Widget::setTextAlignment(horizontal, vertical);
  if (_d->title)
    _d->title->setTextAlignment(horizontal, vertical);
}

}//end namespace gui
