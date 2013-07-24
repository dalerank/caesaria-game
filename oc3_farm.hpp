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

#ifndef __OPENCAESAR3_FARM_H_INCLUDED__
#define __OPENCAESAR3_FARM_H_INCLUDED__

#include "oc3_factory_building.hpp"

class Farm : public Factory
{
public:
  Farm(const GoodType outGood, const BuildingType type );
  virtual ~Farm();
  void init();

  void computePictures();

  virtual void timeStep(const unsigned long time);
  virtual bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

protected:
  class Impl;
  ScopedPtr< Impl > _d;
};

class FarmWheat : public Farm
{
public:
  FarmWheat();
};

class FarmOlive : public Farm
{
public:
  FarmOlive();
};

class FarmGrape : public Farm
{
public:
  FarmGrape();
};

class FarmMeat : public Farm
{
public:
  FarmMeat();
};

class FarmFruit : public Farm
{
public:
  FarmFruit();
};

class FarmVegetable : public Farm
{
public:
  FarmVegetable();
};

typedef SmartPtr< Farm > FarmPtr;

#endif//__OPENCAESAR3_FARM_H_INCLUDED__