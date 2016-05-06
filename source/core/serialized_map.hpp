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

#ifndef _CAESARIA_SERIALIZED_MAP_H_INCLUDE_
#define _CAESARIA_SERIALIZED_MAP_H_INCLUDE_

#include "variant_list.hpp"
#include <set>

template<class A, class B>
class Map
{
public:
  typedef std::map<A,B> Collection;
  typedef typename Collection::iterator iterator;
  typedef typename Collection::const_iterator const_iterator;
  typedef typename Collection::mapped_type mapped_type;
  typedef typename Collection::key_type key_type;

  mapped_type& operator[](const key_type& key) { return _data[key]; }
  const mapped_type& operator[](const key_type& key) const { return _data[key]; }

  iterator find(const key_type& key) { return _data.find(key); }
  const_iterator find(const key_type &key) const { return _data.find(key); }
  size_t count(key_type key) const { return _data.count(key); }
  iterator begin() { return _data.begin(); }
  const_iterator begin() const { return _data.begin(); }
  iterator end() { return _data.end(); }
  const_iterator end() const { return _data.end(); }
  iterator erase(iterator it) { return _data.erase(it); }
  void erase(const key_type& key) { _data.erase(key); }
  void clear() { _data.clear(); }
  size_t size() const { return _data.size(); }
  bool insert(const key_type& key, const mapped_type& value) {
    return _data.insert(std::make_pair(key, value)).second;
  }

  const B& valueOrDefault(const A& type, const B& invalid)
  {
    auto mapIt = find(type);
    if (mapIt == _data.end()) {
      return invalid;
    }
    return mapIt->second;
  }
protected:
  Collection _data;
};

template<class A, class B>
class SerializedMap : public Map<A, B>
{
public:
  virtual VariantList save() const
  {
    VariantList ret;
    for (const auto& item : *this) {
      ret.push_back(VariantList(item.first, item.second));
    }

    return ret;
  }

  virtual void load(const VariantList& stream)
  {
    for (const auto& item : stream) {
      VariantList vl = item.toList();
      (*this)[ vl.get(0) ] = vl.get(1);
    }
  }
};

template<class T>
class Set {
public:
  typedef std::set<T> Collection;
  typedef typename Collection::iterator iterator;
  typedef typename Collection::const_iterator const_iterator;

  iterator find(const T& key) { return _data.find(key); }
  const_iterator find(const T& key) const { return _data.find(key); }
  size_t count(const T& key) const { return _data.count(key); }
  iterator begin() { return _data.begin(); }
  const_iterator begin() const { return _data.begin(); }
  iterator end() { return _data.end(); }
  const_iterator end() const { return _data.end(); }
  iterator erase(iterator it) { return _data.erase(it); }
  void erase(const T& key) { _data.erase(key); }
  void clear() { _data.clear(); }
  size_t size() const { return _data.size(); }
  void insert(const T& value) { _data.insert(value); }

  Set& operator<<(const T& pos)
  {
    _data.insert( pos );
    return *this;
  }

protected:
  Collection _data;
};

#endif//_CAESARIA_SERIALIZED_MAP_H_INCLUDE_
