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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "objects_factory.hpp"
#include "core/logger.hpp"
#include "empire.hpp"
#include "object.hpp"
#include "city.hpp"
#include <map>

namespace world
{

class ObjectsFactory::Impl
{
public:
  typedef std::map< std::string, ObjectCreator* > ObjectCreators;
  ObjectCreators constructors;
};

ObjectsFactory::ObjectsFactory() : _d( new Impl )
{
}

ObjectsFactory::~ObjectsFactory(){}

ObjectPtr ObjectsFactory::create(const std::string& type, EmpirePtr empire)
{
  Impl::ObjectCreators::iterator findConstructor = _d->constructors.find( type );

  if( findConstructor != _d->constructors.end() )
  {
    return findConstructor->second->create( empire );
  }

  Logger::warning( "ObjectsFactory: cannot create empire object from type " + (type.empty() ? "null" : type) );
  return Object::create( empire );
}

void ObjectsFactory::addCreator( const std::string& type, ObjectCreator* ctor )
{
  bool alreadyHaveConstructor = canCreate( type );
  if( alreadyHaveConstructor )
  {
    Logger::warning( "WalkerManager: also have constructor for type " + type );
    return;
  }
  else
  {
    _d->constructors[ type ] = ctor;
  }
}

bool ObjectsFactory::canCreate( const std::string& type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}

}//end namespace world
