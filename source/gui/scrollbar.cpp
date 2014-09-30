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

#include "scrollbar.hpp"
#include "scrollbarprivate.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "gfx/engine.hpp"
#include "gui/environment.hpp"
#include "game/resourcegroup.hpp"

using namespace gfx;

namespace gui
{

ScrollBar::ScrollBar(  Widget* parent, const Rect& rectangle,
                             bool horizontal, int id, bool noclip)
: Widget( parent, id, rectangle ),
	_dragging(false), _isSliderHovered( false ), _lastSliderHovered( true ), _horizontal( horizontal ),
    _draggedBySlider(false), _trayClick(false),
    _overrideBgColorEnabled( false ), _visibleFilledArea( true ),
    _overrideBgColor( 0 ), _value(0), _sliderPos(0), _lastSliderPos( 0 ),
    _drawLenght(0), _minValue(0), _maxVallue(100),
    _smallStep(10), _largeStep(50), _desiredPos(0),
	_d( new Impl )
{
	_d->upButton = 0;
	_d->downButton = 0;
	_d->lastTimeChange = 0l;

  _d->sliderPictureUp = Picture::load( ResourceGroup::panelBackground, 61 );
  _d->sliderPictureDown = Picture::load( ResourceGroup::panelBackground, 53 );

#ifdef _DEBUG
   setDebugName("ScrollBar");
#endif

	_refreshControls();

	setNotClipped(noclip);

	// this element can be tabbed to
	setTabStop(true);
	setTabOrder(-1);

  setValue(0);
}

Signal1<int>& ScrollBar::onPositionChanged() {	return _d->onPositionChanged; }

//! destructor
ScrollBar::~ScrollBar(){}

void ScrollBar::_resolvePositionChanged()
{
  parent()->onEvent( NEvent::Gui( this, 0, guiScrollbarChanged ) );
  emit _d->onPositionChanged( _value );
}

//! called if an event happened
bool ScrollBar::onEvent(const NEvent& event)
{
	if (enabled())
	{
		switch(event.EventType)
		{
		case sEventKeyboard:
			if (event.keyboard.pressed)
			{
                const int oldPos = _value;
				bool absorb = true;
				switch (event.keyboard.key)
				{
        case KEY_LEFT: case KEY_UP: setValue(_value-_smallStep); break;
        case KEY_RIGHT:	case KEY_DOWN: setValue(_value+_smallStep); break;
        case KEY_HOME: setValue(_minValue);	break;
        case KEY_PRIOR: setValue(_value-_largeStep); break;
        case KEY_END: setValue(_maxVallue);	break;
        case KEY_NEXT: setValue(_value+_largeStep); break;
				default:absorb = false;
				}

				if (_value != oldPos)
				{
					_resolvePositionChanged();
				}
				if (absorb)
					return true;
			}
			break;
		case sEventGui:
			if (event.gui.type == guiButtonClicked)
			{
				if (event.gui.caller == _d->upButton)
        {
          setValue(_value-_smallStep);
        }
				else if (event.gui.caller == _d->downButton)
        {
          setValue(_value+_smallStep);
        }
				
        _resolvePositionChanged();

				return true;
			}
			else
				if (event.gui.type == guiElementFocusLost)
				{
					if (event.gui.caller == this)
                        _dragging = false;
				}
				break;
		case sEventMouse:
			{
				_d->cursorPos =event.mouse.pos();
				bool isInside = isPointInside ( _d->cursorPos );
				switch(event.mouse.type)
				{
				case mouseWheel:
					if( isFocused() )
					{
            setValue(	value() +
										( (int)event.mouse.wheel * _smallStep * (_horizontal ? 1 : -1 ) )	);

						_resolvePositionChanged();
						return true;
					}
					break;
				case mouseLbtnPressed:
					{
						if (isInside)
						{
              _dragging = true;
              _draggedBySlider = _d->sliderRect.isPointInside( _d->cursorPos - absoluteRect().lefttop() );
              _trayClick = !_draggedBySlider;
              _desiredPos = _getPosFromMousePos( _d->cursorPos );
							_environment->setFocus ( this );
							return true;
						}
					}
				break;

				case mouseLbtnRelease:
				case mouseMoved:
					{
						if ( !event.mouse.isLeftPressed () )
						{
							_draggedBySlider = false;
							_dragging = false;
							_d->needRecalculateParams = true;
						}

						if ( !_dragging )
							return isInside;

            const int newPos = _getPosFromMousePos( _d->cursorPos );
            const int oldPos = _value;

            if (!_draggedBySlider)
						{
							if ( isInside )
							{
                _draggedBySlider = _d->sliderRect.isPointInside( _d->cursorPos - absoluteRect().lefttop() );
                _trayClick = !_draggedBySlider;
							}

              if( _draggedBySlider )
							{
                setValue(newPos);
							}
							else
							{
                _trayClick = false;
                if (event.mouse.type == mouseMoved)
                {
									return isInside;
                }
							}
						}

						if (_draggedBySlider)
						{
              setValue(newPos);
						}
						else
						{
							_desiredPos = newPos;
						}

						if (_value != oldPos )
						{
							_resolvePositionChanged();
						}
						return isInside;
					}
				break;

				default:
					break;
				}
			} break;
		default:
			break;
		}
	}

	return Widget::onEvent(event);
}

void ScrollBar::_resizeEvent()
{
	_lastStateNameHash = 0;
}

void ScrollBar::afterPaint( unsigned int timeMs )
{
  if( !visible() )
      return;

	if( _dragging && !_draggedBySlider && _trayClick && timeMs > _d->lastTimeChange + 200 )
	{
		_d->lastTimeChange = timeMs;

    const int oldPos = _value;

    if (_desiredPos >= _value + _largeStep)
    {
      setValue(_value + _largeStep);
    }
		else if (_desiredPos <= _value - _largeStep)
    {
      setValue(_value - _largeStep);
    }
		else if (_desiredPos >= _value - _largeStep && _desiredPos <= _value + _largeStep)
    {
      setValue(_desiredPos);
    }

    if (_value != oldPos )
		{
			parent()->onEvent( NEvent::Gui( this, 0, guiScrollbarChanged ) );
		}
	}
}

void ScrollBar::beforeDraw(gfx::Engine& painter )
{
  if( !visible() )
      return;

  bool needRecalculateSliderParams = (_sliderPos != _lastSliderPos);

  if( !(_d->needRecalculateParams || needRecalculateSliderParams) )
      return;

  if( _d->needRecalculateParams )
  {
    _d->backgroundRect = absoluteRect();
    if( _horizontal )
    {
        if( _d->upButton && _d->upButton->visible() )
            _d->backgroundRect.UpperLeftCorner += Point( _d->upButton->width(), 0 );

        if( _d->downButton && _d->downButton->visible() )
            _d->backgroundRect.LowerRightCorner -= Point( _d->downButton->width(), 0 );
    }
    else
    {
        if( _d->upButton && _d->upButton->visible() )
            _d->backgroundRect.UpperLeftCorner += Point( 0, _d->upButton->height() );

        if( _d->downButton && _d->downButton->visible() )
            _d->backgroundRect.LowerRightCorner -= Point( 0, _d->downButton->height() );
    }
  }

  _isSliderHovered = _d->sliderRect.isPointInside( _d->cursorPos - absoluteRect().lefttop()  );
  ElementState st = _dragging && _draggedBySlider
                                ? stPressed
                                : (_lastSliderHovered ? stHovered : stNormal);

  _d->sliderTexture = (st == stPressed) ? _d->sliderPictureDown : _d->sliderPictureUp;
 
  if( needRecalculateSliderParams )
  {
    _lastSliderPos = _sliderPos;
    _d->sliderRect = absoluteRect();

    if( !math::isEqual( getRange(), 0.f ) )
    {
      // recalculate slider rectangle
      if( _horizontal )
      {
        _d->sliderRect.UpperLeftCorner.setX( screenLeft() + _lastSliderPos - _drawLenght/2 );
        if( _d->upButton && _d->upButton->visible() )
           _d->sliderRect.UpperLeftCorner += Point( _d->upButton->width(), 0 );
        
        _d->sliderRect.LowerRightCorner.setX( _d->sliderRect.left() + _drawLenght );
      }
      else
      {
        _d->sliderRect.UpperLeftCorner = Point( screenLeft() + (width() - _d->sliderTexture.width()) / 2,
                                                screenTop() + _lastSliderPos - _drawLenght/2 );
        if( _d->upButton && _d->upButton->visible() )
            _d->sliderRect.UpperLeftCorner += Point( 0, _d->upButton->height() );

        _d->sliderRect.LowerRightCorner.setY( _d->sliderRect.top() + _drawLenght );
      }
    }

    _d->sliderRect -= absoluteRect().lefttop();
  }

  Widget::beforeDraw( painter );
}


//! draws the element and its children
void ScrollBar::draw(gfx::Engine& painter )
{
	if (!visible())
		return;

  //draw background
  if( _d->texture.isValid() )
  {
    painter.draw( _d->texture, absoluteRect().lefttop(), &absoluteClippingRectRef() );
  }

  //draw slider
  if( _d->sliderTexture.isValid() )
  {
    painter.draw( _d->sliderTexture, absoluteRect().lefttop() + _d->sliderRect.lefttop(), &absoluteClippingRectRef() );
  }

	// draw buttons
	Widget::draw( painter );
}

//!
int ScrollBar::_getPosFromMousePos(const Point& pos) const
{
	float w, p;
    if (_horizontal)
	{
		w = width() - float(height())*3.0f;
		p = pos.x() - screenLeft() - height()*1.5f;
	}
	else
	{
		w = height() - float(width())*3.0f;
		p = pos.y() - screenTop() - width()*1.5f;
	}
	return (int) ( p/w * getRange() ) + _minValue;
}


//! sets the position of the scrollbar
void ScrollBar::setValue(int pos)
{
  _value = math::clamp( pos, _minValue, _maxVallue );

  const Rect& borderMarginRect = Rect( 0, 0, 0, 0 );
  if( _horizontal )
	{
    _drawLenght = height() * 3;
    int borderMargin = -borderMarginRect.UpperLeftCorner.x();
    borderMargin -= borderMarginRect.LowerRightCorner.x();

    float f = ( width() + borderMargin - ( height()*2.0f + _drawLenght)) / getRange();
    _sliderPos = (int)( ( ( _value - _minValue ) * f) + _drawLenght * 0.5f ) + borderMarginRect.UpperLeftCorner.x();
	}
	else
	{
		int top = _d->upButton->visible() ? _d->upButton->height() : 0;
		_drawLenght = top	+ ( _d->downButton->visible() ? _d->downButton->height() : 0 );
		int borderMargin = -borderMarginRect.top() - borderMarginRect.bottom();
		int sliderHeight = _d->sliderTexture.height();

    float f = ( height() + borderMargin - _drawLenght - sliderHeight) / getRange();
    _sliderPos = top + (int)( ( ( _value - _minValue ) * f) ) + borderMarginRect.top();
	}
}


//! gets the small step value
int ScrollBar::smallStep() const {    return _smallStep;}

//! sets the small step value
void ScrollBar::setSmallStep(int step) {  _smallStep = step > 0 ? step : 10;}

//! gets the small step value
int ScrollBar::largeStep() const {    return _largeStep;}

//! sets the small step value
 void ScrollBar::setLargeStep(int step){  _largeStep = step > 0 ? step : 50;}

// ! gets the maximum value of the scrollbar.
int ScrollBar::maxValue() const{  return _maxVallue;}


//! sets the maximum value of the scrollbar.
void ScrollBar::setMaxValue(int max)
{
  _maxVallue = std::max<int>( max, _minValue );

  bool enable = !math::isEqual( getRange(), 0.f );
	if( _d->upButton )
		_d->upButton->setEnabled(enable);

	if( _d->downButton )
		_d->downButton->setEnabled(enable);

  setValue(_value);
}

//! gets the maximum value of the scrollbar.
int ScrollBar::minValue() const {    return _minValue;}

//! sets the minimum value of the scrollbar.
void ScrollBar::setMinValue(int min)
{
  _minValue = std::min<int>( min, _maxVallue );

  bool enable = !math::isEqual( getRange(), 0.f );

	if( _d->upButton )
		_d->upButton->setEnabled(enable);

	if( _d->downButton)
		_d->downButton->setEnabled(enable);

  setValue(_value);
}


//! gets the current position of the scrollbar
int ScrollBar::value() const { return _value;}

PushButton* ScrollBar::_createButton( const Rect& rectangle,
                                      Alignment left, Alignment rigth, Alignment top, Alignment bottom, int type )
{
    PushButton* btn = new PushButton( this, rectangle );
    btn->setSubElement(true);
    btn->setTabStop(false);
    btn->setAlignment(left, rigth, top, bottom );

    switch( type )
    {
    case 0: 
      btn->setPicture( Picture::load( ResourceGroup::panelBackground, 247 ), stNormal );
      btn->setPicture( Picture::load( ResourceGroup::panelBackground, 248 ), stHovered );
      btn->setPicture( Picture::load( ResourceGroup::panelBackground, 249 ), stPressed );
      btn->setPicture( Picture::load( ResourceGroup::panelBackground, 250 ), stDisabled );
    break;

    case 1: 
      btn->setPicture( Picture::load( ResourceGroup::panelBackground, 251 ), stNormal );
      btn->setPicture( Picture::load( ResourceGroup::panelBackground, 252 ), stHovered );
      btn->setPicture( Picture::load( ResourceGroup::panelBackground, 253 ), stPressed );
      btn->setPicture( Picture::load( ResourceGroup::panelBackground, 254 ), stDisabled );
    break;
    }

    return btn;
}

//! refreshes the position and text on child buttons
void ScrollBar::_refreshControls()
{
  if (_horizontal)
	{
		int h = height();
		if( !_d->upButton )
			 _d->upButton = _createButton( Rect(0, 0, h, h), align::upperLeft, align::upperLeft, align::upperLeft, align::lowerRight, 2 );

    if (!_d->downButton)
         _d->downButton = _createButton( Rect( width()-h, 0, width(), h),
                                         align::lowerRight, align::lowerRight, align::upperLeft, align::lowerRight, 3 );
	}
	else
	{
	  //int w = getWidth();
		if (!_d->upButton)
    {
      _d->upButton = _createButton( Rect(0,0, 39, 26), align::upperLeft, align::lowerRight, align::upperLeft, align::upperLeft, 0 );
    }

    if (!_d->downButton)
    {
      _d->downButton = _createButton( Rect(0, height()-26, 39, height()),
                                      align::upperLeft, align::lowerRight, align::lowerRight, align::lowerRight, 1 );
    }
	}
}

void ScrollBar::setBackgroundImage( const Picture& pixmap )
{
	_d->texture = pixmap;
  _d->textureRect = Rect( Point(0,0), pixmap.size() );
}

void ScrollBar::setSliderImage( const Picture& pixmap, const ElementState state )
{
  (state == stNormal ? _d->sliderPictureUp : _d->sliderPictureDown ) = pixmap;
	_d->sliderTextureRect = Rect( Point(0,0), pixmap.size() );
}

void ScrollBar::setHorizontal( bool horizontal ) {    _horizontal = horizontal;}
PushButton* ScrollBar::upButton(){    return _d->upButton;}
PushButton* ScrollBar::downButton(){    return _d->downButton;}
void ScrollBar::setVisibleFilledArea( bool vis ){    _visibleFilledArea = vis;}
float ScrollBar::getRange() const{	return (float) ( _maxVallue - _minValue );}

}//end namespace gui
