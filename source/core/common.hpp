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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_COMMON_H_INCLUDE_
#define _CAESARIA_COMMON_H_INCLUDE_

#include "smartlist.hpp"
#include "position.hpp"

namespace utils
{

template<class ObjectList, class Set>
inline void excludeByType( ObjectList& list, const Set& set )
{
  for( auto it=list.begin(); it != list.end(); )
  {
    if( set.count( (*it)->type() ) > 0 ) { it=list.erase( it ); }
    else { ++it; }
  }
}

template< class Object >
SmartPtr<Object> findNearest( const TilePos& pos, const SmartList<Object>& list )
{
  SmartPtr<Object> p;

  int minDistance=99;
  for( auto obj : list )
  {
    int distance = obj->pos().distanceFrom( pos );
    if( distance < minDistance )
    {
      minDistance =  distance;
      p = obj;
    }
  }

  return p;
}

template< class Object >
SmartPtr<Object> findByName( const SmartList<Object> list, const std::string& name)
{
  for( auto obj : list )
  {
    if( obj->name() == name )
      return obj;
  }

  return SmartPtr<Object>();
}

}//end namespace utils

#endif
