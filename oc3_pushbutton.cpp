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


#include "oc3_pushbutton.hpp"
#include "oc3_event.hpp"
#include "oc3_time.hpp"
#include "oc3_guienv.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_gfx_engine.hpp"
#include <iostream>

struct ButtonState
{
    Picture* bgTexture;
    Picture* texture;
    Picture* iconTexture;
    //SDL_Color color;
    //bool overrideColorEnabled;
    Font font;
    Rect rectangle;
    //ElementStyle* style;
};    

class PushButton::Impl
{
public:
    bool pressed;
    bool isPushButton;
    bool drawBody;
    Rect textRect;
    Rect iconRect;
    int clickTime;
    
    ElementState currentButtonState, lastButtonState;
    ButtonState buttonStates[ StateCount ];

oc3_signals public:
    Signal0<> onClickedSignal;

public:

    Impl() : pressed(false), 
        isPushButton(false), drawBody( true ),
        clickTime(0)
    {
        for( int i=0; i < StateCount; i++)
        {
            buttonStates[ ElementState(i) ].texture = 0;
            buttonStates[ ElementState(i) ].iconTexture = 0;
            buttonStates[ ElementState(i) ].bgTexture = 0;
            buttonStates[ ElementState(i) ].font = FontCollection::instance().getFont(FONT_2);
        }
    }    

    ~Impl()
    {
        for( int i=0; i < StateCount; i++)
            releaseTexture( ElementState(i) );
    }  

    void releaseTexture( ElementState state )
    {
      if( buttonStates[ state ].texture )
      {
        GfxEngine::instance().deletePicture( *buttonStates[ state ].texture );
        buttonStates[ state ].texture = 0;
      }
    }
};

//! constructor
PushButton::PushButton( Widget* parent,
					    const Rect& rectangle, 
                        const std::string& caption,
                        int id, 
					    bool noclip )
: Widget( parent, id, rectangle ), _d( new Impl )
{
    setDebugName( "OC3_pushbutton" );

    _d->pressed = false;
    _d->currentButtonState = stNormal;
    _d->lastButtonState = StateCount;
    setTextAlignment( alignCenter, alignCenter );

    _text = caption;
    setNotClipped(noclip);

//     for( int i=0; i < StateCount; i++)
//     {
//        _updateTexture( ElementState(i) );
//     }
}

void PushButton::_updateTexture( ElementState state )
{
    Size btnSize = getSize();
    Picture*& curTxs = _d->buttonStates[ state ].texture;
    
    if( curTxs && curTxs->getSize() != btnSize )
        _d->releaseTexture( state );

    if( !curTxs )
      curTxs = &GfxEngine::instance().createPicture( btnSize.getWidth(), btnSize.getHeight() );

    // draw button background
    if( _d->buttonStates[ state ].bgTexture )
    {
      curTxs->draw( *_d->buttonStates[ state ].bgTexture, 0, 0 );
    }    
    else
    {
      const int picId[StateCount] = { 22, 25, 25, 22, 25 };
      GuiPaneling::instance().draw_basic_text_button( *curTxs, 0, 0, getSize().getWidth(), picId[ state ] );
    }

    if( _d->buttonStates[ state ].font.isValid() )
    {
      Rect textRect = _d->buttonStates[ state ].font.calculateTextRect( getText(), Rect( 0, 0, getWidth(), getHeight() ),
                                                                        getHorizontalTextAlign(), getVerticalTextAlign() );
      _d->buttonStates[ state ].font.draw( *curTxs, getText(), textRect.getLeft(), textRect.getTop() );
    }
}

//! destructor
PushButton::~PushButton()
{
}

void PushButton::setIsPushButton( bool value )
{
    _d->isPushButton = value;
}

bool PushButton::isPushButton() const
{
    return _d->isPushButton;
}

void PushButton::setPicture( Picture* picture, ElementState state )
{
    Rect rectangle( Point(0,0), picture ? picture->getSize() : Size( 0, 0 ) );

    _d->buttonStates[ state ].bgTexture = picture;
    _d->buttonStates[ state ].rectangle = rectangle;
    _updateTexture( state );
}

void PushButton::setPressed( bool pressed )
{
    if( _d->pressed != pressed)
    {
        _d->clickTime = DateTime::getElapsedTime();
        _d->pressed = pressed;
    }
}

bool PushButton::isPressed() const
{
    return _d->pressed;
}

//! called if an event happened.
bool PushButton::onEvent(const NEvent& event)
{
    if( !isEnabled() )
        return getParent()->onEvent(event);

    switch(event.EventType)
    {
    case OC3_KEYBOARD_EVENT:
        if( event.KeyboardEvent.PressedDown
            && ( event.KeyboardEvent.Key == KEY_RETURN || event.KeyboardEvent.Key == KEY_SPACE ) )
        {
            if ( !isPushButton() )
                setPressed(true);
            else
                setPressed(!isPressed());

            return true;
        }

        if( isPressed()
            && !isPushButton()
            && event.KeyboardEvent.PressedDown
            && event.KeyboardEvent.Key == KEY_ESCAPE)
        {
                setPressed(false);
                return true;
        }
        else
          if( !event.KeyboardEvent.PressedDown
              && isPressed()
              && ( event.KeyboardEvent.Key == KEY_RETURN || event.KeyboardEvent.Key == KEY_SPACE ) )
          {
              if (!isPushButton())
                      setPressed(false);

              btnClicked_();
              return true;
          }
     break;

     case OC3_GUI_EVENT:
            switch(event.GuiEvent.EventType)
            {
            case OC3_ELEMENT_FOCUS_LOST:
                            if (event.GuiEvent.Caller == this && !isPushButton())
                                    setPressed(false);
            break;

            default:
            break;
            }
    break;

    case OC3_MOUSE_EVENT:
            switch( event.MouseEvent.Event  )
            {
            case OC3_LMOUSE_PRESSED_DOWN: return leftMouseBtnPressed_( event );
            case OC3_LMOUSE_LEFT_UP: return btnMouseUp_( event );

            default:
            break;
            }
    break;
	
    default:
    break;
    }

	return getParent() ? getParent()->onEvent(event) : false;
}

