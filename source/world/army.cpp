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
#include "city.hpp"
#include "empiremap.hpp"
#include "gfx/tilesarray.hpp"
#include <map>

using namespace gfx;

namespace world
{

class Army::Impl
{
public:
  Point start, stop;

  PointF deltaMove;
  CityPtr base;
  ObjectPtr object;
  PointsArray way;
  unsigned int step;
};

Army::Army( Empire& empire )
  : Object( empire ), __INIT_IMPL(Army)
{

}

ArmyPtr Army::create(Empire &empire, CityPtr base)
{
  ArmyPtr ret( new Army( empire ) );
  ret->setBase( base );
  ret->drop();

  return ret;
}

Army::~Army(){}

void Army::timeStep(const unsigned int time)
{
  if( GameDate::isWeekChanged() )
  {
    __D_IMPL(d,Army)
    d->step = math::clamp<int>( d->step+1, 0, d->way.size()-1 );
    setLocation( d->way[ d->step ] );
  }
}

VariantMap Army::save() const
{
  VariantMap ret;

  return ret;
}

void Army::load(const VariantMap& stream)
{
}

void Army::setBase(CityPtr base){  _dfunc()->base = base;  }

void Army::attack(ObjectPtr obj)
{
  __D_IMPL(d,Army)
  d->object = obj;
  d->stop = obj->location();

  d->way = empire()->map().findRoute( d->start, d->stop, EmpireMap::land );
  setLocation( d->start );
  d->step = 0;
}

}
