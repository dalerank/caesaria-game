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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_CITYPARAMS_H_INCLUDED__
#define __CAESARIA_CITYPARAMS_H_INCLUDED__

#include <map>
#include "core/namedtype.hpp"

namespace city
{

namespace params
{

BEGIN_NAMEDTYPE(Type, unknown)
APPEND_NAMEDTYPE(culture)
APPEND_NAMEDTYPE(empireTaxPayed)
APPEND_NAMEDTYPE(overduePayment)
APPEND_NAMEDTYPE(maxForts)
END_NAMEDTYPE(Type)

class Params : public std::map<int, int>
{
public:
  int get( Type name ) const
  {
    const_iterator it = find( name );
    return it != end() ? it->second : 0;
  }

  void set( Type name, int value )
  {
    (*this)[ name ] = value;
  }
};

}//end namespace params

typedef params::Type Param;

}//end namespace city

#endif //__CAESARIA_CITYPARAMS_H_INCLUDED__
