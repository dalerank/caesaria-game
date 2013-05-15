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

#include "oc3_scrollbar.hpp"
#include "oc3_scrollbarprivate.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_event.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_guienv.hpp"

ScrollBar::ScrollBar(  Widget* parent, const Rect& rectangle,
                             bool horizontal, int id, bool noclip)
: Widget( parent, id, rectangle ),
	_dragging(false), _isSliderHovered( false ), _lastSliderHovered( true ), _horizontal( horizontal ),
    _draggedBySlider(false), _trayClick(false),
    _overrideBgColorEnabled( false ), _visibleFilledArea( true ),
    _overrideBgColor( 0 ), _value(0), _sliderPos(0), _lastSliderPos( 0 ),
    _drawLenght(0), _minValue(0), _maxVallue(100),
    _smallStep(10), _largeStep(50), _desiredPos(0), _lastTimeChange(0),
	_d( new Impl )
{
	_d->upButton = 0;
	_d->downButton = 0;

#ifdef _DEBUG
   setDebugName("ScrollBar");
#endif

  refreshControls_();

	setNotClipped(noclip);

	// this element can be tabbed to
	setTabStop(true);
	setTabOrder(-1);

	setPos(0);
}

Signal1<int>& ScrollBar::onPositionChanged()
{
	return _d->onPositionChanged;
}

//! destructor
ScrollBar::~ScrollBar()
{
}

void ScrollBar::_PositionChanged()
{
    getParent()->onEvent( NEvent::Gui( this, 0, OC3_SCROLL_BAR_CHANGED ) );

    _d->onPositionChanged.emit( _value );
}

