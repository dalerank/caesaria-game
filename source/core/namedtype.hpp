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

#ifndef __CAESARIA_NAMEDTYPE_INCLUDE_HPP__
#define __CAESARIA_NAMEDTYPE_INCLUDE_HPP__

#define BEGIN_NAMEDTYPE(name) \
class name \
{ \
protected: \
  int _type;\
public:   \
  explicit name(int which=0) { _type = which; } \
  inline int toInt() const { return _type; } \
  inline bool operator==( const name& a ) const { return _type == a._type; } \
  inline bool operator!=( const name& a ) const { return _type != a._type; } \
  inline bool operator<( const name& a ) const { return _type < a._type; } \
  inline bool operator>( const name& a ) const { return _type > a._type; } \
  inline bool operator<=( const name& a ) const { return _type <= a._type; } \
  inline name& operator++() { ++_type; return *this; }

#define END_NAMEDTYPE(name) };

#define DEFINE_NAMEDTYPE(name) \
  BEGIN_NAMEDTYPE(name) \
  END_NAMEDTYPE(name)

#endif //__CAESARIA_NAMEDTYPE_INCLUDE_HPP__
