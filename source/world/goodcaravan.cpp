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
#include "good/storage.hpp"
#include "game/resourcegroup.hpp"
#include "core/logger.hpp"
#include "core/common.hpp"
#include "city.hpp"
#include "core/variant_map.hpp"

namespace world
{

class GoodCaravan::Impl
{
public:
  CityPtr base;
  std::string destination;
  good::Storage store;

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
  _d->destination = "";

  bool baseAndDestinationCorrect = _d->base.isValid() && obj.isValid();
  if( baseAndDestinationCorrect )
  {
    _d->destination = obj->name();
    _findWay( _d->base->location(), obj->location() );

    if( !_way().empty() )
    {
      empire()->addObject( this );
    }
    else
    {
      Logger::warning( "GoodCaravan: cannot find way from {0} to {1}", _d->base->name(), obj->name() );
    }
  }
  else
  {
    Logger::warningIf( _d->base.isNull(), "GoodCaravan: base is null" );
    Logger::warningIf( obj.isNull(), "GoodCaravan: destiantion is null" );
  }
}

void GoodCaravan::sendTo(CityPtr obj) { sendTo( obj.as<Object>() ); }
good::Store& GoodCaravan::store() { return _d->store; }
std::string GoodCaravan::type() const { return TEXT(GoodCaravan); }

void GoodCaravan::timeStep(unsigned int time)
{
  MovableObject::timeStep( time );
}

std::string GoodCaravan::about(Object::AboutType type)
{
  switch( type )
  {
  case aboutEmtype: return "world_goodcaravan";
  default: break;
  }

  return "";
}

void GoodCaravan::save(VariantMap& stream) const
{
  MovableObject::save( stream );

  VARIANT_SAVE_CLASS_D( stream, _d, store )
  stream[ "base"  ] = Variant( utils::objNameOrDefault( _d->base ) );
  VARIANT_SAVE_STR_D( stream, _d, destination )
}

void GoodCaravan::load(const VariantMap& stream)
{
  MovableObject::load( stream );

  _d->options = stream;
  _d->base = empire()->findCity( stream.get( "base" ).toString() );
  Logger::warningIf( _d->base.isNull(), "!!! GoodCaravan::load base not exists" );

  VARIANT_LOAD_CLASS_D( _d, store, stream )
  VARIANT_LOAD_STR_D( _d, destination, stream )
}

void GoodCaravan::_reachedWay()
{
  ObjectPtr obj = empire()->findObject( _d->destination );
  if( obj.isValid() )
  {
    obj->addObject( this );
  }
  deleteLater();
}

GoodCaravan::GoodCaravan( CityPtr city )
 : MovableObject( city->empire() ), _d( new Impl )
{
  _d->base = city;
  _d->store.setCapacity( defaultCapacity );
  _d->store.setCapacity( good::any(), defaultCapacity );

  setSpeed( deafaultSpeed );
}

}//end namespace world
