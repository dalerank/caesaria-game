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

#include <iostream>
#include <memory>

#include "pushbutton.hpp"
#include "core/event.hpp"
#include "core/time.hpp"
#include "environment.hpp"
#include "gfx/decorator.hpp"
#include "gfx/engine.hpp"
#include "core/color.hpp"

namespace gui
{

class BackgroundStyleHelper : public EnumsHelper<PushButton::BackgroundStyle>
{
public:
  BackgroundStyleHelper() : EnumsHelper(PushButton::noBackground)
  {
    append( PushButton::grayBorderLine, "grayBorderLine" );
    append( PushButton::smallGrayBorderLine, "smallGrayBorderLine" );
    append( PushButton::whiteBorderUp, "whiteBorderUp" );
    append( PushButton::blackBorderUp, "blackBorderUp" );
    append( PushButton::noBackground, "noBackground" );
  }
};

struct ButtonState
{
  Picture bgTexture;
  PictureRef background;
  PictureRef textPicture;
  Picture iconTexture;
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
  Point textOffset;
  PushButton::BackgroundStyle bgStyle;
  
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
      buttonStates[ ElementState(i) ].background.reset();
      buttonStates[ ElementState(i) ].textPicture.reset();
      buttonStates[ ElementState(i) ].iconTexture = Picture::getInvalid();
      buttonStates[ ElementState(i) ].bgTexture = Picture::getInvalid();
      buttonStates[ ElementState(i) ].font = Font::create( FONT_2 );
    }
  }    

  ~Impl()
  {
    for( int i=0; i < StateCount; i++)
      releaseTexture( ElementState(i) );
  }  

  void releaseTexture( ElementState state )
  {
    if( buttonStates[ state ].background )
    {
      buttonStates[ state ].background.reset();
      buttonStates[ state ].textPicture.reset();
    }
  }
};

//! constructor
PushButton::PushButton(Widget* parent )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->currentButtonState = stNormal;
  _d->lastButtonState = StateCount;
  _d->pressed = false;
  _d->bgStyle = grayBorderLine;
  setTextAlignment( alignCenter, alignCenter );
}

PushButton::PushButton( Widget* parent,
                        const Rect& rectangle,
                        const std::string& caption,
                        int id, 
					              bool noclip, 
                        const BackgroundStyle bgStyle )
: Widget( parent, id, rectangle ), _d( new Impl )
{
  setDebugName( "OC3_pushbutton" );

  _d->pressed = false;
  _d->currentButtonState = stNormal;
  _d->lastButtonState = StateCount;
  _d->bgStyle = bgStyle;
  setTextAlignment( alignCenter, alignCenter );

  setText( caption );
  setNotClipped(noclip);
}

