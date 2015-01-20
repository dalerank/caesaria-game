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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_FACTORY_BUILDING_H_INCLUDE_
#define _CAESARIA_FACTORY_BUILDING_H_INCLUDE_

#include "objects/working.hpp"
#include "predefinitions.hpp"
#include "good/good.hpp"

class Factory : public WorkingBuilding
{
public:
  Factory( const good::Product inGood, const good::Product outGood,
           const TileOverlay::Type type, const Size& size );
  virtual ~Factory();

  good::Stock& inStockRef();
  const good::Stock& inStockRef() const;

  good::Stock& outStockRef();

  good::Product consumeGoodType() const;
  good::Product produceGoodType() const;

  good::Store& store();

  virtual std::string troubleDesc() const;

  // called when the factory has made 100 good units
  virtual void deliverGood();
  virtual void receiveGood();

  virtual int progress();
  virtual void updateProgress( float value );

  virtual bool isActive() const;
  virtual void setActive( bool active );

  virtual bool mayWork() const;
  virtual bool haveMaterial() const;
  virtual bool standIdle() const;

  virtual void timeStep(const unsigned long time);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual void setProductRate( const float rate );
  virtual float productRate() const;
  virtual unsigned int effciency() const;

  virtual unsigned int getFinishedQty() const;
  virtual unsigned int getConsumeQty() const;

  std::string cartStateDesc() const;
  virtual void initialize(const MetaData &mdata);

protected:
  virtual bool _mayDeliverGood() const;
  virtual void _storeChanged();
  virtual void _removeSpoiledGoods();
  void _setUnworkingInterval( unsigned int weeks );
  virtual void _reachUnworkingTreshold();

protected:
  class Impl;
  ScopedPtr< Impl > _d;
};

class Winery : public Factory
{
public:
  Winery();
  virtual bool canBuild(const CityAreaInfo& areaInfo) const;
  virtual bool build(const CityAreaInfo &info);

protected:
  virtual void _storeChanged();
};

class Creamery : public Factory
{
public:
  Creamery();

  virtual bool canBuild( const CityAreaInfo& areaInfo ) const;
  virtual bool build(const CityAreaInfo &info);
protected:
  virtual void _storeChanged();
};

#endif //_CAESARIA_FACTORY_BUILDING_H_INCLUDE_
