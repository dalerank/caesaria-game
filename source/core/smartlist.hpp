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

#ifndef __CAESARIA_SMARTLIST_H_INCLUDE__
#define __CAESARIA_SMARTLIST_H_INCLUDE__

#include "smartptr.hpp"
#include "foreach.hpp"
#include "core/math.hpp"
#include <deque>

template <class T>
class SmartList : public std::deque<SmartPtr< T > >
{
public:
  template< class Src >
  SmartList& operator<<( const SmartList<Src>& srcList )
  {
    foreach( it, srcList )
    {
      SmartPtr<T> ptr = ptr_cast<T>( *it );
      if( ptr.isValid() )
          this->push_back( ptr );
    }

    return *this;
  }

  SmartList& operator<<( SmartPtr< T > a )
  {
    this->push_back( a );
    return *this;
  }

  SmartPtr<T> random() const
  {
    if( this->empty() )
      return SmartPtr<T>();

    typename SmartList<T>::const_iterator it = this->begin();
    std::advance( it, math::random( this->size() ) );
    return *it;
  }

  void remove( const SmartPtr< T >& a )
  {
    for( typename SmartList<T>::iterator it = this->begin(); it != this->end();)
    {
      if( a == *it ) { it = this->erase( it ); }
      else { ++it; }
    }
  }

  template< class W >
  SmartList exclude() const
  {
    SmartList<T> ret;
    foreach( it, *this )
    {
      SmartPtr<W> ptr = ptr_cast<W>( *it );
      if( ptr.isNull() )
      {
        ret.push_back( *it );
      }
    }

    return ret;
  }
};

#endif //__CAESARIA_SMARTLIST_H_INCLUDE__
