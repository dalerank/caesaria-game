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

#ifndef __CAESARIA_WALKERMANAGER_H_INCLUDED__
#define __CAESARIA_WALKERMANAGER_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"
#include "walker/constants.hpp"
#include "walkers_factory_creator.hpp"

class WalkerManager
{
public:
  static WalkerManager& instance();

  bool canCreate( constants::walker::Type type ) const;

  void addCreator( constants::walker::Type type, WalkerCreator* ctor );

  WalkerPtr create( constants::walker::Type walkerType, PlayerCityPtr city );  // get an instance of the given type

  ~WalkerManager();
private:
  WalkerManager();

  class Impl;
  ScopedPtr< Impl > _d;
};

#define REGISTER_CLASS_IN_WALKERFACTORY(type,a) \
namespace { \
struct Registrator_##a { Registrator_##a() { WalkerManager::instance().addCreator( type, new WalkerBaseCreator<a>() ); }}; \
static Registrator_##a rtor_##a; \
}

#endif //__CAESARIA_WALKERMANAGER_H_INCLUDED__
