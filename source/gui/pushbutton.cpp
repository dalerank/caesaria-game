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

#include "pushbutton.hpp"
#include "core/event.hpp"
#include "core/time.hpp"
#include "core/variant_map.hpp"
#include "environment.hpp"
#include "gfx/decorator.hpp"
#include "gfx/engine.hpp"
#include "core/color.hpp"
#include "core/logger.hpp"
#include "core/variant_list.hpp"
#include "gfx/picturesarray.hpp"
#include "widget_factory.hpp"

using namespace gfx;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(PushButton)

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
    append( PushButton::flatBorderLine, "flatBorderLine" );
    append( PushButton::noBackground, "noBackground" );
  }
};

struct ButtonState
{
  Picture background;
  Pictures styleNb;
  Batch style;
  Picture icon;
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
  bool drawText;
  Picture textPicture;
  Point textOffset;
  PushButton::BackgroundStyle bgStyle;
  bool needUpdateTextPic;
  bool needUpdateBackground;

  ElementState currentButtonState, lastButtonState;
  ButtonState buttonStates[ StateCount ];

signals public:
  Signal0<> onClickedSignal;
  Signal1<Widget*> onClickedExSignal;

public:

  Impl() : pressed(false),
      isPushButton(false), drawBody( true ),
      clickTime(0)
  {
    for( int i=0; i < StateCount; i++)
    {
      buttonStates[ ElementState(i) ].style.destroy();
      buttonStates[ ElementState(i) ].icon = Picture::getInvalid();
      buttonStates[ ElementState(i) ].background = Picture::getInvalid();
      buttonStates[ ElementState(i) ].font = Font::create( FONT_2 );
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
  _d->drawText = true;
  _d->bgStyle = greyBorderLine;
  _d->needUpdateBackground = true;
  setTextAlignment( align::center, align::center );
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
  setDebugName( CAESARIA_STR_EXT(PushButton) );

  _d->pressed = false;
  _d->currentButtonState = stNormal;
  _d->lastButtonState = StateCount;
  _d->drawText = true;
  setBackgroundStyle( bgStyle );
  setTextAlignment( align::center, align::center );

  setText( caption );
  setNotClipped(noclip);
}

void PushButton::_updateTextPic()
{
  __D_IMPL(_d,PushButton)
  ElementState state = _state();
  Picture& textTxs = _d->textPicture;

  if( textTxs.isValid() )
  {
    textTxs = Picture( size(), 0, true );
  }

  if( textTxs.size() != size() )
  {
    textTxs = Picture( size(), 0, true );
  }

  Font stFont = _d->buttonStates[ state ].font;
  if( textTxs.isValid() && stFont.isValid() )
  {
    Rect textRect = stFont.getTextRect( text(), Rect( 0, 0, width(), height() ),
                                              horizontalTextAlign(), verticalTextAlign() );
    textTxs.fill( 0x00ffffff, Rect( 0, 0, 0, 0 ) );
    stFont.draw( textTxs, text(), textRect.lefttop() + _d->textOffset, true, false );
  }

  if( _d->bgStyle == flatBorderLine )
  {
    Decorator::drawLine( textTxs, Point( 0, 0), Point( width(), 0), DefaultColors::black );
    Decorator::drawLine( textTxs, Point( width()-1, 0), Point( width()-1, height() ), DefaultColors::black );
    Decorator::drawLine( textTxs, Point( width(), height()-1), Point( 0, height()-1), DefaultColors::black );
    Decorator::drawLine( textTxs, Point( 0, height() ), Point( 0, 0), DefaultColors::black );
  }

  textTxs.update();
}

void PushButton::_updateBackground( ElementState state )
{
  __D_IMPL(_d,PushButton)     

  // draw button background
  Decorator::Mode mode = Decorator::pure;
  Pictures pics;
  if( !_d->buttonStates[ state ].background.isValid() )
  {    
    switch( _d->bgStyle )
    {
    case greyBorderLineSmall:
    {
      mode = (state == stNormal || state == stDisabled)
                                      ? Decorator::greyPanelSmall
                                      : Decorator::brownPanelSmall;
    }
    break;

    case greyBorderLineBig:
    {
       mode = (state == stNormal || state == stDisabled)
                                        ? Decorator::greyPanelBig
                                        : Decorator::lightgreyPanelBig;
    }
    break;

    case greyBorderLine:
    {
      mode = (state == stNormal || state == stDisabled)
                                        ? Decorator::lightgreyPanel
                                        : Decorator::greyPanel;
    }
    break;

    case whiteBorderUp:
    {
      Decorator::draw( pics, Rect( Point( 0, 0 ), size() ), Decorator::whiteArea);
      Decorator::draw( pics, Rect( Point( 0, 0 ), size() ),
                       ( state == stHovered || state == stPressed ) ? Decorator::brownBorder : Decorator::whiteBorderA );
    }
    break;

    case flatBorderLine:
    {
      Decorator::draw( pics, Rect( Point( 0, 0 ), size() ), (state == stHovered || state == stPressed)
                                                                       ? Decorator::blackArea : Decorator::whiteArea );
    }
    break;

    case blackBorderUp:
    {
      Decorator::draw( pics, Rect( Point( 0, 0 ), size() ), Decorator::blackArea );
      Decorator::draw( pics, Rect( Point( 0, 0 ), size() ),
                       state == stHovered ? Decorator::brownBorder : Decorator::whiteBorderA );
    }
    break;

    case noBackground: mode = Decorator::pure; break;
    }
  }

  Decorator::draw( pics, Rect( Point( 0, 0 ), size() ), mode, Decorator::normalY );
  _d->buttonStates[ state ].style.destroy();
  bool batchOk = _d->buttonStates[ state ].style.load( pics, absoluteRect().lefttop() );

  if( !batchOk )
  {
    _d->buttonStates[ state ].style.destroy();
    Decorator::reverseYoffset( pics );
    _d->buttonStates[ state ].styleNb = pics;
  }
}

void PushButton::_updateStyle()
{
  for( int i=0; i != StateCount; i++ )
  {
    _updateBackground( ElementState(i) );
  }
}

//! destructor
PushButton::~PushButton(){}
void PushButton::setIsPushButton( bool value ){  _dfunc()->isPushButton = value; }

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

  setIsPushButton( (bool)ui.get( "pushbutton" ) );
  _d->textOffset = ui.get( "textOffset" );
  _d->textOffset = ui.get( "text.offset", _d->textOffset );
  setEnabled( (bool)ui.get( "enabled", true ) );

  Variant vFont = ui.get( "font" );
  if( vFont.isValid() ) { setFont( Font::create( vFont.toString() ) ); }

  VariantList vlist = ui.get( "normal" ).toList();
  if( !vlist.empty() ) setPicture( vlist.get( 0 ).toString(), vlist.get( 1 ).toInt(), stNormal );

  vlist = ui.get( "hovered" ).toList();
  if( !vlist.empty() ) setPicture( vlist.get( 0 ).toString(), vlist.get( 1 ).toInt(), stHovered  );

  vlist = ui.get( "pressed" ).toList();
  if( !vlist.empty() ) setPicture( vlist.get( 0 ).toString(), vlist.get( 1 ).toInt(), stPressed );

  vlist = ui.get( "disabled" ).toList();
  if( !vlist.empty() ) setPicture( vlist.get( 0 ).toString(), vlist.get( 1 ).toInt(), stDisabled );
}

void PushButton::setTextOffset(const Point& offset) { _dfunc()->textOffset = offset;}
bool PushButton::isPushButton() const { return _dfunc()->isPushButton; }

void PushButton::setPicture(Picture picture, ElementState state )
{
  __D_IMPL(_d,PushButton);
  Rect rectangle( Point(0,0), picture.size() );

  _d->buttonStates[ state ].background = picture;
  _d->buttonStates[ state ].rectangle = rectangle;

  _updateBackground( state );
}

void PushButton::setPicture(const std::string& rcname, int index, ElementState state)
{
  setPicture( Picture( rcname, index ), state );
}

void PushButton::setIcon( const std::string& rcname, int index, ElementState state)
{
  _dfunc()->buttonStates[ state ].icon.load( rcname, index );
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
  Picture pic( rcname, index );
  for( int i=stNormal; i < StateCount; i++ )
  {
    _d->buttonStates[ i ].icon = pic;
  }
}

void PushButton::setPicture( const std::string& rcname, int index )
{
  Picture pic( rcname, index );
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

bool PushButton::isPressed() const {  return _dfunc()->pressed; }

//! called if an event happened.
bool PushButton::onEvent(const NEvent& event)
{
  if( !enabled() )
  {
    return parent()->onEvent(event);
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
    case mouseMoved:     break;
    case mouseLbtnPressed:        return _leftMouseBtnPressed( event );
    case mouseLbtnRelease:        return _btnMouseUp( event );
    default:
    break;
    }
  break;

  default:
  break;
  }

  return parent() ? parent()->onEvent(event) : false;
}

void PushButton::_btnClicked()
{
  parent()->onEvent( NEvent::Gui( this, 0, guiButtonClicked ) );

  emit _dfunc()->onClickedSignal();
  emit _dfunc()->onClickedExSignal( this );
}

Signal0<>& PushButton::onClicked() { return _dfunc()->onClickedSignal; }
Signal1<Widget*>& PushButton::onClickedEx() { return _dfunc()->onClickedExSignal; }

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

