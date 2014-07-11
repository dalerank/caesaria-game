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
  CityPtr base;
  std::string destination;

  VariantMap options;
};

Army::Army( EmpirePtr empire )
  : MovableObject( empire ), __INIT_IMPL(Army)
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

void Army::_reachedWay()
{
  __D_IMPL(d,Army)

  ObjectPtr obj = empire()->findObject( d->destination );
  if( obj.isValid() )
  {
    obj->addObject( this );
  }
  else
  {
    Logger::warning( "!!!Army: nof found object with name " +(d->destination.empty() ? "NULL" : d->destination) );
  }

  MovableObject::_reachedWay();
}

void Army::save(VariantMap& stream) const
{
  MovableObject::save( stream );

  __D_IMPL_CONST(d,Army)
  stream[ "base"  ] = Variant( d->base.isValid() ? d->base->name() : "" );
}

void Army::load(const VariantMap& stream)
{
  MovableObject::load( stream );

  __D_IMPL(d,Army)
  d->base = empire()->findCity( d->options[ "base" ].toString() );
  d->options = stream;
}

std::string Army::type() const { return CAESARIA_STR_EXT(Army); }

void Army::setBase(CityPtr base){  _dfunc()->base = base;  }

void Army::attack(ObjectPtr obj)
{
  __D_IMPL(d,Army)
  if( d->base.isValid() && obj.isValid() )
  {
    d->destination = obj->name();
    _findWay( d->base->location(), obj->location() );

    if( !_way().empty() )
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

}
