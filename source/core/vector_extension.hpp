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

#ifndef _CAESARIA_VECTOR_EXTENSION_INCLUDE_H_
#define _CAESARIA_VECTOR_EXTENSION_INCLUDE_H_

#include <vector>
#include "math.h"

template<class T>
class Array {
public:
  typedef std::vector<T> Collection;

  Array& push_back(const T& p) { _data.push_back(p); return *this; }
  typename Collection::reference operator[](size_t index) { return _data[index]; }
  typename Collection::const_reference operator[](size_t index) const { return _data[index]; }
  typename Collection::const_iterator begin() const { return _data.begin(); }
  typename Collection::iterator begin() { return _data.begin(); }
  typename Collection::iterator end() { return _data.end(); }
  typename Collection::const_iterator end() const { return _data.end(); }
  size_t size() const { return _data.size(); }
  void resize(size_t newSize) { _data.resize(newSize); }
  bool empty() const { return _data.empty(); }
  void clear() { _data.clear(); }
  typename Collection::reference front() { return _data.front(); }

  Array& addUnique(const T& pos)
  {
    auto it = std::find(begin(), end(), pos);
    if (it == end())
      push_back(pos);

    return *this;
  }

  const T& random() const {
    return valueOrEmpty( math::random(size() - 1));
  }

  Array& pop_front() {
    if (!empty())
      _data.erase( begin() );

    return *this;
  }

  void remove(const T& pos) {
    erase( std::remove( begin(), end(), pos ) );
  }

  Array() {}
  Array(const std::vector<T>& d)
  {
    _data.resize(d.size());
    std::copy(d.begin(),d.end(),_data.begin());
  }


  Array& operator<<(const T& v)
  {
    _data.push_back(v);
    return *this;
  }

  const T& valueOrEmpty(size_t index) const
  {
    static T invalid = T();
    return index < _data.size() ? _data.at(index) : invalid;
  }

protected:
  Collection _data;
};


#endif  //_CAESARIA_VECTOR_EXTENSION_INCLUDE_H_
