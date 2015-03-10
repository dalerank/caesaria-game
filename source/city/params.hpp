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

DEFINE_NAMEDTYPE(Param, unknown)
REGISTER_NAMEDTYPE(Param,culture,1)
REGISTER_NAMEDTYPE(Param,empireTaxPayed,2)
REGISTER_NAMEDTYPE(Param,overduePayment,3)
REGISTER_NAMEDTYPE(Param,maxForts,4)

class Params : public std::map<int, int>
{
public:
  int get( Param name ) const
  {
    const_iterator it = find( name );
    return it != end() ? it->second : 0;
  }

  void set( Param name, int value )
  {
    (*this)[ name ] = value;
  }
};

}//end namespace params

}//end namespace city

#endif //__CAESARIA_CITYPARAMS_H_INCLUDED__
