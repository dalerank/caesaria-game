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

template<class A, class B>
class SerializedMap : public std::map<A,B>
{
public:
  virtual VariantList save() const
  {
    VariantList ret;
    for( const auto& item : *this )
      ret.push_back( VariantList(item.first, item.second ) );

    return ret;
  }

  virtual void load( const VariantList& stream )
  {
    for( const auto& item : stream )
    {
      VariantList vl = item.toList();
      (*this)[ vl.get(0) ] = vl.get(1);
    }
  }
};

#endif//_CAESARIA_SERIALIZED_MAP_H_INCLUDE_
