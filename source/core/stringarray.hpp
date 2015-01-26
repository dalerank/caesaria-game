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

#ifndef __CAESARIA_STRINGARRAY_H_INCLUDED__
#define __CAESARIA_STRINGARRAY_H_INCLUDED__

#include <vector>
#include <string>
#include "core/math.hpp"
#include "core/foreach.hpp"

class StringArray : public std::vector< std::string >
{
public:
  inline std::string random() const
  {
    return empty() ? "" : (*this)[ (int)math::random( size() ) ];
  }

  bool contains( const std::string& str )
  {
    foreach( it, *this )
    {
      if( *it == str )
        return true;
    }

    return false;
  }

  inline StringArray& operator << ( const std::string& a )
  {
    push_back( a );
    return *this;
  }
};

#endif //__OPENCAESAR3_STRINGARRAY_H_INCLUDED__
