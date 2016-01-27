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

#ifndef __CAESARIA_GOODHELPER_H_INCLUDED__
#define __CAESARIA_GOODHELPER_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "gfx/picture.hpp"
#include "good.hpp"
#include "city/predefinitions.hpp"
#include "core/direction.hpp"
#include "gfx/animation.hpp"

namespace good
{

class Helper
{
  friend class Info;
public:
  static Helper& instance();

  static const std::string& name(Product type);
  static good::Product type( const std::string& name );

  static float exportPrice(PlayerCityPtr city, good::Product gtype, unsigned int qty);
  static float importPrice(PlayerCityPtr city, good::Product gtype, unsigned int qty);

  static good::Product random();
  ~Helper();
private:
  Helper();

  class Impl;
  ScopedPtr< Impl > _d;
};

class Info
{
public:
  typedef enum { importing=0, exporting } PriceType;
  Info();
  Info( good::Product type );

  const std::string& name() const;
  gfx::Picture picture(bool emp=false) const;
  inline Product type() const { return _type; }

  float price( PlayerCityPtr city, PriceType inOut ) const;
private:
  Product _type;
};

}//end namespace good

#endif //__CAESARIA_GOODHELPER_H_INCLUDED__
