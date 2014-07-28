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
  PointF speed;
};

PositionAnimator::PositionAnimator( Widget* node, 
                                    int flags,
                                    const Point& stopPos,
                                    int time )
	: WidgetAnimator( node, flags ), _d( new Impl )
{
  //"parent must be exist"
  _CAESARIA_DEBUG_BREAK_IF( !node );

	_d->stopPos = stopPos;
  _d->time = time;
  _d->lastTimeUpdate = DateTime::elapsedTime();

	restart();
}

void PositionAnimator::restart()
{
	_d->startPos = parent() ? parent()->relativeRect().UpperLeftCorner : Point( 0, 0 );
	_d->currentPos = _d->startPos.toPointF();
}

void PositionAnimator::beforeDraw(gfx::Engine& painter )
{
	if( enabled() && parent() && isFlag( isActive ) )
	{
    if( fabs(_d->currentPos.x() - _d->stopPos.x() ) > 0.5f 
        || fabs( _d->currentPos.y() - _d->stopPos.y() ) > 0.5f )
		{
			if( _d->stopPos.x() == ANIMATOR_UNUSE_VALUE )
				_d->stopPos.setX( int(_d->currentPos.x() ) );
			if( _d->stopPos.y() == ANIMATOR_UNUSE_VALUE )
				_d->stopPos.setY( int(_d->currentPos.y() ) );

      float fps = 1000.f / float( DateTime::elapsedTime() - _d->lastTimeUpdate + 1 );
      _d->lastTimeUpdate = DateTime::elapsedTime();
			float step = _d->stopPos.getDistanceFrom( _d->startPos ) / float( fps * ( _d->time / 1000.f ) );
			float offsetX = _d->stopPos.x() - _d->currentPos.x();
			float signX = offsetX < 0 ? -1.f : 1.f;
			float offsetY = _d->stopPos.y() - _d->currentPos.y();
			float signY = offsetY < 0 ? -1.f : 1.f;

      if( _d->speed.x() != 0 || _d->speed.y() != 0 )
      {
        offsetX = _d->speed.x();
        offsetY = _d->speed.y();
      }

      _d->currentPos += PointF( signX * std::min<float>( step, fabs( offsetX ) ),
                                signY * std::min<float>( step, fabs( offsetY ) ) );

			parent()->setPosition( _d->currentPos.toPoint() );
		}
		else
		{
			resetFlag( isActive );

			if( isFlag( debug ) )
			{
				parent()->setPosition( _d->startPos );
        return;
			}

			parent()->setVisible( isFlag( showParent ) );
      _afterFinished();
		}
	}

  Widget::beforeDraw( painter );
}

PositionAnimator::~PositionAnimator( void ) {}
void PositionAnimator::setStartPos( const Point& p ){	_d->startPos = p;}
void PositionAnimator::setStopPos( const Point& p ){	_d->stopPos = p;}
Point PositionAnimator::getStartPos() const{	return _d->startPos;}
void PositionAnimator::setTime( int time ){	_d->time = time;}
void PositionAnimator::setSpeed(PointF speed) { _d->speed = speed; }
Point PositionAnimator::getStopPos() const{	return _d->stopPos; }

}//end namespace gui
