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
#include "core/math.hpp"
#include <vector>
#include <deque>

template <class T>
class SmartList : public std::deque<SmartPtr< T > >
{
public:
  template< class Src >
  SmartList& operator<<( const SmartList<Src>& srcList )
  {
    for( auto it : srcList )
      addIfValid( ptr_cast<T>( it ) );

    return *this;
  }

  SmartList& append( const SmartList<T>& other )
  {
    for( auto it : other )
      addIfValid( it );

    return *this;
  }

  SmartList& operator<<( SmartPtr< T > a )
  {
    this->push_back( a );
    return *this;
  }

  bool contain( SmartPtr<T> a ) const
  {
    for( auto it : *this )
      if( it == a ) return true;

    return false;
  }

  template< class Dst >
  SmartList<Dst> select() const
  {
    SmartList<Dst> ret;
    for( auto it : *this )
      ret.addIfValid( ptr_cast<Dst>( it ) );

    return ret;
  }

  SmartList& addIfValid( SmartPtr< T > a )
  {
    if( a.isValid() )
      this->push_back( a );

    return *this;
  }

  SmartPtr<T> random() const
  {
    if( this->empty() )
      return SmartPtr<T>();

    typename SmartList<T>::const_iterator it = this->begin();
    std::advance( it, math::random( this->size()-1 ) );
    return *it;
  }

  // Returns `count` different random elements from this list
  // This list should have at least `count` values
  // Useful for randomized actions
  // Recommended `count` to be not more than half of list size, due to performance reasons.
  SmartList<T> random(size_t count) const
  {
    if (this->size() <= count)
      return *this;

    std::vector<int> rands;
    rands.resize(count);
    math::random_values_of_range(rands, count, 0, this->size()-1);

    SmartList<T> ret;

    for (size_t i = 0; i < count; ++i)
    {
      typename SmartList<T>::const_iterator it = this->begin();
      std::advance(it, rands[i]);
      SmartPtr<T> value = *it;
      ret << value;
    }

    return ret;
  }

  void remove( const SmartPtr< T >& a )
  {
    for( typename SmartList<T>::iterator it = this->begin(); it != this->end(); )
    {
      if( a == *it ) { it = this->erase( it ); }
      else { ++it; }
    }
  }

  SmartPtr<T> valueOrEmpty( unsigned int index ) const
  {
    if( index >= this->size() )
      return SmartPtr<T>();

    typename SmartList<T>::const_iterator it = this->begin();
    std::advance( it, index );
    return *it;
  }

  void removeAt( int index )
  {
    if( index < this->size() )
      return;

    typename SmartList<T>::const_iterator it = this->begin();
    std::advance( it, index );
    this->erase( it );
  }

  template< class W >
  SmartList<T> exclude() const
  {
    SmartList<T> ret;
    for( auto it : *this )
      if( !is_kind_of<W>( it ) )
        ret.push_back( it );

    return ret;
  }
};

#endif //__CAESARIA_SMARTLIST_H_INCLUDE__
