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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef _CAESARIA_FACTORY_BUILDING_H_INCLUDE_
#define _CAESARIA_FACTORY_BUILDING_H_INCLUDE_

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

  GoodStock& inStockRef();
  GoodStock& outStockRef();

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

  virtual void setProductRate( const float rate );
  virtual float getProductRate() const;

  virtual unsigned int getFinishedQty() const;
  virtual unsigned int getConsumeQty() const;

protected:  
  virtual bool _mayDeliverGood() const;

protected:
  class Impl;
  ScopedPtr< Impl > _d;
};

class TimberLogger : public Factory
{
public:
   TimberLogger();
   virtual bool canBuild( PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const;  // returns true if it can be built there
};

class IronMine : public Factory
{
public:
   IronMine();
   bool canBuild(PlayerCityPtr city, TilePos pos , const TilesArray& aroundTiles) const;  // returns true if it can be built there
};

class WeaponsWorkshop : public Factory
{
public:
   WeaponsWorkshop();

   virtual bool canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const;
   virtual void build(PlayerCityPtr city, const TilePos &pos);
};

class FurnitureWorkshop : public Factory
{
public:
  virtual bool canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const;
  virtual void build(PlayerCityPtr city, const TilePos &pos);

  FurnitureWorkshop();
};

class Winery : public Factory
{
public:
   Winery();
   virtual bool canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const;
   virtual void build(PlayerCityPtr city, const TilePos &pos);
};

class Creamery : public Factory
{
public:
   Creamery();

   virtual bool canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const;
   virtual void build(PlayerCityPtr city, const TilePos &pos);
};

#endif //_CAESARIA_FACTORY_BUILDING_H_INCLUDE_
