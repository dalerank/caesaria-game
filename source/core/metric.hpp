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

namespace metric
{

static const float unit2QtyLimiter = 100.f;
class Unit;

class Qty
{
public:
  Qty() { _value = 0; }
  Qty( const Qty& other ) { *this = other; }
  explicit Qty( unsigned int value ) : _value( value ) {}

  inline operator unsigned int() const { return _value; }
  inline Qty& operator=( const Qty& other ) {_value = other._value; return *this; }
  Unit toUnits() const;

private:
  unsigned int _value;
};

class Unit
{
public:
  Unit() { _value = 0; }
  Unit( const Qty& qty ) { _value = (unsigned int)qty / unit2QtyLimiter; }
  Unit( const Unit& unit ) { *this = unit; }
  static Unit fromQty( unsigned int value ) { return Unit( value / unit2QtyLimiter ); }
  static Unit fromValue( unsigned int value ) { return Unit( value ); }
  unsigned int toQty() { return _value * unit2QtyLimiter; }
  float value() const { return _value; }
  int ivalue() const { return (int)_value; }
  bool operator>( float v ) const { return _value > v; }
  Unit& operator=( const Unit& other ) { _value=other._value; return *this; }
  inline bool operator>=( const Unit& v ) const { return _value >= v._value; }

private:
  explicit Unit( unsigned int value) : _value( value ) {}

  float _value;
};

class Measure
{
public:
  typedef enum { native=0, metric, roman, count } Mode;

  static const char* measureType()
  {
    switch( instance()._mode )
    {
    case native: return "##quantity##";
    case metric: return "##kilogram##";
    case roman:  return "##modius##";
    default: return "unknown";
    }
  }

  static const char* measureShort()
  {
    switch( instance()._mode )
    {
    case native: return "##qty##";
    case metric: return "##kg##";
    case roman:  return "##md##";
    default: return "unknown";
    }
  }

  static void setMode( Mode mode ) { instance()._mode = mode; }
  static int convQty( int qty )
  {
    switch( instance()._mode )
    {
    case native: return qty;
    case metric: return qty / 2;
    case roman: return qty / 7;
    default: return qty;
    }
  }

  static Measure& instance()
  {
    static Measure inst;
    return inst;
  }

private:
  Measure() : _mode( native ) {}

  Mode _mode;
};

inline Unit Qty::toUnits() const { return Unit::fromQty( _value ); }

}
#endif //__CAESARIA_METRIC_H_INCLUDED__
