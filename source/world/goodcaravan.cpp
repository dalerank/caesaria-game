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

#include "goodcaravan.hpp"
#include "empire.hpp"
#include "good/goodstore_simple.hpp"
#include "city.hpp"

namespace world
{

class GoodCaravan::Impl
{
public:
  CityPtr base;
  std::string destination;
  SimpleGoodStore store;

  VariantMap options;
};

GoodCaravanPtr GoodCaravan::create( CityPtr city )
{
  GoodCaravanPtr ret( new GoodCaravan( city ) );
  ret->drop();

  return ret;
}

void GoodCaravan::sendTo(ObjectPtr obj)
{
  _d->destination = (obj.isValid() ? obj->name() : "");
}

void GoodCaravan::sendTo(CityPtr obj)
{
  sendTo( ptr_cast<Object>( obj ) );
}

GoodStore& GoodCaravan::store() { return _d->store; }
std::string GoodCaravan::type() const { return CAESARIA_STR_EXT(GoodCaravan); }

void GoodCaravan::save(VariantMap& stream) const
{
  MovableObject::save( stream );

  stream[ "store" ] = _d->store.save();
  stream[ "base"  ] = Variant( _d->base.isValid() ? _d->base->name() : "" );
  stream[ "destination" ] = Variant( _d->destination );
}

void GoodCaravan::load(const VariantMap& stream)
{
  MovableObject::load( stream );

  _d->options = stream;
  _d->store.load( stream.get( "store").toMap() );
  _d->base = empire()->findCity( stream.get( "base" ).toString() );
  _d->destination = stream.get( "destination" ).toString();
}

void GoodCaravan::_reachedWay()
{
  ObjectPtr obj = empire()->findObject( _d->destination );
  if( obj.isValid() )
  {
    obj->addObject( this );
  }
}

GoodCaravan::GoodCaravan( CityPtr city )
 : MovableObject( city->empire() ), _d( new Impl )
{
  _d->base = city;
}

}
