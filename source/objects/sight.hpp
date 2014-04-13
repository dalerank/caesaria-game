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

#ifndef _CAESARIA_SIGHT_H_INCLUDE_
#define _CAESARIA_SIGHT_H_INCLUDE_

#include "building.hpp"

class SmallStatue : public Building
{
public:
  SmallStatue();
  bool isNeedRoadAccess() const;
};

class MediumStatue : public Building
{
public:
  MediumStatue();
  bool isNeedRoadAccess() const;
};

class BigStatue : public Building
{
public:
  BigStatue();
  bool isNeedRoadAccess() const;
};

class TriumphalArch : public Building
{
public:
  TriumphalArch();
};

#endif //_CAESARIA_BUILDING_H_INCLUDE_