//! called if an event happened
bool ScrollBar::onEvent(const NEvent& event)
{
	if (isEnabled())
	{
		switch(event.EventType)
		{
		case OC3_KEYBOARD_EVENT:
			if (event.KeyboardEvent.PressedDown)
			{
                const int oldPos = _value;
				bool absorb = true;
				switch (event.KeyboardEvent.Key)
				{
				case KEY_LEFT:
				case KEY_UP:
                    setPos(_value-_smallStep);
					break;
				case KEY_RIGHT:
				case KEY_DOWN:
                    setPos(_value+_smallStep);
					break;
				case KEY_HOME:
                    setPos(_minValue);
					break;
				case KEY_PRIOR:
                    setPos(_value-_largeStep);
					break;
				case KEY_END:
                    setPos(_maxVallue);
					break;
				case KEY_NEXT:
                    setPos(_value+_largeStep);
					break;
				default:
					absorb = false;
				}

                if (_value != oldPos)
				{
					_PositionChanged();
				}
				if (absorb)
					return true;
			}
			break;
    case OC3_GUI_EVENT:
			if (event.GuiEvent.EventType == OC3_BUTTON_CLICKED)
			{
				if (event.GuiEvent.Caller == _d->upButton)
        {
          setPos(_value-_smallStep);
        }
				else if (event.GuiEvent.Caller == _d->downButton)
        {
          setPos(_value+_smallStep);
        }
				
        _PositionChanged();

				return true;
			}
			else
				if (event.GuiEvent.EventType == OC3_ELEMENT_FOCUS_LOST)
				{
					if (event.GuiEvent.Caller == this)
                        _dragging = false;
				}
				break;
		case OC3_MOUSE_EVENT:
			{
        _d->cursorPos =event.MouseEvent.getPosition();
				bool isInside = isPointInside ( _d->cursorPos );
				switch(event.MouseEvent.Event)
				{
				case OC3_MOUSE_WHEEL:
					if( isFocused() )
					{
						// thanks to a bug report by REAPER
						// thanks to tommi by tommi for another bugfix
						// everybody needs a little thanking. hallo niko!;-)
						setPos(	getPos() +
                            ( (int)event.MouseEvent.Wheel * _smallStep * (_horizontal ? 1 : -1 ) )
							);

						_PositionChanged();
						return true;
					}
					break;
				case OC3_LMOUSE_PRESSED_DOWN:
					{
						if (isInside)
						{
              _dragging = true;
              _draggedBySlider = _d->sliderRect.isPointInside( _d->cursorPos );
              _trayClick = !_draggedBySlider;
              _desiredPos = _getPosFromMousePos( _d->cursorPos );
							_environment->setFocus ( this );
							return true;
						}
						break;
					}
				case OC3_LMOUSE_LEFT_UP:
				case OC3_MOUSE_MOVED:
					{
						if ( !event.MouseEvent.isLeftPressed () )
                        {
                            _draggedBySlider = false;
                            _dragging = false;
                        }

                        if ( !_dragging )
							return isInside;

						if ( event.MouseEvent.Event == OC3_LMOUSE_LEFT_UP )
            {
                            _dragging = false;
            }
            
            const int newPos = _getPosFromMousePos( _d->cursorPos );
            const int oldPos = _value;

            if (!_draggedBySlider)
						{
							if ( isInside )
							{
                                _draggedBySlider = _d->sliderRect.isPointInside( _d->cursorPos );
                                _trayClick = !_draggedBySlider;
							}

                            if (_draggedBySlider)
							{
								setPos(newPos);
							}
							else
							{
                _trayClick = false;
								if (event.MouseEvent.Event == OC3_MOUSE_MOVED)
                {
									return isInside;
                }
							}
						}

                        if (_draggedBySlider)
						{
							setPos(newPos);
						}
						else
						{
                            _desiredPos = newPos;
						}

                        if (_value != oldPos )
						{
							_PositionChanged();
						}
						return isInside;
					} break;

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

void ScrollBar::resizeEvent_()
{
	_lastStateNameHash = 0;
}

void ScrollBar::afterPaint( unsigned int timeMs )
{
    if( !isVisible() )
        return;

    if( _dragging && !_draggedBySlider && _trayClick && timeMs > _lastTimeChange + 200 )
	{
        _lastTimeChange = timeMs;

        const int oldPos = _value;

    if (_desiredPos >= _value + _largeStep)
    {
      setPos(_value + _largeStep);
    }
		else if (_desiredPos <= _value - _largeStep)
    {
      setPos(_value - _largeStep);
    }
		else if (_desiredPos >= _value - _largeStep && _desiredPos <= _value + _largeStep)
    {
      setPos(_desiredPos);
    }

    if (_value != oldPos )
		{
      getParent()->onEvent( NEvent::Gui( this, 0, OC3_SCROLL_BAR_CHANGED ) );
		}
	}

  if( /*(timeMs > _lastTimeSliderRectUpdate + 20) && */!_d->currentSliderRect.IsEqual( _d->sliderRect.toRectF(), 0.3f ))
  {
    //_lastTimeSliderRectUpdate = timeMs;
    _d->currentSliderRect = _d->currentSliderRect.relativeTo( _d->sliderRect.toRectF(), 10.f );

    if( _d->currentSliderRect.IsEqual( _d->sliderRect.toRectF(), 0.3f ) )
        _d->currentSliderRect = _d->sliderRect.toRectF();
  }
}

void ScrollBar::beforeDraw( GfxEngine& painter )
{
    if( !isVisible() )
        return;

    bool needRecalculateSliderParams = (_sliderPos != _lastSliderPos);

    if( !(_d->needRecalculateParams || needRecalculateSliderParams) )
        return;

    if( _d->needRecalculateParams )
    {
      _d->backgroundRect = getAbsoluteRect();
      if( _horizontal )
      {
          if( _d->upButton && _d->upButton->isVisible() )
              _d->backgroundRect.UpperLeftCorner += Point( _d->upButton->getWidth(), 0 );

          if( _d->downButton && _d->downButton->isVisible() )
              _d->backgroundRect.LowerRightCorner -= Point( _d->downButton->getWidth(), 0 );
      }
      else
      {
          if( _d->upButton && _d->upButton->isVisible() )
              _d->backgroundRect.UpperLeftCorner += Point( 0, _d->upButton->getHeight() );

          if( _d->downButton && _d->downButton->isVisible() )
              _d->backgroundRect.LowerRightCorner -= Point( 0, _d->downButton->getHeight() );
      }
    }

  _isSliderHovered = _d->sliderRect.isPointInside( _d->cursorPos );
    
	if( _d->sliderTexture.isValid() )
  {
    ElementState st = _dragging && _draggedBySlider
                                ? stPressed
                                : (_lastSliderHovered ? stHovered : stNormal);

    _d->sliderTexture = (st == stPressed) ? _d->sliderPictureDown : _d->sliderPictureUp;
  }

  if( needRecalculateSliderParams )
  {
    _lastSliderPos = _sliderPos;
    _d->sliderRect = getAbsoluteRect();

    if( !math::isEqual( getRange(), 0.f ) )
    {
      // recalculate slider rectangle
      if( _horizontal )
      {
        _d->sliderRect.UpperLeftCorner.setX( getScreenLeft() + _lastSliderPos - _drawLenght/2 );
        if( _d->upButton && _d->upButton->isVisible() )
           _d->sliderRect.UpperLeftCorner += Point( _d->upButton->getWidth(), 0 );
        
        _d->sliderRect.LowerRightCorner.setX( _d->sliderRect.UpperLeftCorner.getX() + _drawLenght );
      }
      else
      {
        _d->sliderRect.UpperLeftCorner.setY( getScreenTop()+ _lastSliderPos - _drawLenght/2 );
        if( _d->upButton && _d->upButton->isVisible() )
            _d->sliderRect.UpperLeftCorner += Point( 0, _d->upButton->getHeight() );

        _d->sliderRect.LowerRightCorner.setY( _d->sliderRect.UpperLeftCorner.getY() + _drawLenght );
      }
    }
  }

  Widget::beforeDraw( painter );
}


//! draws the element and its children
void ScrollBar::draw( GfxEngine& painter )
{
	if (!isVisible())
		return;

  //draw background
  if( _d->texture.isValid() )
  {
      painter.drawPicture( _d->texture, getAbsoluteRect().UpperLeftCorner );
  }

  //draw slider
  Rect curRect( _d->currentSliderRect.toRect() );
  if( _d->sliderTexture.isValid() )
  {
    painter.drawPicture( _d->sliderTexture, curRect.UpperLeftCorner );
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
		w = getWidth() - float(getHeight())*3.0f;
		p = pos.getX() - getScreenLeft() - getHeight()*1.5f;
	}
	else
	{
		w = getHeight() - float(getWidth())*3.0f;
		p = pos.getY() - getScreenTop() - getWidth()*1.5f;
	}
    return (int) ( p/w * getRange() ) + _minValue;
}


//! sets the position of the scrollbar
void ScrollBar::setPos(int pos)
{
  _value = math::clamp( pos, _minValue, _maxVallue );

  const Rect& borderMarginRect = Rect( 0, 0, 0, 0 );
  if (_horizontal)
	{
    _drawLenght = getHeight() * 3;
    int borderMargin = -borderMarginRect.UpperLeftCorner.getX();
    borderMargin -= borderMarginRect.LowerRightCorner.getX();

    float f = ( getWidth() + borderMargin - ( getHeight()*2.0f + _drawLenght)) / getRange();
    _sliderPos = (int)( ( ( _value - _minValue ) * f) + _drawLenght * 0.5f ) + borderMarginRect.UpperLeftCorner.getX();
	}
	else
	{
    _drawLenght = getWidth() * 3;
    int borderMargin = -borderMarginRect.UpperLeftCorner.getY();
    borderMargin -= borderMarginRect.LowerRightCorner.getY();

    float f = ( getHeight() + borderMargin - ( getWidth()*0.0f + _drawLenght)) / getRange();
    _sliderPos = (int)( ( ( _value - _minValue ) * f) + _drawLenght * 0.5f ) + borderMarginRect.UpperLeftCorner.getY();
	}
}


//! gets the small step value
int ScrollBar::getSmallStep() const
{
    return _smallStep;
}


//! sets the small step value
void ScrollBar::setSmallStep(int step)
{
	if (step > 0)
        _smallStep = step;
	else
        _smallStep = 10;
}


//! gets the small step value
int ScrollBar::getLargeStep() const
{
    return _largeStep;
}


//! sets the small step value
void ScrollBar::setLargeStep(int step)
{
	if (step > 0)
        _largeStep = step;
	else
        _largeStep = 50;
}


//! gets the maximum value of the scrollbar.
int ScrollBar::getMax() const
{
    return _maxVallue;
}


//! sets the maximum value of the scrollbar.
void ScrollBar::setMax(int max)
{
  _maxVallue = std::max<int>( max, _minValue );

  bool enable = !math::isEqual( getRange(), 0.f );
	if( _d->upButton )
		_d->upButton->setEnabled(enable);

	if( _d->downButton )
		_d->downButton->setEnabled(enable);

    setPos(_value);
}

//! gets the maximum value of the scrollbar.
int ScrollBar::getMin() const
{
    return _minValue;
}


//! sets the minimum value of the scrollbar.
void ScrollBar::setMin(int min)
{
  _minValue = std::min<int>( min, _maxVallue );

  bool enable = !math::isEqual( getRange(), 0.f );

	if( _d->upButton )
		_d->upButton->setEnabled(enable);

	if( _d->downButton)
		_d->downButton->setEnabled(enable);

  setPos(_value);
}


//! gets the current position of the scrollbar
int ScrollBar::getPos() const
{
    return _value;
}

PushButton* ScrollBar::_CreateButton( const Rect& rectangle, 
                                      TypeAlign left, TypeAlign rigth, TypeAlign top, TypeAlign bottom )
{
    PushButton* btn = new PushButton( this, rectangle );
    btn->setSubElement(true);
    btn->setTabStop(false);
    btn->setAlignment(left, rigth, top, bottom );

    return btn;
}

//! refreshes the position and text on child buttons
void ScrollBar::refreshControls_()
{
  if (_horizontal)
	{
		int h = getHeight();
		if( !_d->upButton )
         _d->upButton = _CreateButton( Rect(0, 0, h, h), alignUpperLeft, alignUpperLeft, alignUpperLeft, alignLowerRight );

    if (!_d->downButton)
         _d->downButton = _CreateButton( Rect( getWidth()-h, 0, getWidth(), h), 
                                         alignLowerRight, alignLowerRight, alignUpperLeft, alignLowerRight );
	}
	else
	{
		int w = getWidth();
		if (!_d->upButton)
            _d->upButton = _CreateButton( Rect(0,0, w, w), alignUpperLeft, alignLowerRight, alignUpperLeft, alignUpperLeft );

        if (!_d->downButton)
            _d->downButton = _CreateButton( Rect(0, getHeight()-w, w, getHeight()), 
                                             alignUpperLeft, alignLowerRight, alignLowerRight, alignLowerRight );
	}
}


//! Writes attributes of the element.
void ScrollBar::save( VariantMap& out ) const
{
// 	Widget::save( out );
// 
//     out->AddBool( SerializeHelper::horizontalProp, _horizontal);
//     out->AddInt ( SerializeHelper::valueProp,		_value);
//     out->AddVector2d( SerializeHelper::intervalProp, core::Vector2F( (float)_minValue, (float)_maxVallue ) );
//     out->AddInt ( smallStepProp, _smallStep);
//     out->AddInt ( largeStepProp, _largeStep);
//     out->AddBool( filledAreaVisibleProp, _visibleFilledArea );
}


//! Reads attributes of the element
void ScrollBar::load( const VariantMap& in )
{
// 	Widget::load( in );
// 
//     _horizontal = in->getAttributeAsBool( SerializeHelper::horizontalProp );
//     setMin( (int)in->getAttributeAsVector2d( SerializeHelper::intervalProp ).X );
// 	setMax( (int)in->getAttributeAsVector2d( SerializeHelper::intervalProp ).Y );
//     setPos( in->getAttributeAsInt( SerializeHelper::valueProp ));
// 	setSmallStep( in->getAttributeAsInt( smallStepProp ) );
// 	setLargeStep(in->getAttributeAsInt( largeStepProp ));
//     setVisibleFilledArea( in->getAttributeAsBool( filledAreaVisibleProp ) );
// 
//     refreshControls_();
}

void ScrollBar::setBackgroundImage( const Picture& pixmap )
{
	_d->texture = pixmap;
  _d->textureRect = Rect( Point(0,0), pixmap.getSize() );
}

void ScrollBar::setSliderImage( const Picture& pixmap )
{
	_d->sliderTexture = pixmap;
	_d->sliderTextureRect = Rect( Point(0,0), pixmap.getSize() );
}

void ScrollBar::setHorizontal( bool horizontal )
{
    _horizontal = horizontal;
}

PushButton* ScrollBar::getUpButton()
{
    return _d->upButton;
}

PushButton* ScrollBar::getDownButton()
{
    return _d->downButton;
}

void ScrollBar::setVisibleFilledArea( bool vis )
{
    _visibleFilledArea = vis;
}

float ScrollBar::getRange() const
{
	return (float) ( _maxVallue - _minValue );
}
