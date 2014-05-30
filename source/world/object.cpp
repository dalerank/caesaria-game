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
#include "core/variant.hpp"
#include "empire.hpp"
#include "gfx/animation.hpp"

using namespace gfx;

namespace world
{

class Object::Impl
{
public:
  Picture pic;
  Point location;
  Empire* empire;
  std::string name;
  Animation animation;
  Pictures pictures;
  unsigned int time;
};

ObjectPtr Object::create(Empire& empire)
{
  ObjectPtr ret( new Object() );
  ret->_d->empire = &empire;
  ret->drop();

  return ret;
}

EmpirePtr Object::empire() const { return _d->empire; }
std::string Object::name() const { return _d->name; }

Point Object::location() const { return _d->location;}
void Object::setLocation(const Point& location){  _d->location = location; }

Picture Object::picture() const { return _d->pic; }

const Pictures& Object::pictures() const
{
  _d->animation.update( _d->time++ );
  _d->pictures[ 1 ] = _d->animation.currentFrame();

  return _d->pictures;
}

void Object::setPicture(Picture pic)
{
  _d->pic = pic;
  _d->pictures[ 0 ] = pic;
}

VariantMap Object::save() const
{
  VariantMap ret;
  ret[ "location" ] = _d->location;
  ret[ "picture" ] = Variant( _d->pic.name() );
  ret[ "name" ] = Variant( _d->name );
  ret[ "animation" ] = _d->animation.save();

  return ret;
}

void Object::load(const VariantMap& stream)
{
  _d->location = stream.get( "location" ).toPoint();
  _d->name = stream.get( "name" ).toString();
  setPicture( Picture::load( stream.get( "picture" ).toString() ) );
  _d->animation.load( stream.get( "animation" ).toMap() );
}

Object::~Object() {}
Object::Object() : _d( new Impl )
{
  _d->time = 0;
  _d->pictures.resize( 2 );
}

}
