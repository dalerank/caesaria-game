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

#ifndef __CAESARIA_CITY_OPTION_H_INCLUDED__
#define __CAESARIA_CITY_OPTION_H_INCLUDED__

#include "city.hpp"
#include "core/serialized_map.hpp"

namespace city
{  

PlayerCity::OptionType findOption( const std::string& name);

class Options : public SerializedMap<int,int>
{
public:
  virtual void load(const VariantList &stream );
  void resetIfNot( int name, int value );
};

}//end namespace city
#endif //__CAESARIA_CITY_OPTION_H_INCLUDED__
