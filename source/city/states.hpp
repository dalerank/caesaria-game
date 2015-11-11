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

#ifndef __CAESARIA_CITY_STATES_H_INCLUDED__
#define __CAESARIA_CITY_STATES_H_INCLUDED__

#include "world/nation.hpp"
#include "core/time.hpp"

namespace city
{

struct States
{
  unsigned int age = 0;
  world::Nation nation = world::nation::unknown;
  unsigned int population = 0;
  bool romeCity = false;
  DateTime birth;
};

}//end namespace city

#endif //__CAESARIA_CITY_STATES_H_INCLUDED__
