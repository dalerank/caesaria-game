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

namespace world
{

class Object::Impl
{
public:
  Picture pic;
  Point location;
  Empire* empire;
  std::string name;
};

ObjectPtr Object::create(Empire& empire)
{
  ObjectPtr ret( new Object() );
  ret->_d->empire = &empire;
  ret->drop();

  return ret;
}

EmpirePtr Object::getEmpire() const {  return _d->empire; }
std::string Object::getName() const {  return _d->name; }

Point Object::getLocation() const{  return _d->location;}
void Object::setLocation(const Point& location){  _d->location = location; }

Picture Object::getPicture() const{  return _d->pic;}
void Object::setPicture(Picture pic){ _d->pic = pic;}

VariantMap Object::save() const
{
  VariantMap ret;
  ret[ "location" ] = _d->location;
  ret[ "picture" ] = Variant( _d->pic.getName() );
  ret[ "name" ] = Variant( _d->name );

  return ret;
}

void Object::load(const VariantMap& stream)
{
  _d->location = stream.get( "location" ).toPoint();
  _d->name = stream.get( "name" ).toString();
  _d->pic = Picture::load( stream.get( "picture" ).toString() + ".png" );
}

Object::~Object() {}
Object::Object() : _d( new Impl ) {}


}