void PushButton::btnClicked_()
{
    getParent()->onEvent( NEvent::Gui( this, 0, OC3_BUTTON_CLICKED ) );

    onClicked().emit();
}

Signal0<>& PushButton::onClicked()
{
    return _d->onClickedSignal;
}

bool PushButton::btnMouseUp_( const NEvent& event )
{
	bool wasPressed = isPressed();

	if ( !getAbsoluteClippingRect().isPointInside( event.MouseEvent.getPosition() ) )
	{
		if (!isPushButton())
			setPressed(false);
		return true;
	}

	if (!isPushButton())
		setPressed(false);
	else
	{
		setPressed(!isPressed());
	}

	if ((!isPushButton() && wasPressed ) ||
		(isPushButton() && wasPressed != isPressed()))
		btnClicked_();

	return true;
}

bool PushButton::leftMouseBtnPressed_( const NEvent& event )
{
	if( _environment->hasFocus(this) &&
		!getAbsoluteClippingRect().isPointInside( event.MouseEvent.getPosition() ) )
	{
		removeFocus();
		return false;
	}

	if (!isPushButton())
		setPressed(true);

	setFocus();
	return true;
}

ElementState PushButton::getActiveButtonState_()
{
    if( isEnabled() )
        return ( isPressed() 
                    ? stPressed 
                    : ( isHovered() ? stHovered : stNormal) );

    return stDisabled;
}

void PushButton::beforeDraw( GfxEngine& painter )
{
    // todo:	move sprite up and text down if the pressed state has a sprite
    //			draw sprites for focused and mouse-over 
    //          Point spritePos = AbsoluteRect.getCenter();
    _d->currentButtonState = getActiveButtonState_();

    if( !_d->buttonStates[ _d->currentButtonState ].texture )
        _updateTexture( _d->currentButtonState );

	Widget::beforeDraw( painter  );
}

bool PushButton::isBodyVisible() const
{
    return _d->drawBody;
}

//! draws the element and its children
void PushButton::draw( GfxEngine& painter )
{
    if( !isVisible() )
        return;

	// todo:	move sprite up and text down if the pressed state has a sprite
	//			draw sprites for focused and mouse-over 
    if( isBodyVisible() )
    {
		const ButtonState& state = _d->buttonStates[ _d->currentButtonState ];
        if( state.texture )
        {
            painter.drawPicture( *state.texture, getScreenLeft(), getScreenTop() );

//             if( isEnabled() &&
//                 ( hoverImageOpacity <= 0xff ) &&
//                 ( hoverImageOpacity >= 5 ) &&
//                 _d->buttonStates[ stHovered ].texture.isValid() )
//             {
//                 Color hoverColor = resultOpacityColor_;
//                 hoverColor.setAlpha( math::middle_<u32>( u32( hoverImageOpacity ), resultOpacityColor_.getAlpha(), 255 ) );
// 
//                 Color hoverOpacityColors[ 4 ] =  { hoverColor, hoverColor, hoverColor, hoverColor };
// 
//                 painter->drawImage( _buttonStates[ stHovered ].texture, getAbsoluteRect(),
//                                       _buttonStates[ stHovered].rectangle, &getAbsoluteClippingRectRef(), hoverOpacityColors, true  );
//             }			
        }
	}

    drawIcon( painter );
 
    Widget::draw( painter );
}

void PushButton::drawIcon( GfxEngine& painter )
{
    Picture* iconTexture = _d->buttonStates[ _d->currentButtonState ].iconTexture;	

    if( !iconTexture )
        return;

    Point pos = convertLocalToScreen( _d->iconRect ).UpperLeftCorner;
    painter.drawPicture( *iconTexture, pos.getX(), pos.getY() );
}

void PushButton::setText( const std::string& text )
{
	Widget::setText( text );

    for( int i=0; i != StateCount; i++ )
        _updateTexture( ElementState(i) );
}

void PushButton::setFont( const Font& font, ElementState state )
{
    _d->buttonStates[ state ].font = font;
    _updateTexture( state );
}

void PushButton::setFont( const Font& font )
{
    for( int i=0; i != StateCount; i++ )
    {
        _d->buttonStates[ ElementState(i) ].font = font;
        _updateTexture( ElementState(i) );
    }
}

Picture* PushButton::_getPicture( ElementState state )
{
    return _d->buttonStates[ state ].texture; 
}

Font& PushButton::getFont( ElementState state )
{
    return _d->buttonStates[ state ].font;
}

void PushButton::resizeEvent_()
{
    for( int i=0; i != StateCount; i++ )
    {
        _updateTexture( ElementState(i) );
    }
}