    if((!isPushButton() && wasPressed ) ||
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

  if( !isPushButton() )
  {
    setPressed(true);
  }

  setFocus();
  return true;
}

ElementState PushButton::_state()
{
  if( enabled() )
      return ( isPressed()
                  ? stPressed
                  : ( isHovered() ? stHovered : stNormal) );

  return stDisabled;
}

void PushButton::beforeDraw( gfx::Engine& painter )
{
  // todo:	move sprite up and text down if the pressed state has a sprite
  // draw sprites for focused and mouse-over
  // Point spritePos = AbsoluteRect.getCenter();
  __D_IMPL(_d,PushButton);

	_d->currentButtonState = _state();
  if( _d->needUpdateBackground )
  {
    _updateStyle();
    _d->needUpdateBackground = false;
  }

	if( _d->needUpdateTextPic )
	{    
		_updateTextPic();
		_d->needUpdateTextPic = false;
	}

	Widget::beforeDraw( painter  );
}

bool PushButton::isBodyVisible() const { return _dfunc()->drawBody; }
bool gui::PushButton::isTextVisible() const { return _dfunc()->drawText; }
void gui::PushButton::setTextVisible(bool value) { _dfunc()->drawText = value; }

//! draws the element and its children
void PushButton::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

	__D_IMPL(_d,PushButton);
	// todo:	move sprite up and text down if the pressed state has a sprite
	//			  draw sprites for focused and mouse-over
  const ButtonState& state = _d->buttonStates[ _d->currentButtonState ];

