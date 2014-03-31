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
  BackgroundStyleHelper()
    : EnumsHelper<PushButton::BackgroundStyle>(PushButton::noBackground)
  {
    append( PushButton::greyBorderLine, "grayBorderLine" );
    append( PushButton::greyBorderLineBig, "greyBorderLineBig" );
    append( PushButton::greyBorderLineSmall, "smallGrayBorderLine" );
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
  Point iconOffset;
  Font font;
  Rect rectangle;
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
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), __INIT_IMPL(PushButton)
{
  __D_IMPL(_d,PushButton)
  _d->currentButtonState = stNormal;
  _d->lastButtonState = StateCount;
  _d->pressed = false;
  _d->bgStyle = greyBorderLine;
  setTextAlignment( alignCenter, alignCenter );
}

PushButton::PushButton( Widget* parent,
                        const Rect& rectangle,
                        const std::string& caption,
                        int id,
					              bool noclip,
                        const BackgroundStyle bgStyle )
: Widget( parent, id, rectangle ), __INIT_IMPL(PushButton)
{
  __D_IMPL(_d,PushButton)
  setDebugName( "pushbutton" );

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
  __D_IMPL(_d,PushButton)
  Size btnSize = size();      
  PictureRef& curTxs = _d->buttonStates[ state ].background;
  PictureRef& textTxs = _d->buttonStates[ state ].textPicture;

  if( !curTxs.isNull() && curTxs->size() != btnSize )
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
    curTxs->fill( 0x00000000, Rect( Point( 0, 0 ), btnSize ) );
    curTxs->draw( _d->buttonStates[ state ].bgTexture, 0, 0, false );
  }
  else
  {
    switch( _d->bgStyle )
    {
    case greyBorderLineSmall:
    {
      PictureDecorator::Mode mode = (state == stNormal || state == stDisabled)
                                      ? PictureDecorator::greyPanelSmall
                                      : PictureDecorator::brownPanelSmall;
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), size() ), mode );
    }
    break;

    case greyBorderLineBig:
    {
      PictureDecorator::Mode mode = (state == stNormal || state == stDisabled)
                                        ? PictureDecorator::greyPanelBig
                                        : PictureDecorator::lightgreyPanelBig;
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), size() ), mode );
    }
    break;

    case greyBorderLine:
    {
      PictureDecorator::Mode mode = (state == stNormal || state == stDisabled)
                                        ? PictureDecorator::lightgreyPanel
                                        : PictureDecorator::greyPanel;
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), size() ), mode );
    }
    break;

    case whiteBorderUp:
    {
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteArea );
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), size() ),
                              state == stHovered ? PictureDecorator::brownBorder : PictureDecorator::whiteBorderA );
    }
    break;

    case blackBorderUp:
    {
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), size() ), PictureDecorator::blackArea );
      PictureDecorator::draw( *curTxs, Rect( Point( 0, 0 ), size() ),
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
    Rect textRect = stFont.calculateTextRect( text(), Rect( 0, 0, width(), height() ),
                                              getHorizontalTextAlign(), getVerticalTextAlign() );
    textTxs->fill( 0x00ffffff, Rect( 0, 0, 0, 0 ) );
    stFont.draw( *textTxs, text(), textRect.UpperLeftCorner + _d->textOffset );
  }
}

//! destructor
PushButton::~PushButton(){}
void PushButton::setIsPushButton( bool value )
{
  __D_IMPL(_d,PushButton)
  _d->isPushButton = value;
}

