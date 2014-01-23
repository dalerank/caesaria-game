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
#include "walker/walker.hpp"

class Fort : public WorkingBuilding
{
public:
  Fort( constants::building::Type type, int picIdLogo );
  virtual ~Fort();

  virtual bool canBuild(PlayerCityPtr city, TilePos pos, const TilesArray &aroundTiles) const;
  virtual void build(PlayerCityPtr city, const TilePos &pos);

  virtual bool isNeedRoadAccess() const;
  virtual float evaluateTrainee( constants::walker::Type traineeType);
  virtual void timeStep(const unsigned long time);

  virtual void destroy();

  virtual TilePos getFreeSlot() const;
  virtual void changePatrolArea();

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

protected:
  virtual void _readyNewSoldier() {}
  virtual void _setPatrolPoint( PatrolPointPtr patrolPoint );

private:  
  class Impl;
  ScopedPtr< Impl > _d;
};

class FortLegionnaire : public Fort
{
public:  
  FortLegionnaire();

  virtual void build(PlayerCityPtr city, const TilePos &pos);

protected:
  virtual void _readyNewSoldier();
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
  FortArea();

  virtual ~FortArea();

  virtual bool isFlat() const;
  virtual bool isWalkable() const;

  virtual void destroy();

  void setBase( FortPtr base );
private:
  class Impl;

  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_MILITARY_BUILDING_H_INCLUDED__
