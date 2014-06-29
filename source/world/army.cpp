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

#include "army.hpp"
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

class Army::Impl
{
public:
  Point start, stop;

  CityPtr base;
  ObjectPtr object;
  PointsArray way;
  unsigned int step;

  VariantMap options;
};

Army::Army( EmpirePtr empire )
  : Object( empire ), __INIT_IMPL(Army)
{
  _animation().load( ResourceGroup::empirebits, 37, 16 );
  _animation().setLoop( Animation::loopAnimation );
}

ArmyPtr Army::create(EmpirePtr empire)
{
  ArmyPtr ret( new Army( empire ) );
  ret->drop();

  return ret;
}

Army::~Army(){}

void Army::timeStep(const unsigned int time)
{
  if( GameDate::isWeekChanged() )
  {
    __D_IMPL(d,Army)
    if( !d->way.empty() )
    {
      d->step++;

      if( d->step >= d->way.size() )
      {
        if( is_kind_of<City>( d->object ) )
        {
          CityPtr cityp = ptr_cast<City>( d->object );
          cityp->addObject( this );
        }
        deleteLater();
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

void Army::initialize()
{
  __D_IMPL(d,Army)

  d->base = empire()->findCity( d->options[ "base" ].toString() );
  d->object = empire()->findObject( d->options[ "object" ].toString() );

  d->options.clear();
}

void Army::save(VariantMap& stream) const
{
  Object::save( stream );

  __D_IMPL_CONST(d,Army)
  stream[ "start" ] = d->start;
  stream[ "stop"  ] = d->stop;
  stream[ "base"  ] = Variant( d->base.isValid() ? d->base->name() : "" );
  stream[ "object"] = Variant( d->object.isValid() ? d->object->name() : "" );

  VariantList pointsVl;
  foreach( i, d->way ) { pointsVl.push_back( *i ); }

  stream[ "points" ] = pointsVl;
  stream[ "step" ] = d->step;
}

void Army::load(const VariantMap& stream)
{
  Object::load( stream );

  __D_IMPL(d,Army)
  d->options = stream;
  d->start = stream.get( "start" ).toPoint();
  d->stop = stream.get( "stop"  ).toPoint();

  VariantList points = stream.get( "points" ).toList();
  foreach( i, points ) { d->way.push_back( (*i).toPoint() ); }

  d->step = stream.get( "step" );
}

std::string Army::type() const { return CAESARIA_STR_EXT(Army); }

void Army::setBase(CityPtr base){  _dfunc()->base = base;  }

void Army::attack(ObjectPtr obj)
{
  __D_IMPL(d,Army)
  if( d->base.isValid() && obj.isValid() )
  {
    d->object = obj;
    d->start = d->base->location();
    d->stop = obj->location();

    d->way = empire()->map().findRoute( d->start, d->stop, EmpireMap::land );
    setLocation( d->start );
    d->step = 0;

    if( !d->way.empty() )
    {
      empire()->addObject( this );
    }
    else
    {
      Logger::warning( "Army: cannot find way from %s to %s", d->base->name().c_str(), obj->name().c_str() );
    }
  }
  else
  {
    Logger::warningIf( d->base.isNull(), "Army: base is null" );
    Logger::warningIf( obj.isNull(), "Army: object for attack is null" );
  }
}

Picture Army::picture() const
{
  return Object::picture();
}

}
