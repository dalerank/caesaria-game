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

#ifndef _CAESARIA_WALKERHELPER_H_INCLUDE_
#define _CAESARIA_WALKERHELPER_H_INCLUDE_

#include "core/requirements.hpp"
#include "vfs/path.hpp"
#include "core/variant.hpp"
#include "world/nation.hpp"
#include "walker.hpp"

class WalkerHelper
{
public:
  static WalkerHelper& instance();

  static std::string getTypename( constants::walker::Type type );
  static constants::walker::Type getType( const std::string& name );
  static std::string getPrettyTypename( constants::walker::Type type );
  static std::string getNationName( world::Nation type );
  static world::Nation getNation( const std::string& name );
  static gfx::Picture getBigPicture( constants::walker::Type type );
  static VariantMap getOptions( const std::string& name );
  static VariantMap getOptions( const constants::walker::Type type );
  static bool isHuman( WalkerPtr wlk );
  static bool isAnimal( WalkerPtr wlk );

  void load( const vfs::Path& filename );

  virtual ~WalkerHelper();  
private:
  WalkerHelper();

  class Impl;
  ScopedPtr< Impl > _d;
};

class WalkerRelations
{
public:
  typedef enum { enemy=-1, neitral=0, friendt=1 } Relation;
  static WalkerRelations& instance();

  static void addFriend( constants::walker::Type who, constants::walker::Type friendType );
  static void remFriend( constants::walker::Type who, constants::walker::Type friendType );
  static void addFriend( world::Nation who, world::Nation friendType );
  static void addEnemy( constants::walker::Type who, constants::walker::Type enemyType );
  static void addEnemy( world::Nation who, world::Nation enemyType );
  static bool isNeutral( constants::walker::Type a, constants::walker::Type b );
  static bool isNeutral( world::Nation a, world::Nation b);

  void load( vfs::Path path );
  void load( const VariantMap& stream );

  void clear();

  VariantMap save() const;

private:
  WalkerRelations();

  class Impl;
  ScopedPtr<Impl> _d;
};

template< class Wlk >
SmartPtr<Wlk> findNearestWalker( TilePos pos, const SmartList<Wlk>& walkers )
{
  SmartPtr< Wlk > p;

  int minDistance=99;
  for( typename SmartList<Wlk>::const_iterator it=walkers.begin(); it != walkers.end(); ++it )
  {
    int distance = (*it)->pos().distanceFrom( pos );
    if( distance < minDistance )
    {
      minDistance =  distance;
      p = *it;
    }
  }

  return p;
}

#endif //_CAESARIA_WALKERHELPER_H_INCLUDE_
