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

#include "building/working.hpp"
#include "predefinitions.hpp"
#include "game/good.hpp"

class GoodStore;

class Factory : public WorkingBuilding
{
public:
  Factory( const Good::Type inGood, const Good::Type outGood,
           const TileOverlay::Type type, const Size& size );
  ~Factory();

  GoodStock& getInGood();
  GoodStock& getOutGood();

  Good::Type getInGoodType() const;
  Good::Type getOutGoodType() const;

  GoodStore& getGoodStore();

  // called when the factory has made 100 good units
  virtual void deliverGood();
  virtual void receiveGood();

  virtual int getProgress();
  virtual void updateProgress( float value );

  virtual bool isActive() const;
  virtual void setActive( bool active );

  virtual bool mayWork() const;
  virtual bool standIdle() const;

  virtual void timeStep(const unsigned long time);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  void setProductRate( const float rate );

  virtual std::string getError() const;

protected:  
  virtual bool _mayDeliverGood() const;
  void _setError( const std::string& err );

protected:
  class Impl;
  ScopedPtr< Impl > _d;
};

class TimberLogger : public Factory
{
public:
   TimberLogger();
   virtual bool canBuild(CityPtr city, const TilePos& pos ) const;  // returns true if it can be built there
};

class IronMine : public Factory
{
public:
   IronMine();
   bool canBuild(CityPtr city, const TilePos& pos ) const;  // returns true if it can be built there
};

class WeaponsWorkshop : public Factory
{
public:
   WeaponsWorkshop();

   virtual bool canBuild(CityPtr city, const TilePos &pos) const;
};

class WorkshopFurniture : public Factory
{
public:
  virtual bool canBuild(CityPtr city, const TilePos &pos) const;

  WorkshopFurniture();
};

class Winery : public Factory
{
public:
   Winery();
   virtual bool canBuild(CityPtr city, const TilePos &pos) const;
};

class Creamery : public Factory
{
public:
   Creamery();

   virtual bool canBuild(CityPtr city, const TilePos &pos) const;
};

#endif
