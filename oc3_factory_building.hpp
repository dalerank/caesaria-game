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

#include "oc3_building.hpp"


class Factory: public WorkingBuilding
{
public:
   Factory(const GoodType inGood, const GoodType outGood);
   GoodStock& getInGood();
   GoodStock& getOutGood();
   SimpleGoodStore& getGoodStore();

   // called when the factory has made 100 good units
   void deliverGood();
   int getProgress();

   virtual void timeStep(const unsigned long time);

   virtual GuiInfoBox* makeInfoBox( Widget* parent );
   static std::map<GoodType, Factory*>& getSpecimen();

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

protected:
   GoodType _inGoodType;
   GoodType _outGoodType;
   SimpleGoodStore _goodStore;

   float _productionRate;  // max production / year
   float _progress;  // progress of the work, in percent (0-100).
   Picture *_stockPicture; // stock of input good
   static std::map<GoodType, Factory*> _specimen;
   // CartPusher _cartPusher;  // delivery man
};


class FactoryMarble : public Factory
{
public:
   FactoryMarble();
   virtual FactoryMarble* clone() const;
   virtual bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
   void timeStep(const unsigned long time);
};

class FactoryTimber : public Factory
{
public:
   FactoryTimber();
   virtual FactoryTimber* clone() const;
   virtual bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
};

class FactoryIron : public Factory
{
public:
   FactoryIron();
   virtual FactoryIron* clone() const;
   virtual bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
};

class FactoryClay : public Factory
{
public:
   FactoryClay();
   virtual FactoryClay* clone() const;
   virtual bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
};

class FactoryWeapon : public Factory
{
public:
   FactoryWeapon();
   virtual FactoryWeapon* clone() const;
};

class FactoryFurniture : public Factory
{
public:
   FactoryFurniture();
   virtual FactoryFurniture* clone() const;
};

class FactoryWine : public Factory
{
public:
   FactoryWine();
   virtual FactoryWine* clone() const;
};

class FactoryOil : public Factory
{
public:
   FactoryOil();
   virtual FactoryOil* clone() const;
};

class FactoryPottery : public Factory
{
public:
   FactoryPottery();
   virtual FactoryPottery* clone() const;
};


class FarmTile
{
public:
   FarmTile(const GoodType outGood, const TilePos& pos );
   void computePicture(const int percent);
   Picture& getPicture();

private:
   int _i;
   int _j;
   Picture _picture;
   Animation _animation;
};


class Farm : public Factory
{
public:
   Farm(const GoodType outGood);
   void init();

   void computePictures();
   virtual void timeStep(const unsigned long time);

protected:
   std::vector<FarmTile> _subTiles;
   Picture _pictureBuilding;  // we need to change its offset
};

class FarmWheat : public Farm
{
public:
   FarmWheat();
   virtual FarmWheat* clone() const;
};

class FarmOlive : public Farm
{
public:
   FarmOlive();
   virtual FarmOlive* clone() const;
};

class FarmGrape : public Farm
{
public:
   FarmGrape();
   virtual FarmGrape* clone() const;
};

class FarmMeat : public Farm
{
public:
   FarmMeat();
   virtual FarmMeat* clone() const;
};

class FarmFruit : public Farm
{
public:
   FarmFruit();
   virtual FarmFruit* clone() const;
};

class FarmVegetable : public Farm
{
public:
  FarmVegetable();
  virtual FarmVegetable* clone() const;
};

class Wharf : public Factory
{
public:
  Wharf();
  Wharf* clone() const;
  virtual bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
};

#endif
