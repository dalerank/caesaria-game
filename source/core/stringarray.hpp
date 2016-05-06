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

#include "vector_extension.hpp"
#include <string>
#include "core/color_list.hpp"
#include "core/math.hpp"
#include "core/foreach.hpp"
#include "core/variant_list.hpp"

class StringArray : public Array<std::string>
{
public:
  StringArray& addIfValid( const std::string& str )
  {
    if (!str.empty())
      _data.push_back(str);

    return *this;
  }

  bool operator ==(const StringArray& a) const {
    return _data == a._data;
  }

  inline StringArray& operator << (const std::string& a)
  {
    push_back(a);
    return *this;
  }

  inline StringArray& operator << (const VariantList& a)
  {
    for (const auto& item : a)
      push_back(item.toString());

    return *this;
  }
};

class ColoredString
{
public:
  std::string str;
  NColor color;

  ColoredString(const std::string& r, NColor c=ColorList::black)
    : str(r), color(c)
  {}
};

class ColoredStrings : public Array<ColoredString>
{
public:
  ColoredStrings& addIfValid( const ColoredString& clrStr )
  {
    if( !clrStr.str.empty() )
      this->push_back( clrStr.str );

    return *this;
  }

  ColoredString random() const
  {
    if( this->empty() )
      return ColoredString( "no_reason", ColorList::red );

    return at( math::random( this->size() - 1 ) );
  }
};

#endif //__CAESARIA_STRINGARRAY_H_INCLUDED__
