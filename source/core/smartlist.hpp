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
#include <functional>
#include <deque>

template <class T>
class SmartList
{
public:
  typedef std::deque<SmartPtr<T>> Collection;
  typedef typename Collection::iterator iterator;
  typedef typename Collection::const_iterator const_iterator;
  typedef typename Collection::reference reference;
  typedef typename Collection::const_reference const_reference;

  const_iterator begin() const { return _data.begin(); }
  iterator begin() { return _data.begin(); }
  iterator end() { return _data.end(); }
  const_iterator end() const { return _data.end(); }
  size_t size() const { return _data.size(); }
  void resize(size_t newSize) { _data.resize(newSize); }
  bool empty() const { return _data.empty(); }
  void clear() { _data.clear(); }
  void push_back(const SmartPtr<T>& value) { _data.push_back(value); }
  reference front() { return _data.front(); }
  const_reference front() const { return _data.front(); }
  const_reference back() const { return _data.back(); }
  iterator erase( typename Collection::iterator it) { return _data.erase(it); }
  reference operator[](size_t index) { return _data[index]; }
  const_reference operator [](size_t index) const { return _data[index]; }

  SmartList& append( const SmartList<T>& other, bool checkValid=true)
  {
    if (checkValid) {
      for (const auto& it : other)
        addIfValid(it);
    } else {
      _data.insert(_data.end(), other.begin(), other.end());
    }

    return *this;
  }

  SmartList& operator<<( SmartPtr<T> a )
  {
    _data.push_back( a );
    return *this;
  }

  bool contain(SmartPtr<T> a) const
  {
    for (const auto& it : _data)
      if (it == a) return true;

    return false;
  }

  template< class Dst >
  SmartList<Dst> select() const
  {
    SmartList<Dst> ret;
    for( const auto& it : _data )
      ret.addIfValid(ptr_cast<Dst>(it));

    return ret;
  }

  SmartList& for_each(std::function<void (SmartPtr<T>)> function)
  {
    for (const auto& item : _data)
       function(item);

    return *this;
  }

  SmartList<T> where( std::function<bool (SmartPtr<T>)> comparer ) const
  {
    SmartList<T> ret;
    for( const auto& item : *this )
      if( comparer( item ) )
        ret.push_back( item );

    return ret;
  }

  SmartPtr<T> select(std::function<bool (SmartPtr<T>)> comparer) const
  {
    for( const auto& item : *this )
      if( comparer( item ) )
        return item;

    return SmartPtr<T>();
  }

  SmartPtr<T> firstOrEmpty() const
  {
    return _data.empty()
              ? SmartPtr<T>()
              : _data.front();
  }

  template<class U>
  SmartPtr<U> firstOrEmpty() const
  {
    for( const auto& it : *this )
    {
      SmartPtr<U> ptr = ptr_cast<U>( it );
      if( ptr.isValid() )
        return ptr;
    }

    return SmartPtr<U>();
  }

  template< class Q >
  Q summ(const Q& initial, std::function<Q (SmartPtr<T>)> func_summ) const
  {
    Q ret = initial;
    for( const auto& item : *this )
      ret += func_summ( item );

    return ret;
  }

  SmartList& addIfValid( SmartPtr< T > a )
  {
    if( a.isValid() )
      _data.push_back( a );

    return *this;
  }

  SmartPtr<T> random() const
  {
    if( this->empty() )
      return SmartPtr<T>();

    auto it = this->begin();
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
    math::random_values_of_range(rands.data(), count, 0, this->size()-1);

    SmartList<T> ret;

    for (size_t i = 0; i < count; ++i)
    {
      auto it = this->begin();
      std::advance(it, rands[i]);
      SmartPtr<T> value = *it;
      ret << value;
    }

    return ret;
  }

  void remove( const SmartPtr<T>& a )
  {
    for( auto it = this->begin(); it != this->end(); )
    {
      if( a == *it ) { it = this->erase( it ); }
      else { ++it; }
    }
  }

  SmartPtr<T> valueOrEmpty(unsigned int index) const
  {
    if( index >= this->size() )
      return SmartPtr<T>();

    auto it = this->begin();
    std::advance( it, index );
    return *it;
  }

  void removeAt(int index)
  {
    if( index < this->size() )
      return;

    auto it = this->begin();
    std::advance( it, index );
    this->erase( it );
  }

  int indexOf(SmartPtr<T> who) const
  {
    int index=0;
    for( const auto& it : *this )
      if( it == who ) return index;
      else index++;

    return -1;
  }

  template< class W >
  int count() const
  {
    int ret = 0;
    for( const auto& it : *this )
      ret += (is_kind_of<W>( it ) ? 1 : 0);

    return ret;
  }

  int count(std::function<bool (SmartPtr<T>)> comparer) const
  {
    int ret = 0;
    for( const auto& item : *this )
      ret += comparer( item ) ? 1 : 0;

    return ret;
  }

  template< class W >
  SmartList<T> exclude() const
  {
    SmartList<T> ret;
    for( const auto& it : *this )
      if( !is_kind_of<W>( it ) )
        ret.push_back( it );

    return ret;
  }

  const_reference at(size_t index) const {
    return _data.at(index);
  }

  reference at(size_t index) {
    return _data.at(index);
  }

protected:
  Collection _data;
};

#endif //__CAESARIA_SMARTLIST_H_INCLUDE__
