#include "window.hpp"
#include "label.hpp"
#include "core/event.hpp"
#include "core/flagholder.hpp"
#include "core/logger.hpp"
#include "gfx/engine.hpp"
#include "modal_widget.hpp"
#include "gfx/decorator.hpp"

using namespace gfx;

namespace gui
{

class Window::Impl
{
public:
	std::vector< PushButton* > buttons;
	Rect headerRect, captionRect;

	Label* title;

	Picture backgroundImage;
	PictureRef bg;
	Point dragStartPosition;
	bool dragging;

	NColor currentColor;
	NColor captionColor;

	FlagHolder<Window::FlagName> flags;
};

//! constructor
Window::Window( Widget* parent, const Rect& rectangle, const std::string& title, int id )
	: Widget( parent, id, rectangle ),
	  _d( new Impl )
{
  _d->flags.setFlag( draggable, true );
  _d->flags.setFlag( backgroundVisible, true );
  _d->flags.setFlag( titleVisible, true );
	_d->title = 0;
#ifdef _DEBUG
	setDebugName( L"NrpWindow");
#endif
	_d->backgroundImage = Picture::getInvalid();
	_d->dragging = false;
	_d->buttons.resize( buttonCount );
	for( unsigned int index=0; index < _d->buttons.size(); index++ )
        _d->buttons[ index ] = NULL;

    // this element is a tab group
  setTabgroup( true );
  setTabStop(true);
  setTabOrder(-1);
  setText( title );
}

void Window::setText(const std::string& text )
{
	Widget::setText( text );
	_d->title->setText( text );
}

void Window::_createSystemButton( ButtonName btnName, /*ELEMENT_STYLE_TYPE configName,*/ const std::string& tooltip, bool visible )
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

//! destructor
Window::~Window()
{
	Logger::warning( "Window was removed" );

#ifdef _CAESARIA_COMPILE_WITH_SCRIPT_
	CallScriptFunction( GUIELEMENT_ON_REMOVE, this, NULL );
#endif
}


//! called if an event happened.
bool Window::onEvent(const NEvent& event)
{
	if( isEnabled() )
	{
		switch(event.EventType)
		{
		case sEventGui:
			if (event.gui.type == guiElementFocusLost)
			{
#ifdef _CAESARIA_COMPILE_WITH_SCRIPT_
								CallScriptFunction( GUI_EVENT + ELEMENT_FOCUS_LOST, this );
#endif
				_d->dragging = false;
			}

			else if (event.gui.type == guiElementFocused)
			{
#ifdef _CAESARIA_COMPILE_WITH_SCRIPT_
										CallScriptFunction( GUI_EVENT + ELEMENT_FOCUSED, this );
#endif
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
				_d->dragging = _d->flags.isFlag( draggable );
				bringToFront();
#ifdef _CAESARIA_COMPILE_WITH_SCRIPT_
								CallScriptFunction( NMOUSE_EVENT + LMOUSE_PRESSED_DOWN, this, (void*)&event );
#endif
				return true;
			case mouseRbtnRelease:
			case mouseLbtnRelease:
				_d->dragging = false;
#ifdef _CAESARIA_COMPILE_WITH_SCRIPT_
				CallScriptFunction( NMOUSE_EVENT + event.MouseEvent.Event, this, (void*)&event );
#endif
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
#ifdef _CAESARIA_COMPILE_WITH_SCRIPT_
				CallScriptFunction( GUIELEMENT_KEY_INPUT, this, (void*)&event );
#endif
			}
		break;

		default:
			break;
		}
	}

	return parent()->onEvent(event);
}


void Window::beforeDraw( Engine& painter )
{
	if( _d->bg.isNull() || ( size() != _d->bg->size() ) )
	{
		_d->bg.init( size() );
		PictureDecorator::draw( *_d->bg, Rect( Point( 0, 0), size() ), PictureDecorator::whiteFrame );
	}

	Widget::beforeDraw( painter );
}

//! draws the element and its children
void Window::draw( Engine& painter )
{
	if( isVisible() )
	{
		NColor colors[ 4 ] = { _d->currentColor, _d->currentColor, _d->currentColor, _d->currentColor };

		if( _d->backgroundImage.isValid() )
		{
			Rect rsize( Point( 0, 0 ), _d->backgroundImage.size() );
			painter.draw( _d->backgroundImage, absoluteRect().UpperLeftCorner );
		}
	}

	Widget::draw( painter );
}

//! Returns pointer to the maximize button
PushButton* Window::getButton(ButtonName btn) const
{
  if( btn < buttonClose || btn > buttonMax )
    return 0;

	return _d->buttons[ btn ];
}

//! Set if the window background will be drawn
void Window::setBackgroundVisible(bool draw)
{
	_d->flags.setFlag( backgroundVisible, draw );
}

//! Get if the window background will be drawn
bool Window::isBackgroundVisible() const
{
	return _d->flags.isFlag( backgroundVisible );
}

//! Set if the window titlebar will be drawn
void Window::setHeaderVisible(bool draw)
{
	_d->flags.setFlag( titleVisible, draw );
	_d->title->setVisible( draw );
}

//! Get if the window titlebar will be drawn
bool Window::isHeaderVisible() const
{
	return _d->flags.isFlag( titleVisible );
}

void Window::setBackground( Picture texture )
{
	_d->backgroundImage = texture;
}

Rect Window::getClientRect() const
{
	return Rect(0, 0, 0, 0);
}

void Window::setModal()
{
	ModalScreen* mdScr = new ModalScreen( parent() );
	mdScr->addChild( this );
}

Picture Window::getBackground() const
{
	return _d->backgroundImage;
}

void Window::setWindowFlag( FlagName flag, bool enabled/*=true */ )
{
	_d->flags.setFlag( flag, enabled );
}

void Window::setTextAlignment( Alignment horizontal, Alignment vertical )
{
	Widget::setTextAlignment( horizontal, vertical );
	_d->title->setTextAlignment( horizontal, vertical );
}

}//end namespace gui
