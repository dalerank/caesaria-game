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
      Logger::warning( "GoodCaravan: cannot find way from %s to %s", _d->base->name().c_str(), obj->name().c_str() );
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
std::string GoodCaravan::type() const { return CAESARIA_STR_EXT(GoodCaravan); }

void GoodCaravan::timeStep(unsigned int time)
{
  MovableObject::timeStep( time );
}

void GoodCaravan::save(VariantMap& stream) const
{
  MovableObject::save( stream );

  VARIANT_SAVE_CLASS_D( stream, _d, store )
  stream[ "base"  ] = Variant( _d->base.isValid() ? _d->base->name() : "" );
  VARIANT_SAVE_STR_D( stream, _d, destination )
}

void GoodCaravan::load(const VariantMap& stream)
{
  MovableObject::load( stream );

  _d->options = stream;
  _d->base = empire()->findCity( stream.get( "base" ).toString() );
  Logger::warningIf( _d->base == 0, "!!! WARNING: GoodCaravan::load base not exists" );

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

  setPicture( gfx::Picture( ResourceGroup::panelBackground, 108 ) );
}

}//end namespace world
