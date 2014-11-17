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

#ifndef __CAESARIA_INDUSTRY_H_INCLUDE__
#define __CAESARIA_INDUSTRY_H_INCLUDE__

#include <vector>
#include "objects/constants.hpp"
#include "core/priorities.hpp"

namespace city
{

class Industry
{
public:
  typedef std::vector<constants::objects::Group> BuildingGroups;
  typedef enum
  {
    factoryAndTrade=0,
    food,
    engineering,
    water,
    prefectures,
    military,
    entertainment,
    healthAndEducation,
    administrationAndReligion,
    count
  } Type;

  static BuildingGroups toGroups(Type type);
};

typedef Priorities<Industry::Type> HirePriorities;

}

#endif //__CAESARIA_INDUSTRY_H_INCLUDE__
