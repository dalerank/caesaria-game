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

#ifndef _CAESARIA_SEAMERCHANT_INCLUDE_H_
#define _CAESARIA_SEAMERCHANT_INCLUDE_H_

#include "merchant.hpp"
#include "world/trading.hpp"

/** This is the empire merchant which buy/sell goods at warehouses */
class SeaMerchant : public Merchant
{
  WALKER_MUST_INITIALIZE_FROM_FACTORY
public:
  virtual ~SeaMerchant();

  void send2city();

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual good::ProductMap sold() const;
  virtual good::ProductMap bougth() const;
  virtual good::ProductMap mayBuy() const;

  virtual void timeStep(const unsigned long time);

  bool isWaitFreeDock() const;

  virtual std::string thoughts(Thought th) const;
  virtual TilePos places(Place type) const;
  virtual void initialize(const VariantMap &options);

  std::string parentCity() const;

protected:
  virtual void _reachedPathway();

private:
  SeaMerchant( PlayerCityPtr city, world::MerchantPtr merchant=nullptr );

  class Impl;
  ScopedPtr< Impl > _d;
};



#endif //_CAESARIA_SEAMERCHANT_INCLUDE_H_
