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

#ifndef __CAESARIA_EMPEROR_H_INCLUDED__
#define __CAESARIA_EMPEROR_H_INCLUDED__

#include "predefinitions.hpp"
#include "core/scopedptr.hpp"
#include "core/variant.hpp"

namespace world
{

class Emperor
{
public:
  Emperor();
  virtual ~Emperor();

  int relation( const std::string& cityname );
  void updateRelation( const std::string& cityname, int value );

  void sendGift( const std::string& cityname, unsigned int money );
  void timeStep( unsigned int time );

  void cityTax( const std::string& cityname, unsigned int money );

  VariantMap save() const;
  void load( const VariantMap& stream );

  void init( Empire& empire );
private:
  __DECLARE_IMPL(Emperor)
};

}

#endif //__CAESARIA_EMPEROR_H_INCLUDED__
