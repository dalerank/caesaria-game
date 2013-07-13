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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef FACTORY_BUILDING_HPP
#define FACTORY_BUILDING_HPP

#include "oc3_working_building.hpp"
#include "oc3_predefinitions.hpp"

class GoodStore;

class Factory : public WorkingBuilding
{
public:
  Factory( const GoodType inGood, const GoodType outGood,
           const BuildingType type, const Size& size );
  ~Factory();

  GoodStock& getInGood();
  GoodStock& getOutGood();

  GoodType getOutGoodType() const;

  GoodStore& getGoodStore();

  // called when the factory has made 100 good units
  virtual void deliverGood();
  virtual void receiveGood();
  virtual int getProgress();

  virtual void timeStep(const unsigned long time);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

protected:
  void _setProductRate( const float rate );
  virtual bool _mayDeliverGood() const;
  virtual bool _mayWork() const;

protected:
  class Impl;
  ScopedPtr< Impl > _d;
};

class TimberLogger : public Factory
{
public:
   TimberLogger();
   bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
};

class IronMine : public Factory
{
public:
   IronMine();
   bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
};

class WeaponsWorkshop : public Factory
{
public:
   WeaponsWorkshop();
};

class FactoryFurniture : public Factory
{
public:
   FactoryFurniture();
};

class Winery : public Factory
{
public:
   Winery();
};

class FactoryOil : public Factory
{
public:
   FactoryOil();
};

class Wharf : public Factory
{
public:
  Wharf();
  virtual bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
};

#endif
