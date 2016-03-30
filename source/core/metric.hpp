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
// Copyright 2014-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_METRIC_H_INCLUDED__
#define __CAESARIA_METRIC_H_INCLUDED__

#include "core/singleton.hpp"
#include "core/platform_types.hpp"

namespace metric
{

static const float unit2QtyLimiter = 100.f;
class Unit;

class Qty
{
  unsigned int _value = 0;
public:
  Qty( const Qty& other ) { *this = other; }
  explicit Qty( unsigned int value ) : _value( value ) {}

  inline operator unsigned int() const { return _value; }
  inline Qty& operator=( const Qty& other ) {_value = other._value; return *this; }
  Unit toUnits() const;
};

class Unit
{
  float _value = 0.f;
public:
  Unit( const Qty& qty ) { _value = (unsigned int)qty / unit2QtyLimiter; }
  Unit( const Unit& unit ) { *this = unit; }
  static Unit fromQty(unsigned int value) { return Unit( (unsigned int)(value / unit2QtyLimiter)); }
  static Unit fromValue(unsigned int value) { return Unit(value); }
  unsigned int toQty() { return (int)(_value * unit2QtyLimiter); }
  float value() const { return _value; }
  int ivalue() const { return (int)_value; }
  bool operator>( float v ) const { return _value > v; }
  Unit& operator=( const Unit& other ) { _value=other._value; return *this; }
  inline bool operator>=( const Unit& v ) const { return _value >= v._value; }

private:
  explicit Unit(unsigned int value) : _value( (float)value ) {}
};

class Measure : public StaticSingleton<Measure>
{
  SET_STATICSINGLETON_FRIEND_FOR(Measure)
public:
  typedef enum { native=0, metric, roman, count } Mode;

  static const char* measureType();
  static const char* measureShort();

  static void setMode( Mode mode ) { instance()._mode = mode; }
  static int convQty( int qty );

  static Mode mode() { return instance()._mode; }
  static bool isRoman() { return instance()._mode == roman; }

private:
  Measure() : _mode( native ) {}

  Mode _mode;
};

inline Unit Qty::toUnits() const { return Unit::fromQty( _value ); }

}
#endif //__CAESARIA_METRIC_H_INCLUDED__
