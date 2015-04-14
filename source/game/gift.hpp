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

#ifndef _CAESARIA_GIFT_H_INCLUDE_
#define _CAESARIA_GIFT_H_INCLUDE_

#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "core/variant_map.hpp"

class Gift
{
public:
  typedef enum { modest, generous, lavish } Type;

  const std::string& name() const;
  const std::string& sender() const;
  const DateTime& date() const;
  int value() const;

  void load(const VariantMap& stream);
  VariantMap save() const;

  Gift& operator=(const Gift& a);

  Gift();
  Gift( const Gift& a );
  Gift( const std::string& sender, const std::string& name, int money, int data=-1 );
  ~Gift();
private:

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //_CAESARIA_GIFT_H_INCLUDE_
