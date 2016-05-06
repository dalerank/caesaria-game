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

class VariantList
{
public:
  typedef std::list<Variant> Collection;
  typedef Delegate1< const Variant& > Visitor;

  VariantList() {}

  Variant get( const unsigned int index, Variant defaultVal=Variant() ) const;

  void visitEach( Visitor visitor );

  VariantList& operator <<( const Variant& v );
  Collection::const_iterator begin() const { return _data.begin(); }
  Collection::const_iterator end() const { return _data.end(); }
  bool operator==(const VariantList& a) const;
  bool empty() const { return _data.empty(); }
  Collection::reference front() { return _data.front(); }
  Collection::reference back() { return _data.back(); }
  size_t size() const { return _data.size(); }

  VariantList& push_back(const Variant& a) { _data.push_back(a); return *this; }

  template<class T>
  VariantList( const std::vector<T>& array )
  {
    //typename std::vector<T>::iterator it = array.begin();
    for( auto& item : array ) {
      _data.push_back( Variant(item) );
    }
  }

  template<class A, class B>
  VariantList( const A& a, const B& b)
  {
    _data.emplace_back( Variant(a) );
    _data.emplace_back( Variant(b) );
  }
private:
  Collection _data;
};

class VariantListReader
{
public:
  VariantListReader( const VariantList& list );

  const Variant& next();

  bool atEnd();

private:
  VariantList::Collection::const_iterator _it;
  const VariantList& _list;
};

template<class T>
typename std::vector<T>& operator<<(std::vector<T>& v, const VariantList& vars)
{
  for( const auto& it : vars )
    v.push_back( (T)it );

  return v;
}

#endif // __CAESARIA_VARIANT_LIST_H_INCLUDE__