void PushButton::_updateTexture( ElementState state )
{
  Size btnSize = getSize();
  PictureRef& curTxs = _d->buttonStates[ state ].background;
  PictureRef& textTxs = _d->buttonStates[ state ].textPicture;
  
  if( !curTxs.isNull() && curTxs->getSize() != btnSize )
  {
    _d->releaseTexture( state );
  }

  if( curTxs.isNull() )
  {
    curTxs.reset( Picture::create( btnSize ) );
  }

  if( textTxs.isNull() )
  {
    textTxs.reset( Picture::create( btnSize ) );
  }

  // draw button background
  if( _d->buttonStates[ state ].bgTexture.isValid() )
  {
    curTxs->draw( _d->buttonStates[ state ].bgTexture, 0, 0, false );
  }    
  else
  {
    switch( _d->bgStyle )
    {
    case smallGrayBorderLine:
    {
      PictureDecorator::Mode mode = (state == stNormal || state == stDisabled)
                                      ? PictureDecorator::smallGreyPanel
                                      : PictureDecorator::smallBrownPanel;
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), getSize() ), mode );
    }
    break;

    case grayBorderLine:
    {
      PictureDecorator::Mode mode = (state == stNormal || state == stDisabled) 
                                        ? PictureDecorator::lightgreyPanel 
                                        : PictureDecorator::greyPanel;
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), getSize() ), mode );
    }
    break;

    case whiteBorderUp:
    {
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteArea );
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), getSize() ), 
                              state == stHovered ? PictureDecorator::brownBorder : PictureDecorator::whiteBorderA );
    }
    break;

    case blackBorderUp:
    {
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::blackArea );
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), getSize() ), 
                              state == stHovered ? PictureDecorator::brownBorder : PictureDecorator::whiteBorderA );
    }
    break;

    case noBackground:
      curTxs->fill( 0x00ffffff, Rect( 0, 0, 0, 0 ) );
    break;
    }
  }

  Font stFont = _d->buttonStates[ state ].font;
  if( textTxs && stFont.isValid() )
  {
    Rect textRect = stFont.calculateTextRect( getText(), Rect( 0, 0, getWidth(), getHeight() ),
                                              getHorizontalTextAlign(), getVerticalTextAlign() );
    textTxs->fill( 0x00ffffff, Rect( 0, 0, 0, 0 ) );
    stFont.draw( *textTxs, getText(), textRect.UpperLeftCorner + _d->textOffset );
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

void PushButton::setupUI(const VariantMap &ui)
{
  Widget::setupUI( ui );

  Variant tmp;
  tmp = ui.get( "bgtype" );
  if( tmp.isValid() )
  {
    BackgroundStyleHelper helper;
    setBackgroundStyle( helper.findType( tmp.toString() ) );
  }

  _d->textOffset = ui.get( "textOffset" ).toPoint();

  VariantList vlist = ui.get( "normal" ).toList();
  if( !vlist.empty() ) setPicture( vlist.get( 0 ).toString(), vlist.get( 1 ).toInt(), stNormal );

  vlist = ui.get( "hovered" ).toList();
  if( !vlist.empty() ) setPicture( vlist.get( 0 ).toString(), vlist.get( 1 ).toInt(), stHovered  );

  vlist = ui.get( "pressed" ).toList();
  if( !vlist.empty() ) setPicture( vlist.get( 0 ).toString(), vlist.get( 1 ).toInt(), stPressed );

  vlist = ui.get( "disabled" ).toList();
  if( !vlist.empty() ) setPicture( vlist.get( 0 ).toString(), vlist.get( 1 ).toInt(), stDisabled );
}

void PushButton::setTextOffset(const Point& offset)
{
  _d->textOffset = offset;
}

bool PushButton::isPushButton() const
{
  return _d->isPushButton;
}

void PushButton::setPicture( const Picture& picture, ElementState state )
{
  Rect rectangle( Point(0,0), picture.isValid() ? picture.getSize() : Size( 0, 0 ) );

  _d->buttonStates[ state ].bgTexture = picture;
  _d->buttonStates[ state ].rectangle = rectangle;
  _updateTexture( state );
}

void PushButton::setPicture(const std::string& rcname, int index, ElementState state)
{
  setPicture( Picture::load( rcname, index ), state );
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
  {
    return getParent()->onEvent(event);
  }

  switch(event.EventType)
  {
  case sEventKeyboard:
    if( event.keyboard.pressed
        && ( event.keyboard.key == KEY_RETURN || event.keyboard.key == KEY_SPACE ) )
    {
      if ( !isPushButton() )
        setPressed(true);
      else
        setPressed( !isPressed() );

      return true;
    }

    if( isPressed() && !isPushButton() && event.keyboard.pressed
        && event.keyboard.key == KEY_ESCAPE)
    {
      setPressed(false);
      return true;
    }
    else
      if( !event.keyboard.pressed && isPressed()
          && ( event.keyboard.key == KEY_RETURN || event.keyboard.key == KEY_SPACE ) )
      {
        if (!isPushButton())
        {
          setPressed(false);
        }

        _btnClicked();
        return true;
      }
   break;

   case sEventGui:
    switch(event.gui.type)
    {
    case guiElementFocusLost:
      if (event.gui.caller == this && !isPushButton())
      {
        setPressed(false);
      }
    break;

    default:
    break;
    }
  break;

  case sEventMouse:
    switch( event.mouse.type  )
    {
    case mouseLbtnPressed: return _leftMouseBtnPressed( event );
    case mouseLbtnRelease: return _btnMouseUp( event );

    default:
    break;
    }
  break;
	
  default:
  break;
  }

	return getParent() ? getParent()->onEvent(event) : false;
}

void PushButton::_btnClicked()
{
  getParent()->onEvent( NEvent::Gui( this, 0, guiButtonClicked ) );

  onClicked().emit();
}

Signal0<>& PushButton::onClicked()
{
  return _d->onClickedSignal;
}

bool PushButton::_btnMouseUp( const NEvent& event )
{
	bool wasPressed = isPressed();

	if ( !getAbsoluteClippingRect().isPointInside( event.mouse.getPosition() ) )
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
		_btnClicked();

	return true;
}

bool PushButton::_leftMouseBtnPressed( const NEvent& event )
{
	if( _environment->hasFocus(this) &&
		!getAbsoluteClippingRect().isPointInside( event.mouse.getPosition() ) )
	{
		removeFocus();
		return false;
	}

	if (!isPushButton())
		setPressed(true);

	setFocus();
	return true;
}

ElementState PushButton::_getActiveButtonState()
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
  // draw sprites for focused and mouse-over 
  // Point spritePos = AbsoluteRect.getCenter();
  _d->currentButtonState = _getActiveButtonState();

  if( !_d->buttonStates[ _d->currentButtonState ].background )
  {
    _updateTexture( _d->currentButtonState );
  }

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
	//			  draw sprites for focused and mouse-over 
  const ButtonState& state = _d->buttonStates[ _d->currentButtonState ];

  if( isBodyVisible() )
  {
    if( state.background )
    {
      painter.drawPicture( *state.background, getScreenLeft(), getScreenTop(), &getAbsoluteClippingRectRef() );

//             if( isEnabled() &&
//                 ( hoverImageOpacity <= 0xff ) &&
//                 ( hoverImageOpacity >= 5 ) &&
//                 _d->buttonStates[ stHovered ].background.isValid() )
//             {
//                 Color hoverColor = resultOpacityColor_;
//                 hoverColor.setAlpha( math::middle_<u32>( u32( hoverImageOpacity ), resultOpacityColor_.getAlpha(), 255 ) );
// 
//                 Color hoverOpacityColors[ 4 ] =  { hoverColor, hoverColor, hoverColor, hoverColor };
// 
//                 painter->drawImage( _buttonStates[ stHovered ].background, getAbsoluteRect(),
//                                       _buttonStates[ stHovered].rectangle, &getAbsoluteClippingRectRef(), hoverOpacityColors, true  );
//             }			
    }
	}

  if( state.textPicture )
  {
    painter.drawPicture( *state.textPicture, getScreenLeft(), getScreenTop(), &getAbsoluteClippingRectRef() );
  }

  drawIcon( painter );
 
  Widget::draw( painter );
}

void PushButton::drawIcon( GfxEngine& painter )
{
  const Picture& iconTexture = _d->buttonStates[ _d->currentButtonState ].iconTexture;	

  if( !iconTexture.isValid() )
      return;

  Point pos = convertLocalToScreen( _d->iconRect ).UpperLeftCorner;
  painter.drawPicture( iconTexture, pos.getX(), pos.getY() );
}

void PushButton::setText( const std::string& text )
{
	Widget::setText( text );

  _resizeEvent();
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

PictureRef& PushButton::_getBackground( ElementState state )
{
  return _d->buttonStates[ state ].background; 
}

PictureRef& PushButton::_getTextPicture( ElementState state )
{
  return _d->buttonStates[ state ].textPicture; 
}

Font& PushButton::getFont( ElementState state )
{
  return _d->buttonStates[ state ].font;
}

void PushButton::_resizeEvent()
{
  for( int i=0; i != StateCount; i++ )
  {
    _updateTexture( ElementState(i) );
  }
}

void PushButton::setBackgroundStyle( const BackgroundStyle style )
{
  _d->bgStyle = style;
  _resizeEvent();
}

}//end namespace gui
