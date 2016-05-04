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

#ifndef _CAESARIA_WALKERTYPESET_INCLUDE_H_
#define _CAESARIA_WALKERTYPESET_INCLUDE_H_

#include "core/variant_list.hpp"
#include "walker/constants.hpp"
#include <set>

class WalkerTypeSet
{
public:
  typedef std::set<walker::Type> Collection;

  WalkerTypeSet() {}

  WalkerTypeSet(walker::Type a, walker::Type b)
  {
    _data.insert(a);
    _data.insert(b);
  }

  size_t count(walker::Type type) const { return _data.count(type); }

  WalkerTypeSet& operator << ( walker::Type a )
  {
    _data.insert(a);
    return *this;
  }

  VariantList save() const
  {
    VariantList ret;
    for (auto& item : _data)
      ret.push_back(item);

    return ret;
  }

  void load(const VariantList& stream)
  {
    for(const auto& item : stream )
      _data.insert(item.toEnum<walker::Type>());
  }

protected:
  Collection _data;
};

#endif //_CAESARIA_WALKERTYPESET_INCLUDE_H_
