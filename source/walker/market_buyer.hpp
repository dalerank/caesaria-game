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

#ifndef __CAESARIA_MARKETLADY_H_INCLUDED__
#define __CAESARIA_MARKETLADY_H_INCLUDED__

#include "human.hpp"
#include "objects/warehouse.hpp"

/** This is the market lady buying goods at granaries and warehouses */
class MarketBuyer : public Citizen
{
  WALKER_MUST_INITIALIZE_FROM_FACTORY
public:
  static unsigned int maxBuyDistance();

  virtual ~MarketBuyer();

  void send2City( MarketPtr market );

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual std::string thoughts(Thought th) const;
  virtual TilePos places(Place type) const;

protected:
  virtual void _reachedPathway();

private:
  MarketBuyer( PlayerCityPtr city );
  // compute the destination to fetch the given good
  void _computeWalkerDestination( MarketPtr market );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_MARKETLADY_H_INCLUDED__
