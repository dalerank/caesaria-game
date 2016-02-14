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

#include "object.hpp"
#include "core/variant_map.hpp"
#include "empire.hpp"
#include "core/logger.hpp"
#include "gfx/animation.hpp"
#include "core/logger.hpp"
#include "core/stacktrace.hpp"
#include "objects_factory.hpp"

using namespace gfx;

namespace world
{

REGISTER_CLASS_IN_WORLDFACTORY(Object)

class Object::Impl
{
public:
  Picture pic;
  std::string emtype;
  Point location;
  EmpirePtr empire;
  std::string name;
  unsigned int time;
  bool isDeleted;
  Nation nation;
};

ObjectPtr Object::create( EmpirePtr empire)
{
  ObjectPtr ret( new Object( empire ) );
  ret->drop();

  return ret;
}

bool Object::isDeleted() const { return _d->isDeleted; }
std::string Object::type() const { return TEXT(Object); }
void Object::timeStep(const unsigned int time) {}
EmpirePtr Object::empire() const { return _d->empire; }
std::string Object::name() const { return _d->name; }
void Object::setName(const std::string& name) { _d->name = name; }
Point Object::location() const { return _d->location;}

void Object::addObject(ObjectPtr obj)
{
  Logger::warning( "WorldObjects: {} added to {}", obj->name(), name() );
}

void Object::setLocation(const Point& location){  _d->location = location; }
Nation Object::nation() const {  return _d->nation; }

std::string Object::about(Object::AboutType type)
{
  if( type == aboutEmtype )
    return _d->emtype;

  return "";
}

void Object::save( VariantMap& stream ) const
{
  VARIANT_SAVE_ANY_D( stream, _d, location )
  stream[ "picture" ] = Variant( _d->pic.name() );
  VARIANT_SAVE_STR_D( stream, _d, name )    
  VARIANT_SAVE_STR_D( stream, _d, emtype )
  VARIANT_SAVE_ANY_D( stream, _d, isDeleted )
  stream[ "type" ] = Variant( type() );
  stream[ "nation" ] = world::toString( _d->nation );
}

void Object::load(const VariantMap& stream)
{
  VARIANT_LOAD_ANYDEF_D( _d, location, _d->location, stream )
  VARIANT_LOAD_STRDEF_D( _d, name, _d->name, stream )
  VARIANT_LOAD_STRDEF_D( _d, emtype, "unknown", stream )

  Logger::warningIf( _d->name.empty(), "Object: name is null" );

  VARIANT_LOAD_ANY_D( _d, isDeleted, stream )
  Variant vNation = stream.get( "nation" );
  if( vNation.isValid() )
    _d->nation = world::toNation( vNation.toString() );
}

void Object::attach()
{
  if( _d->empire.isValid() )
    _d->empire->addObject( this );
}

Object::~Object() {}

Object::Object(EmpirePtr empire) : _d( new Impl )
{
  _d->time = 0;
  _d->nation = nation::unknown;
  _d->empire = empire;
  _d->emtype = "unknown";
  _d->isDeleted = false;
}

void Object::deleteLater() { _d->isDeleted = true; }
void Object::_setNation(Nation nation) { _d->nation = nation; }

}//end namespace world
