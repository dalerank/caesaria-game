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

#include "movableobject.hpp"
#include "core/foreach.hpp"
#include "game/gamedate.hpp"
#include "empire.hpp"
#include "core/logger.hpp"
#include "city.hpp"
#include "empiremap.hpp"
#include "gfx/tilesarray.hpp"
#include "game/resourcegroup.hpp"
#include <map>

using namespace gfx;

namespace world
{

class MovableObject::Impl
{
public:
  Point start, stop;

  PointsArray way;
  unsigned int step;
  unsigned int speed;
  VariantMap options;
};

MovableObject::MovableObject( EmpirePtr empire )
  : Object( empire ), __INIT_IMPL(MovableObject)
{
  setSpeed( 1.f );
}

MovableObject::~MovableObject(){}

void MovableObject::setSpeed(float speed)
{
  __D_IMPL(d,MovableObject)
  d->speed = GameDate::days2ticks( DateTime::daysInWeek ) / speed;
}

void MovableObject::timeStep(const unsigned int time)
{
  __D_IMPL(d,MovableObject)
  if( ( time % (d->speed+1) ) == 1 )
  {    
    if( !d->way.empty() )
    {
      d->step++;

      if( d->step >= d->way.size() )
      {
        _reachedWay();
      }
      else
      {
        d->step = math::clamp<int>( d->step, 0, d->way.size()-1 );
        setLocation( d->way[ d->step ] );
      }
    }
    else
    {
      Logger::warning( "Army: way are empty" );
    }
    }
}

void MovableObject::_reachedWay()
{
  deleteLater();
}

PointsArray &MovableObject::_way() { return _dfunc()->way; }

void MovableObject::save(VariantMap& stream) const
{
  Object::save( stream );

  __D_IMPL_CONST(d,MovableObject)
  stream[ "start" ] = d->start;
  stream[ "stop"  ] = d->stop;

  VariantList pointsVl;
  foreach( i, d->way ) { pointsVl.push_back( *i ); }

  stream[ "points" ] = pointsVl;
  stream[ "step" ] = d->step;
}

void MovableObject::load(const VariantMap& stream)
{
  Object::load( stream );

  __D_IMPL(d,MovableObject)
  d->options = stream;
  d->start = stream.get( "start" ).toPoint();
  d->stop = stream.get( "stop"  ).toPoint();

  VariantList points = stream.get( "points" ).toList();
  foreach( i, points ) { d->way.push_back( (*i).toPoint() ); }

  d->step = stream.get( "step" );
}

void MovableObject::_findWay( Point p1, Point p2 )
{
  __D_IMPL(d,MovableObject)

  d->start = p1;
  d->stop = p2;

  d->way = empire()->map().findRoute( d->start, d->stop, EmpireMap::land );
  setLocation( d->start );
  d->step = 0;

  empire()->addObject( this );

  if( d->way.empty() )
  {
    Logger::warning( "MovableObject: cannot find way from [%d,%d] to [%d,%d]", p1.x(), p1.y(), p2.x(), p2.y() );
  }
}

}
