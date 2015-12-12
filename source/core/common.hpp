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
#include "gfx/tilepos.hpp"
#include "objects/param.hpp"

namespace utils
{

template<class ObjectList, class Set>
void excludeByType( ObjectList& list, const Set& set )
{
  for( auto it=list.begin(); it != list.end(); )
  {
    if( set.count( (*it)->type() ) > 0 ) { it=list.erase( it ); }
    else { ++it; }
  }
}

template<class ObjectList, class T>
int countByType( const ObjectList& objects, const T& type )
{
  int ret = 0;
  for( auto& it : objects )
  {
    if( it->type() == type )
      ret++;
  }

  return ret;
}

template<class ObjectList>
ObjectList selectByType( const ObjectList& objects, int type )
{
  ObjectList ret;
  for( auto& it : objects )
  {
    if( it->type() == type )
      ret.push_back( it );
  }

  return ret;
}

template<class ObjectPtr>
inline std::string objNameOrDefault( ObjectPtr obj, const std::string& defaultValue="" )
{
  return obj.isValid() ? obj->name() : defaultValue;
}

template<class ObjectPtr>
inline TilePos objPosOrDefault( ObjectPtr obj, const TilePos& defaultValue=TilePos::invalid() )
{
  return obj.isValid() ? obj->pos() : defaultValue;
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

template<class ObjectList>
ObjectList selectAgressive( const ObjectList& list, int much=0 )
{
  ObjectList ret;
  for( auto it : list )
  {
    if( it->agressive() > much )
      ret.push_back( it );
  }

  return ret;
}

template<class ObjectList>
ObjectList& eraseIfDeleted( ObjectList& list )
{
  for( typename ObjectList::iterator it=list.begin(); it != list.end(); )
  {
    if( (*it)->isDeleted() ) { it = list.erase( it ); }
    else { ++it; }
  }

  return list;
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

template< class Object >
SmartPtr<Object> withMinParam( const SmartList<Object> list, Param name)
{
  int lowValue = 9999;
  SmartPtr<Object> ret;
  for( auto obj : list )
  {
    int state = obj->state( name );
    if( lowValue < state );
    {
      lowValue = state;
      ret = obj;
    }
  }

  return ret;
}

template<class Object, class Parent>
std::set<SmartPtr<Object>> select( const std::set<SmartPtr<Parent>>& objects )
{
  std::set<SmartPtr<Object>> ret;
  for( auto item : objects )
  {
    SmartPtr<Object> a = ptr_cast<Object>( item );
    if( a.isValid() )
      ret.insert( a );
  }

  return ret;
}

template<typename Object>
void deleteLater( SmartPtr<Object> overlay )
{
  if( overlay.isValid() )
    overlay->deleteLater();
}

}//end namespace utils

#endif
