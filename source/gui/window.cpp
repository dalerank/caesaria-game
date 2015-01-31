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
#include "core/event.hpp"
#include "core/flagholder.hpp"
#include "core/logger.hpp"
#include "gfx/engine.hpp"
#include "modal_widget.hpp"
#include "core/variant_map.hpp"
#include "gfx/decorator.hpp"
#include "gfx/picturesarray.hpp"

using namespace gfx;

namespace gui
{

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

class Window::Impl
{
public:
	std::vector< PushButton* > buttons;
	Rect headerRect, captionRect;

	Label* title;

	Picture backgroundImage;
	Pictures bgStyle;
	Point dragStartPosition;
	Window::BackgroundType backgroundType;
	bool dragging;

	NColor currentColor;
	NColor captionColor;

	FlagHolder<Window::FlagName> flags;
};

//! constructor
Window::Window( Widget* parent, const Rect& rectangle, const std::string& title, int id, BackgroundType type )
	: Widget( parent, id, rectangle ),
	  _d( new Impl )
{
  _d->flags.setFlag( fdraggable, true );
  _d->flags.setFlag( fbackgroundVisible, true );
  _d->flags.setFlag( ftitleVisible, true );
	_d->title = 0;
#ifdef _DEBUG
  setDebugName( "Window");
#endif
	_d->backgroundImage = Picture::getInvalid();
	_d->dragging = false;
	_d->buttons.resize( buttonCount );
	for( unsigned int index=0; index < _d->buttons.size(); index++ )
        _d->buttons[ index ] = NULL;

  _init();

    // this element is a tab group
  setBackground( type );
  setTabgroup( true );
  setTabStop(true);
  setTabOrder(-1);
  setText( title );
}

void Window::setText(const std::string& text )
{
	Widget::setText( text );
  if( _d->title )
    _d->title->setText( text );
}

void Window::_createSystemButton( ButtonName btnName, const std::string& tooltip, bool visible )
{
    PushButton*& btn = _d->buttons[ btnName ];
    if( !btn )
    {
        btn = new PushButton( this, Rect( 0, 0, 10,10 ) );
        btn->setTooltipText( tooltip );
        btn->setVisible(visible);
        btn->setSubElement(true);
        btn->setTabStop(false);
        btn->setAlignment(align::lowerRight, align::lowerRight, align::upperLeft, align::upperLeft);
    }
}

void Window::_init()
{
  _createSystemButton( buttonClose, "Close", true );
  _createSystemButton( buttonMin,"Min", false );
  _createSystemButton( buttonMax, "Restore", false );

	if( !_d->title )
	{
		_d->title = new Label( this, Rect( 0, 0, width(), 20 ), text(), false );
		_d->title->setSubElement( true );
	}

  _d->title->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::upperLeft );
}

void Window::_resizeEvent()
{
  Widget::_resizeEvent();
  if( _d->backgroundType != bgNone  )
  {
    setBackground( _d->backgroundType );
  }
}

Window::~Window()
{
	Logger::warning( "Window was removed" );
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
				_d->dragging = false;
			}

			else if (event.gui.type == guiElementFocused)
			{
					if( ((event.gui.caller == this) || isMyChild(event.gui.caller)))
						bringToFront();
			}
			else
				if (event.gui.type == guiButtonClicked)
				{
					if (event.gui.caller == _d->buttons[ buttonClose ] )
					{
    					// send close event to parent
    					// if the event was not absorbed
              if( !parent()->onEvent( NEvent::Gui( this, 0, guiElementClosed ) ) )
					        deleteLater();
              return true;
					}
				}
		break;

		case sEventMouse:
			switch(event.mouse.type)
			{
			case mouseLbtnPressed:
				_d->dragStartPosition = event.mouse.pos();
				_d->dragging = _d->flags.isFlag( fdraggable );
				bringToFront();

				return true;
			case mouseRbtnRelease:
			case mouseLbtnRelease:
				_d->dragging = false;

				return true;
			case mouseMoved:
				if ( !event.mouse.isLeftPressed() )
					_d->dragging = false;

				if (_d->dragging)
				{
					// gui window should not be dragged outside its parent
					const Rect& parentRect = parent()->absoluteRect();
					if( (event.mouse.x < parentRect.left() +1 ||
						event.mouse.y < parentRect.top() +1 ||
						event.mouse.x > parentRect.right() -1 ||
						event.mouse.y > parentRect.bottom() -1))
						return true;

					move( event.mouse.pos() - _d->dragStartPosition );
					_d->dragStartPosition = event.mouse.pos();

                    return true;
				}
				break;
			default:
				break;
			}
		break;

		case sEventKeyboard:
			{
			}
		break;

		default:
			break;
		}
	}

  return Widget::onEvent(event);
}


void Window::beforeDraw( Engine& painter )
{
	Widget::beforeDraw( painter );
}

//! draws the element and its children
void Window::draw( Engine& painter )
{
	if( visible() )
	{
		//NColor colors[ 4 ] = { _d->currentColor, _d->currentColor, _d->currentColor, _d->currentColor };

		if( _d->flags.isFlag( fbackgroundVisible ) )
		{
			if( _d->backgroundImage.isValid() )
			{
				painter.draw( _d->backgroundImage, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
			}
			else
			{
				painter.draw( _d->bgStyle, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
			}
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
void Window::setHeaderVisible(bool draw)
{
	_d->flags.setFlag( ftitleVisible, draw );
	_d->title->setVisible( draw );
}

//! Get if the window titlebar will be drawn
bool Window::headerVisible() const {	return _d->flags.isFlag( ftitleVisible );}
Rect Window::clientRect() const{	return Rect(0, 0, 0, 0);}

void Window::setBackground( Picture texture )
{
  _d->backgroundImage = texture;
  _d->backgroundType = bgNone;
  _d->bgStyle.clear();
}

void Window::setBackground(Window::BackgroundType type)
{
  _d->backgroundImage = Picture::getInvalid();
  _d->backgroundType = type;
  _d->bgStyle.clear();
  switch( type )
  {
  case bgWhiteFrame: Decorator::draw( _d->bgStyle, Rect( 0, 0, width(), height()), Decorator::whiteFrame ); break;
  default: break;
  }
}

void Window::setModal()
{
  ModalScreen* mdScr = new ModalScreen( parent() );
  mdScr->addChild( this );
}

Picture Window::background() const {return _d->backgroundImage; }

void Window::setWindowFlag( FlagName flag, bool enabled/*=true */ )
{
  _d->flags.setFlag( flag, enabled );
}

void Window::setupUI(const VariantMap &ui)
{
  Widget::setupUI( ui );

  StringArray buttons = ui.get( "buttons" ).toStringArray();  
  if( buttons.empty() || buttons.front() == "off" )
  {
    foreach( i, _d->buttons )
       (*i)->hide();
  }

  _d->flags.setFlag( fdraggable, !ui.get( "static", false ).toBool() );

  WindowBackgroundHelper helper;
  std::string modeStr = ui.get( "bgtype" ).toString();
  if( !modeStr.empty() )
  {
    Window::BackgroundType mode = helper.findType( modeStr );
    setBackground( mode );
  }
}

void Window::setupUI(const vfs::Path& path)
{
  Widget::setupUI( path );
}

void Window::setTextAlignment( Alignment horizontal, Alignment vertical )
{
	Widget::setTextAlignment( horizontal, vertical );
	_d->title->setTextAlignment( horizontal, vertical );
}

}//end namespace gui