void PushButton::setupUI(const VariantMap &ui)
{
  Widget::setupUI( ui );

  __D_IMPL(_d,PushButton)

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

void PushButton::setTextOffset(const Point& offset) { __D_IMPL(_d,PushButton); _d->textOffset = offset;}
bool PushButton::isPushButton() const { __D_IMPL_CONST(_d,PushButton); return _d->isPushButton; }

void PushButton::setPicture(Picture picture, ElementState state )
{
  __D_IMPL(_d,PushButton);
  Rect rectangle( Point(0,0), picture.size() );

  _d->buttonStates[ state ].bgTexture = picture;
  _d->buttonStates[ state ].rectangle = rectangle;
  _updateTexture( state );
}

void PushButton::setPicture(const std::string& rcname, int index, ElementState state)
{
  setPicture( Picture::load( rcname, index ), state );
}

void PushButton::setIcon( const std::string& rcname, int index, ElementState state)
{
  __D_IMPL(_d,PushButton);
  _d->buttonStates[ state ].iconTexture = Picture::load( rcname, index );
}

void PushButton::setIconOffset(Point offset)
{
  __D_IMPL(_d,PushButton);
  for( int i=stNormal; i < StateCount; i++ )
  {
    _d->buttonStates[ i ].iconOffset = offset;
  }
}

void PushButton::setIcon(const std::string& rcname, int index)
{
  __D_IMPL(_d,PushButton);
  Picture pic = Picture::load( rcname, index );
  for( int i=stNormal; i < StateCount; i++ )
  {
    _d->buttonStates[ i ].iconTexture = pic;
  }
}

void PushButton::setPicture( const std::string& rcname, int index )
{
  Picture pic = Picture::load( rcname, index );
  for( int i=stNormal; i < StateCount; i++ )
  {
    setPicture( pic, (ElementState)i );
  }
}

void PushButton::setPressed( bool pressed )
{
  __D_IMPL(_d,PushButton);
  if( _d->pressed != pressed)
  {
    _d->clickTime = DateTime::elapsedTime();
    _d->pressed = pressed;
  }
}

bool PushButton::isPressed() const {  __D_IMPL_CONST(_d,PushButton); return _d->pressed; }

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

Signal0<>& PushButton::onClicked() { __D_IMPL(_d,PushButton); return _d->onClickedSignal; }

bool PushButton::_btnMouseUp( const NEvent& event )
{
	bool wasPressed = isPressed();

	if ( !absoluteClippingRect().isPointInside( event.mouse.pos() ) )
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
		!absoluteClippingRect().isPointInside( event.mouse.pos() ) )
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
  __D_IMPL(_d,PushButton);
  _d->currentButtonState = _getActiveButtonState();

  if( !_d->buttonStates[ _d->currentButtonState ].background )
  {
    _updateTexture( _d->currentButtonState );
  }

	Widget::beforeDraw( painter  );
}

bool PushButton::isBodyVisible() const { __D_IMPL_CONST(_d,PushButton); return _d->drawBody; }

//! draws the element and its children
void PushButton::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

	__D_IMPL(_d,PushButton);
	// todo:	move sprite up and text down if the pressed state has a sprite
	//			  draw sprites for focused and mouse-over
  const ButtonState& state = _d->buttonStates[ _d->currentButtonState ];

  if( isBodyVisible() )
  {
    if( state.background )
    {
      painter.drawPicture( *state.background, screenLeft(), screenTop(), &absoluteClippingRectRef() );

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
    painter.drawPicture( *state.textPicture, screenLeft(), screenTop(), &absoluteClippingRectRef() );
  }

  drawIcon( painter );

  Widget::draw( painter );
}

void PushButton::drawIcon( GfxEngine& painter )
{
  __D_IMPL(_d,PushButton);
  const ButtonState& bstate = _d->buttonStates[ _d->currentButtonState ];

  const Picture& iconTexture = bstate.iconTexture;

  if( !iconTexture.isValid() )
      return;

  Point pos = convertLocalToScreen( _d->iconRect ).UpperLeftCorner;
  painter.drawPicture( iconTexture, pos + bstate.iconOffset );
}

void PushButton::setText( const std::string& text )
{
	Widget::setText( text );

  _resizeEvent();
}

void PushButton::setFont( const Font& font, ElementState state )
{
  __D_IMPL(_d,PushButton);
  _d->buttonStates[ state ].font = font;
  _updateTexture( state );
}

void PushButton::setFont( const Font& font )
{
  __D_IMPL(_d,PushButton);
  for( int i=0; i != StateCount; i++ )
  {
    _d->buttonStates[ ElementState(i) ].font = font;
    _updateTexture( ElementState(i) );
  }
}

PictureRef& PushButton::_backgroundRef( ElementState state )
{
  __D_IMPL(_d,PushButton);
  return _d->buttonStates[ state ].background;
}

PictureRef& PushButton::_textPictureRef( ElementState state )
{
  __D_IMPL(_d,PushButton);
  return _d->buttonStates[ state ].textPicture;
}

Font PushButton::getFont( ElementState state )
{
  __D_IMPL(_d,PushButton);
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
  __D_IMPL(_d,PushButton);
  _d->bgStyle = style;
  _resizeEvent();
}

void PushButton::setBackgroundStyle(const std::string &strStyle)
{
  BackgroundStyleHelper helper;
  PushButton::BackgroundStyle style = helper.findType( strStyle );
  if( style != noBackground )
  {
    setBackgroundStyle( style );
  }
}

}//end namespace gui
