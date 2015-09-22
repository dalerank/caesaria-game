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

#ifndef _CAESARIA_PRIORITIES_INCLUDE_H_
#define _CAESARIA_PRIORITIES_INCLUDE_H_

#include <vector>
#include "variant_list.hpp"
#include <set>

template<class T>
inline std::set<T>& operator<<(std::set<T>& which, const T& value)
{
  which.insert( value );
  return which;
}

template<class T>
inline std::set<T>& operator<<(std::set<T>& which, const VariantList& values)
{
  for( auto& item : values )
    which.insert( item );

  return which;
}

template<class T>
class Priorities : public std::vector< T >
{
public:
  inline Priorities& operator<<( const T& v )
  {
    this->push_back( v );
    return *this;
  }

  VariantList save() const
  {
    VariantList vl;
    for( auto& item : *this ) { vl.push_back( Variant( item ) ); }

    return vl;
  }

  void load( const VariantList& stream )
  {
    *this << stream;
  }

  bool contain( const T& v ) const
  {
    for( auto& item : *this ) { if( item == v ) return true; }

    return false;
  }

  Priorities& operator << ( const VariantList& vl )
  {
    for( auto& i : vl ) { this->push_back( (T)i.toInt() ); }

    return *this;
  }
};

#endif  //_CAESARIA_PRIORITIES_INCLUDE_H_
