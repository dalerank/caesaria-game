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
#include "core/variant_map.hpp"

using namespace gfx;

namespace world
{

class MovableObject::Impl
{
public:
  Point start, stop;

  Route way;
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
  d->speed = game::Date::days2ticks( DateTime::daysInWeek ) / speed;
}

void MovableObject::timeStep(const unsigned int time)
{
  __D_IMPL(d,MovableObject)
  if( ( time % (d->speed+1) ) == 1 )
  {    
    if( !d->way.empty() )
    {
      d->way.step++;

      if( d->way.step >= d->way.size() )
      {
        d->way.reset();
        _reachedWay();
      }
      else
      {
        d->way.step = math::clamp<int>( d->way.step, 0, d->way.size()-1 );
        setLocation( d->way[ d->way.step ] );
      }
    }
    else
    {
      _noWay();
      Logger::warning( "Army: way are empty" );
    }
  }
}

int MovableObject::viewDistance() const { return 40; }
const Route& MovableObject::way() const { return _dfunc()->way; }

void MovableObject::_reachedWay()
{
  deleteLater();
}

void MovableObject::_noWay() {}

Route& MovableObject::_way() { return _dfunc()->way; }

void MovableObject::save(VariantMap& stream) const
{
  Object::save( stream );

  __D_IMPL_CONST(d,MovableObject)
  VARIANT_SAVE_ANY_D( stream, d, start )
  VARIANT_SAVE_ANY_D( stream, d, stop )

  VariantList pointsVl;
  foreach( i, d->way ) { pointsVl.push_back( *i ); }

  stream[ "points" ] = pointsVl;
  stream[ "step"   ] = (int)d->way.step;
}

void MovableObject::load(const VariantMap& stream)
{
  Object::load( stream );

  __D_IMPL(d,MovableObject)
  d->options = stream;
  VARIANT_LOAD_ANY_D( d, start, stream )
  VARIANT_LOAD_ANY_D( d, stop, stream )

  d->way.step = (int)stream.get( "step" );
  VariantList points = stream.get( "points" ).toList();
  foreach( i, points ) { d->way.push_back( (*i).toPoint() ); }
}

bool MovableObject::_findWay( Point p1, Point p2 )
{
  __D_IMPL(d,MovableObject)

  d->start = p1;
  d->stop = p2;

  d->way = empire()->map().findRoute( d->start, d->stop, EmpireMap::land );
  setLocation( d->start );
  d->way.step = 0;

  if( d->way.empty() )
  {
    Logger::warning( "MovableObject: cannot find way from [%d,%d] to [%d,%d]", p1.x(), p1.y(), p2.x(), p2.y() );
    return false;
  }

  return true;
}

class Messenger::Impl
{
public:
  std::string title;
  std::string message;
};

Messenger::~Messenger()
{

}

void Messenger::now( EmpirePtr empire,
                     const std::string& cityname,
                     const std::string& title,
                     const std::string& message)
{
  Messenger* m = new Messenger( empire );
  m->_dfunc()->title = title;
  m->_dfunc()->message = message;

  ObjectPtr obj( m );
  obj->drop();

  CityPtr pcity = empire->findCity(cityname );
  if( pcity.isValid() )
    pcity->addObject( obj );
}

std::string Messenger::title() const { return _dfunc()->title; }
std::string Messenger::message() const { return _dfunc()->message; }

Messenger::Messenger(EmpirePtr empire)
 : MovableObject( empire ), __INIT_IMPL(Messenger)
{

}

}
