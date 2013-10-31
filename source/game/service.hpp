// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_SERVICE_H_INCLUDED__
#define __OPENCAESAR3_SERVICE_H_INCLUDED__

#include "core/enumerator.hpp"
#include "core/math.hpp"

class Service
{
public:
  typedef enum
  {
    well, fontain,
    market,
    engineer,
    senate, forum,
    prefect,
    religionNeptune, religionCeres, religionVenus, religionMars, religionMercury, oracle,
    doctor, barber, baths, hospital,
    school, library, college,
    theater, amphitheater, colloseum,
    hippodrome,
    burningRuins,
    workersRecruter,
    srvCount
  } Type;

  Service() : _value( 0 ), _min( 0 ), _max( 100 ) {}

  void set( int i ) { _value = math::clamp<int>( i, _min, _max); }
  int value() const { return _value; }

  int getMax() const { return _max; }
  void setMax( int value ) { _max = value; set( _value ); }

  operator int() const { return _value; }
  Service& operator=(  int i) { set( i ); return *this; }
  Service& operator-=(int i) { set( _value - i ); return *this; }
  Service& operator+=(int i) { set( _value + i ); return *this; }
private:
  int _value, _min, _max;
};

class ServiceHelper : public EnumsHelper<Service::Type>
{
public:
  static ServiceHelper& instance();

  static Service::Type getType(const std::string& name);
  static std::string getName( Service::Type type );

private:
  ServiceHelper();
};

#endif //__OPENCAESAR3_SERVICE_H_INCLUDED__
