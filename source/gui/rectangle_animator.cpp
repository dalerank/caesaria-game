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

#include "rectangle_animator.hpp"
#include "environment.hpp"
#include "gfx/engine.hpp"

using namespace gfx;

namespace gui
{

class RectangleAnimator::Impl
{
public:
	Rect startRect;
	Rect stopRect;
	RectF currentRect;
	unsigned int time;

	void restart( Widget* parent );

public signals:
	Signal0<> finishedSignal;
};

RectangleAnimator::~RectangleAnimator(void)
{
}

RectangleAnimator::RectangleAnimator( Widget* parent, 
												const Rect& startRect,
												const Rect& stopRect,
												unsigned int time,
												int flags )
	: WidgetAnimator( parent, flags ),
	  _d( new Impl )			
{
	_d->startRect = startRect;
	_d->stopRect = stopRect;
	_d->time = time;
	_d->restart( parent );
}

Signal0<>& RectangleAnimator::onFinished(){	return _d->finishedSignal;}

void RectangleAnimator::Impl::restart( Widget* parent )
{
  currentRect = startRect.toRectF();
  parent->setGeometry( startRect );
}

void RectangleAnimator::beforeDraw( Engine& painter)
{
	if( enabled() && parent() && isFlag( isActive ) )
	{
		bool mayRemove = true;

		PointF ul( _d->stopRect.left(), _d->stopRect.top() );
		if( !_d->currentRect.UpperLeftCorner.IsEqual( ul, 0.5f ) )
		{
			float step = _d->stopRect.UpperLeftCorner.getDistanceFrom( _d->startRect.UpperLeftCorner ) / float( painter.fps() * (_d->time / 1000.f) );
			float offsetX = _d->stopRect.left() - _d->currentRect.left();
			int signX = offsetX < 0 ? -1 : 1;
			float offsetY = _d->stopRect.top() - _d->currentRect.top();
			int signY = offsetY < 0 ? -1 : 1;

      _d->currentRect.UpperLeftCorner.rx() += signX * std::min<float>( step, fabs( offsetX ) );
      _d->currentRect.UpperLeftCorner.ry() += signY * std::min<float>( step, fabs( offsetY ) );
      mayRemove = false;
    }

		PointF lr( _d->stopRect.right(), _d->stopRect.bottom() );
		if( !_d->currentRect.LowerRightCorner.IsEqual( lr, 0.5f ) )
		{
			float step = _d->stopRect.LowerRightCorner.getDistanceFrom( _d->startRect.LowerRightCorner ) / float( painter.fps() * (_d->time / 1000.f) );
			float offsetX = _d->stopRect.right() - _d->currentRect.right();
			int signX = offsetX < 0 ? -1 : 1;
			float offsetY = _d->stopRect.bottom() - _d->currentRect.bottom();
			int signY = offsetY < 0 ? -1 : 1;

      _d->currentRect.LowerRightCorner.rx() += signX * std::min<float>( step, fabs( offsetX ) );
      _d->currentRect.LowerRightCorner.ry() += signY * std::min<float>( step, fabs( offsetY ) );
      mayRemove = false;
    }

    if( mayRemove )
        updateFinished_();

    parent()->setGeometry( _d->currentRect.toRect() );
  }

  WidgetAnimator::beforeDraw( painter );
}

void RectangleAnimator::updateFinished_()
{
    resetFlag( isActive ); 

    if( !isFlag( debug ) )
    {
        parent()->setVisible( isFlag( showParent ) );

        _afterFinished();
    }
    else
    {
        parent()->setGeometry( _d->startRect );
    }
}

void RectangleAnimator::restart() {	_d->restart( parent() );}
void RectangleAnimator::setStopRect( const Rect& r ){	_d->stopRect = r;}
unsigned int RectangleAnimator::getTime() const{	return _d->time;}
void RectangleAnimator::setTime( unsigned int t ){	_d->time = t;}
void RectangleAnimator::setStartRect( const Rect& r ){	_d->startRect = r;}

}//end namespace gui
