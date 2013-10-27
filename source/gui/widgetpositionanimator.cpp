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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#include "widgetpositionanimator.hpp"
#include "core/time.hpp"

namespace gui
{

class PositionAnimator::Impl
{
public:
	Point stopPos, startPos;
	PointF currentPos;
	int time;
    int lastTimeUpdate;
};

PositionAnimator::PositionAnimator( Widget* node, 
                                    int flags,
								    const Point& stopPos,
									int time )
	: WidgetAnimator( node, flags ), _d( new Impl )
{
  _OC3_DEBUG_BREAK_IF( !node && "parent must be exist" );

	_d->stopPos = stopPos;
	_d->time = time;
  _d->lastTimeUpdate = DateTime::getElapsedTime();

	restart();
}

void PositionAnimator::restart()
{
	_d->startPos = getParent() ? getParent()->getRelativeRect().UpperLeftCorner : Point( 0, 0 );
	_d->currentPos = _d->startPos.toPointF();
}

void PositionAnimator::beforeDraw( GfxEngine& painter )
{
	if( isEnabled() && getParent() && isFlag( isActive ) )
	{
    if( fabs(_d->currentPos.getX() - _d->stopPos.getX() ) > 0.5f 
        || fabs( _d->currentPos.getY() - _d->stopPos.getY() ) > 0.5f )
		{
			if( _d->stopPos.getX() == ANIMATOR_UNUSE_VALUE )
				_d->stopPos.setX( int(_d->currentPos.getX() ) );
			if( _d->stopPos.getY() == ANIMATOR_UNUSE_VALUE )
				_d->stopPos.setY( int(_d->currentPos.getY() ) );

      float fps = 1000.f / float( DateTime::getElapsedTime() - _d->lastTimeUpdate + 1 );
      _d->lastTimeUpdate = DateTime::getElapsedTime();
			float step = _d->stopPos.getDistanceFrom( _d->startPos ) / float( fps * ( _d->time / 1000.f ) );
			float offsetX = _d->stopPos.getX() - _d->currentPos.getX();
			float signX = offsetX < 0 ? -1.f : 1.f;
			float offsetY = _d->stopPos.getY() - _d->currentPos.getY();
			float signY = offsetY < 0 ? -1.f : 1.f;

      _d->currentPos += PointF( signX * std::min<float>( step, fabs( offsetX ) ),
                                signY * std::min<float>( step, fabs( offsetY ) ) );

			getParent()->setPosition( _d->currentPos.toPoint() );
		}
		else
		{
			resetFlag( isActive );

			if( isFlag( debug ) )
			{
				getParent()->setPosition( _d->startPos );
                return;
			}

			getParent()->setVisible( isFlag( showParent ) );

            afterFinished_();
		}
	}

    Widget::beforeDraw( painter );
}

PositionAnimator::~PositionAnimator( void )
{
}

void PositionAnimator::setStartPos( const Point& p )
{
	_d->startPos = p;
}

void PositionAnimator::setStopPos( const Point& p )
{
	_d->stopPos = p;
}

Point PositionAnimator::getStartPos() const
{
	return _d->startPos;
}

void PositionAnimator::setTime( int time )
{
	_d->time = time;
}

Point PositionAnimator::getStopPos() const
{
	return _d->stopPos;
}

}//end namespace gui
