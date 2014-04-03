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
#include "walker.hpp"

class WalkerHelper
{
public:
  static WalkerHelper& instance();

  static std::string getTypename( constants::walker::Type type );
  static constants::walker::Type getType( const std::string& name );
  static std::string getPrettyTypeName( constants::walker::Type type );
  static gfx::Picture getBigPicture( constants::walker::Type type );
  static VariantMap getOptions( const std::string& name );
  static VariantMap getOptions( const constants::walker::Type type );

  void initialize( const vfs::Path& filename );

  virtual ~WalkerHelper();
private:
  WalkerHelper();

  class Impl;
  ScopedPtr< Impl > _d;
};

template< class Dst, class Src >
inline std::list<SmartPtr<Dst> >& operator<<(std::list<SmartPtr<Dst> >& dstList, const std::list< SmartPtr<Src> >& srcList )
{
  foreach( it, srcList )
  {
    SmartPtr<Dst> ptr = ptr_cast<Dst>( *it );
    if( ptr.isValid() )
        dstList.push_back( ptr );
  }

  return dstList;
}

template< class Dst >
inline std::list<SmartPtr<Dst> >& operator<<(std::list<SmartPtr<Dst> >& dstList, const WalkerList& srcList )
{
  foreach( it, srcList )
  {
    SmartPtr<Dst> ptr = ptr_cast<Dst>( *it );
    if( ptr.isValid() )
        dstList.push_back( ptr );
  }

  return dstList;
}

template< class Wlk >
SmartPtr<Wlk> findNearestWalker( TilePos pos, const std::list< SmartPtr< Wlk > >& walkers )
{
  SmartPtr< Wlk > p;

  int minDistance=99;
  for( typename std::list< SmartPtr< Wlk > >::const_iterator it=walkers.begin(); it != walkers.end(); ++it )
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
