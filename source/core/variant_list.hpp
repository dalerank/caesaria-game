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

#ifndef __CAESARIA_VARIANT_LIST_H_INCLUDE__
#define __CAESARIA_VARIANT_LIST_H_INCLUDE__

#include "variant.hpp"
#include "delegate.hpp"

class VariantList : public std::list<Variant>
{
public:
  VariantList() {}

  Variant get( const unsigned int index, Variant defaultVal=Variant() ) const
  {
    VariantList::const_iterator it = begin();
    if( index >= size() )
    {
      return defaultVal;
    }
    else
    {
      std::advance( it, index );
      return *it;
    }
  }

  typedef Delegate1< const Variant& > Visitor;

  void visitEach( Visitor visitor )
  {
    foreach( it, *this )
      visitor( *it );
  }

  VariantList& operator <<( const Variant& v )
  {
    push_back( v );
    return *this;
  }

  template<class T>
  VariantList( const std::vector<T>& array )
  {
    //typename std::vector<T>::iterator it = array.begin();
    foreach( it, array )
    {
      push_back( Variant(*it) );
    }
  }
};

template<class T>
typename std::vector<T>& operator<<(std::vector<T>& v, const VariantList& vars)
{
  foreach( it, vars )
    v.push_back( (T)(*it) );

  return v;
}

#endif // __CAESARIA_VARIANT_LIST_H_INCLUDE__
