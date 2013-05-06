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


#ifndef WALKER_MARKET_BUYER_HPP
#define WALKER_MARKET_BUYER_HPP

#include "oc3_walker.hpp"
#include "oc3_warehouse.hpp"


/** This is the market lady buying goods at granaries and warehouses */
class MarketBuyer : public Walker
{
public:
   MarketBuyer();
   virtual ~MarketBuyer();
   
   void setMarket(Market &_market);
   void start();
   virtual void onDestination();

   // compute the destination to fetch the given good
   void computeWalkerDestination();

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

private:
   BuildingPtr _destBuilding;  // granary or warehouse
   GoodType _priorityGood;
   Market *_market;
   SimpleGoodStore _basket;
   int _maxDistance;
   long _reservationID;
};


#endif
