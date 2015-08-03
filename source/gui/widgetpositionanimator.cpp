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
  struct { Point  stop,
                  start;
           PointF current; } pos;
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

  _d->pos.stop = stopPos;
  _d->time = time;
  _d->lastTimeUpdate = DateTime::elapsedTime();

	restart();
}

void PositionAnimator::restart()
{
  _d->pos.start = parent() ? parent()->relativeRect().lefttop() : Point( 0, 0 );
  _d->pos.current = _d->pos.start.toPointF();
}

void PositionAnimator::beforeDraw(gfx::Engine& painter )
{
	if( enabled() && parent() && isFlag( isActive ) )
	{
    if( fabs(_d->pos.current.x() - _d->pos.stop.x() ) > 0.5f
        || fabs( _d->pos.current.y() - _d->pos.stop.y() ) > 0.5f )
		{
      if( _d->pos.stop.x() == ANIMATOR_UNUSE_VALUE )
        _d->pos.stop.setX( int(_d->pos.current.x() ) );
      if( _d->pos.stop.y() == ANIMATOR_UNUSE_VALUE )
        _d->pos.stop.setY( int(_d->pos.current.y() ) );

      float fps = 1000.f / float( DateTime::elapsedTime() - _d->lastTimeUpdate + 1 );
      _d->lastTimeUpdate = DateTime::elapsedTime();
      float step = _d->pos.stop.getDistanceFrom( _d->pos.start ) / float( fps * ( _d->time / 1000.f ) );
      float offsetX = _d->pos.stop.x() - _d->pos.current.x();
			float signX = offsetX < 0 ? -1.f : 1.f;
      float offsetY = _d->pos.stop.y() - _d->pos.current.y();
			float signY = offsetY < 0 ? -1.f : 1.f;

      if( _d->speed.x() != 0 || _d->speed.y() != 0 )
      {
        offsetX = _d->speed.x();
        offsetY = _d->speed.y();
      }

      _d->pos.current += PointF( signX * std::min<float>( step, fabs( offsetX ) ),
                                 signY * std::min<float>( step, fabs( offsetY ) ) );

      parent()->setPosition( _d->pos.current.toPoint() );
		}
		else
		{
			resetFlag( isActive );

			if( isFlag( debug ) )
			{
        parent()->setPosition( _d->pos.start );
        return;
			}

			parent()->setVisible( isFlag( showParent ) );
      _afterFinished();
		}
	}

  Widget::beforeDraw( painter );
}

PositionAnimator::~PositionAnimator( void ) {}
void PositionAnimator::setStartPos( const Point& p ){	_d->pos.start = p;}
void PositionAnimator::setStopPos( const Point& p ){	_d->pos.stop = p;}
Point PositionAnimator::getStartPos() const{	return _d->pos.start;}
void PositionAnimator::setTime( int time ){	_d->time = time;}
void PositionAnimator::setSpeed(PointF speed) { _d->speed = speed; }
Point PositionAnimator::getStopPos() const{	return _d->pos.stop; }

}//end namespace gui
