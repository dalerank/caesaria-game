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

#ifndef __CAESARIA_MILITARY_BUILDING_H_INCLUDED__
#define __CAESARIA_MILITARY_BUILDING_H_INCLUDED__

#include "working.hpp"
#include "constants.hpp"

class Fort : public Building
{
public:
  Fort( constants::building::Type type, int picIdLogo );
  virtual ~Fort();

  bool canBuild(PlayerCityPtr city, TilePos pos, const TilesArray &aroundTiles) const;
  void build(PlayerCityPtr city, const TilePos &pos);

  bool isNeedRoadAccess() const;
private:
  class Impl;
  ScopedPtr< Impl > _d;
};



class FortLegionnaire : public Fort
{
public:  
  FortLegionnaire();
};

class FortJaveline : public Fort
{
public:  
  FortJaveline();
};

class FortMounted : public Fort
{
public:  
  FortMounted();
};

class FortArea : public Building
{
public:
  FortArea( Fort* fort );

  bool isFlat() const;
private:
  class Impl;

  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_MILITARY_BUILDING_H_INCLUDED__
