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

using namespace gfx;

namespace world
{

class Object::Impl
{
public:
  Picture pic;
  Point location;
  EmpirePtr empire;
  std::string name;
  Animation animation;
  Pictures pictures;
  unsigned int time;
  bool isDeleted;
};

ObjectPtr Object::create( EmpirePtr empire)
{
  ObjectPtr ret( new Object( empire ) );
  ret->drop();

  return ret;
}

bool Object::isDeleted() const { return _d->isDeleted; }
std::string Object::type() const { return CAESARIA_STR_EXT(Object); }
void Object::timeStep(const unsigned int time) {}
EmpirePtr Object::empire() const { return _d->empire; }
std::string Object::name() const { return _d->name; }
void Object::setName(const std::string& name) { _d->name = name; }
Point Object::location() const { return _d->location;}

void Object::addObject(ObjectPtr obj)
{
  Logger::warning( "WorldObjects: %s added to %s", obj->name().c_str(), name().c_str() );
}

void Object::setLocation(const Point& location){  _d->location = location; }
Picture Object::picture() const { return _d->pic; }

const Pictures& Object::pictures() const
{
  _d->animation.update( _d->time++ );
  _d->pictures[ idxAnimation ] = _d->animation.currentFrame();

  return _d->pictures;
}

void Object::setPicture(Picture pic)
{
  _d->pic = pic;
  _d->pictures[ idxPicture ] = pic;
}

bool Object::isMovable() const { return false; }

std::string Object::about(Object::AboutType)
{
  return "";
}

void Object::save( VariantMap& stream ) const
{
  VARIANT_SAVE_ANY_D( stream, _d, location )
  stream[ "picture" ] = Variant( _d->pic.name() );
  VARIANT_SAVE_STR_D( stream, _d, name )
  VARIANT_SAVE_CLASS_D( stream, _d, animation )
  VARIANT_SAVE_ANY_D( stream, _d, isDeleted )
  stream[ "type" ] = Variant( type() );
}

void Object::load(const VariantMap& stream)
{
  VARIANT_LOAD_ANYDEF_D( _d, location, _d->location, stream )
  VARIANT_LOAD_STRDEF_D( _d, name, _d->name, stream )

  Logger::warningIf( _d->name.empty(), "Object: name is null" );

  std::string picName = stream.get( "picture" ).toString();
  if( !picName.empty() )
    setPicture( Picture( picName )  );
  VARIANT_LOAD_CLASS_D( _d, animation, stream )
  VARIANT_LOAD_ANY_D( _d, isDeleted, stream )
}

void Object::attach()
{
  if( _d->empire.isValid() )
    _d->empire->addObject( this );
}

Object::~Object() {}

Object::Object( EmpirePtr empire) : _d( new Impl )
{
  _d->time = 0;
  _d->empire = empire;
  _d->pictures.resize( 2 );
  _d->isDeleted = false;
}

void Object::deleteLater() { _d->isDeleted = true; }
Animation& Object::_animation() { return _d->animation; }
Pictures&  Object::_pictures()  { return _d->pictures; }

}
