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


#ifndef WALKER_CART_PUSHER_HPP
#define WALKER_CART_PUSHER_HPP

#include "oc3_walker.hpp"
#include "oc3_warehouse.hpp"
#include "oc3_factory_building.hpp"


/** This walker delivers goods */
class CartPusher : public Walker
{
public:
   CartPusher();
   virtual CartPusher *clone() const;
   static void initStatic();

   void setProducerBuilding(Building &building);
   void setConsumerBuilding(Building &building);
   Building &getProducerBuilding();
   Building &getConsumerBuilding();
   void setStock(const GoodStock &stock);

   Picture& getCartPicture();
   virtual void getPictureList(std::vector<Picture*> &oPics);
   virtual void onNewDirection();
   virtual void onDestination();

   void start();

   void computeWalkerDestination();
   Building* getWalkerDestination_factory(Propagator &pathPropagator, PathWay &oPathWay);
   Warehouse* getWalkerDestination_warehouse(Propagator &pathPropagator, PathWay &oPathWay);
   Granary* getWalkerDestination_granary(Propagator &pathPropagator, PathWay &oPathWay);

private:
   GoodStock _stock;
   Building *_producerBuilding;
   Building *_consumerBuilding;
   Picture *_cartPicture;
   int _maxDistance;
   long _reservationID;
};


#endif
