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

#ifndef _CAESARIA_SCROLLBAR_INCLUDE_
#define _CAESARIA_SCROLLBAR_INCLUDE_

#include "gui/widget.hpp"
#include "core/predefinitions.hpp"
#include "core/signals.hpp"
#include "core/scopedptr.hpp"


namespace gfx
{
	class Picture;
}

namespace gui
{

class PushButton;

class ScrollBar : public Widget
{
public:
	enum { Vertical=false, Horizontal=true };
	//! constructor
	ScrollBar( Widget* parent, const Rect& rectangle, bool horizontal=false, int id=-1,
	           bool noclip=false);

	//! destructor
	virtual ~ScrollBar();

	virtual void beforeDraw( gfx::Engine& painter );

	//! called if an event happened.
	virtual bool onEvent(const NEvent& event);

	//! draws the element and its children
	virtual void draw( gfx::Engine& painter );

	virtual void afterPaint(unsigned int timeMs);

	//! gets the maximum value of the scrollbar.
	virtual int maxValue() const;

	//! sets the maximum value of the scrollbar.
	virtual void setMaxValue(int max);

	//! gets the minimum value of the scrollbar.
	virtual int minValue() const;

	//! sets the minimum value of the scrollbar.
	virtual void setMinValue(int max);

	//! gets the small step value
  virtual int smallStep() const;

	//! sets the small step value
	virtual void setSmallStep(int step);

	//! gets the large step value
  virtual int largeStep() const;

	//! sets the large step value
	virtual void setLargeStep(int step);

	//! gets the current position of the scrollbar
  virtual int value() const;

	//! sets the position of the scrollbar
  virtual void setValue(int pos);

	//! sets the texture which draw
	virtual void setBackgroundImage( const gfx::Picture& pixmap );

	//! gets the UpButton
	virtual PushButton* upButton();

	virtual PushButton* downButton();

	virtual void setSliderImage( const gfx::Picture& pixmap, const ElementState state );

  virtual void setHorizontal( bool horizontal );
  virtual void setVisibleFilledArea( bool vis );

	float getRange () const;

public signals:
	Signal1<int>& onPositionChanged();

protected:
	void _resizeEvent();
	void _refreshControls();

  virtual int _getPosFromMousePos(const Point& p) const;

  bool _dragging;
  bool _isSliderHovered, _lastSliderHovered;
  bool _horizontal;
  bool _draggedBySlider;
  bool _trayClick;
  bool _overrideBgColorEnabled, _visibleFilledArea;

  unsigned int _lastStateNameHash;

  int _overrideBgColor;
  int _value;
  int _sliderPos, _lastSliderPos;
	int _drawLenght;
  int _minValue;
  int _maxVallue;
  int _smallStep;
  int _largeStep;
  int _desiredPos;

	class Impl;
	ScopedPtr< Impl > _d;

  void _resolvePositionChanged();
  PushButton* _createButton( const Rect& rectangle,
                             Alignment left, Alignment rigth, Alignment top, Alignment bottom, int type );
};

}//end namesapce gui

#endif //_CAESARIA_SCROLLBAR_INCLUDE_
