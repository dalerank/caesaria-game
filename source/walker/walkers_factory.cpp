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

#include "walkers_factory.hpp"
#include "core/logger.hpp"
#include "helper.hpp"
#include "city/city.hpp"
#include <map>

using namespace constants;


class WalkerManager::Impl
{
public:
  typedef std::map< walker::Type, WalkerCreator* > WalkerCreators;
  WalkerCreators constructors;
};

WalkerManager::WalkerManager() : _d( new Impl )
{
}

WalkerManager::~WalkerManager(){}

WalkerPtr WalkerManager::create(const walker::Type walkerType, PlayerCityPtr city)
{
  Impl::WalkerCreators::iterator findConstructor = _d->constructors.find( walkerType );

  if( findConstructor != _d->constructors.end() )
  {
    return ptr_cast<Walker>( findConstructor->second->create( city ) );
  }

  Logger::warning( "WalkerManager: cannot create walker from type %d", walkerType );
  return WalkerPtr();
}

WalkerManager& WalkerManager::instance()
{
  static WalkerManager inst;
  return inst;
}

void WalkerManager::addCreator( const walker::Type type, WalkerCreator* ctor )
{
  std::string typeName = WalkerHelper::getTypename( type );

  bool alreadyHaveConstructor = _d->constructors.find( type ) != _d->constructors.end();
  if( alreadyHaveConstructor )
  {
    Logger::warning( "WalkerManager: also have constructor for type %s", typeName.c_str() );
    return;
  }
  else
  {
    _d->constructors[ type ] = ctor;
  }
}

bool WalkerManager::canCreate( const walker::Type type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