  if( isBodyVisible() )
  {
    if( state.background.isValid() )
      painter.draw( state.background, absoluteRect().lefttop(), &absoluteClippingRectRef() );
    else
    {
      if( state.style.valid() )
        painter.draw( state.style, &absoluteClippingRectRef());
      else
        painter.draw( state.styleNb, absoluteRect().lefttop(), &absoluteClippingRectRef());
    }
	}

  if( _d->drawText && _d->textPicture.isValid() )
  {
    painter.draw( _d->textPicture, screenLeft(), screenTop(), &absoluteClippingRectRef() );
  }

#ifdef DEBUG
  if( ui()->hasFlag( Ui::buttonShowDebugArea ) )
  {
    painter.drawLine( DefaultColors::red, absoluteRect().lefttop(), absoluteRect().righttop() );
    painter.drawLine( DefaultColors::red, absoluteRect().righttop(), absoluteRect().rightbottom() );
    painter.drawLine( DefaultColors::red, absoluteRect().rightbottom(), absoluteRect().leftbottom() );
    painter.drawLine( DefaultColors::red, absoluteRect().leftbottom(), absoluteRect().lefttop() );

    if( _d->currentButtonState == stPressed )
    {
      painter.drawLine( DefaultColors::red, absoluteRect().lefttop(), absoluteRect().rightbottom() );
      painter.drawLine( DefaultColors::red, absoluteRect().leftbottom(), absoluteRect().righttop() );
    }
  }
#endif

  drawIcon( painter );

  Widget::draw( painter );
}

void PushButton::drawIcon( gfx::Engine& painter )
{
  __D_IMPL(_d,PushButton);
  const ButtonState& bstate = _d->buttonStates[ _d->currentButtonState ];

  const Picture& iconTexture = bstate.icon;

  if( !iconTexture.isValid() )
      return;

  Point pos = localToScreen( _d->iconRect ).lefttop();
  painter.draw( iconTexture, pos + bstate.iconOffset );
}

void PushButton::setText( const std::string& text )
{
  Widget::setText( text );
  _dfunc()->needUpdateTextPic = true;
}

void PushButton::setFont( const Font& font, ElementState state )
{
  _dfunc()->buttonStates[ state ].font = font;
  _dfunc()->needUpdateTextPic = true;
}

void PushButton::setFont( const Font& font )
{
  for( int i=0; i != StateCount; i++ )
  {
    _dfunc()->buttonStates[ ElementState(i) ].font = font;
  }

  _dfunc()->needUpdateTextPic = true;
}

Picture& PushButton::_textPicture() { return _dfunc()->textPicture; }
Font PushButton::font( ElementState state ) {  return _dfunc()->buttonStates[ state ].font;}

void PushButton::_finalizeResize()
{
  _updateStyle();
  _dfunc()->needUpdateTextPic = true;
}

void PushButton::setBackgroundStyle( const BackgroundStyle style )
{
  _dfunc()->bgStyle = style;
  _dfunc()->needUpdateBackground = true;
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
