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



#include <walker_cart_pusher.hpp>
#include <building_data.hpp>
#include <exception.hpp>
#include <scenario.hpp>
#include <iostream>



CartPusher::CartPusher()
{
   _cartPicture = NULL;
   _walkerGraphic = WG_PUSHER;
   _walkerType = WT_CART_PUSHER;
   _producerBuilding = NULL;
   _consumerBuilding = NULL;
   _maxDistance = 25;
}

CartPusher* CartPusher::clone() const
{
   return new CartPusher(*this);
}


void CartPusher::onDestination()
{
   Walker::onDestination();
   _cartPicture = NULL;

   Granary *granary = dynamic_cast<Granary*> (_consumerBuilding);
   Warehouse *warehouse = dynamic_cast<Warehouse*> (_consumerBuilding);
   Factory *factory = dynamic_cast<Factory*> (_consumerBuilding);
   if (granary != NULL)
   {
      granary->getGoodStore().applyStorageReservation(_stock, _reservationID);
      granary->computePictures();
      _reservationID = 0;
   }
   else if (warehouse != NULL)
   {
      warehouse->getGoodStore().applyStorageReservation(_stock, _reservationID);
      warehouse->computePictures();
      _reservationID = 0;
   }
   else if (factory != NULL)
   {
      factory->getGoodStore().applyStorageReservation(_stock, _reservationID);
      // factory->computePictures();
      _reservationID = 0;
   }
   _isDeleted = true;
}


void CartPusher::setStock(const GoodStock &stock)
{
   _stock = stock;
}

void CartPusher::setProducerBuilding(Building &building)
{
   _producerBuilding = &building;
}

void CartPusher::setConsumerBuilding(Building &building)
{
   _consumerBuilding = &building;
}

Building &CartPusher::getProducerBuilding()
{
   if (_producerBuilding == NULL) THROW("ProducerBuilding is not initialized");
   return *_producerBuilding;
}

Building &CartPusher::getConsumerBuilding()
{
   if (_consumerBuilding == NULL) THROW("ConsumerBuilding is not initialized");
   return *_consumerBuilding;
}

Picture& CartPusher::getCartPicture()
{
   if (_cartPicture == NULL)
   {
      _cartPicture = &CartLoader::instance().getCart(_stock, getDirection());
   }

   return *_cartPicture;
}

void CartPusher::onNewDirection()
{
   Walker::onNewDirection();
   _cartPicture = NULL;  // need to get the new graphic
}

void CartPusher::getPictureList(std::vector<Picture*> &oPics)
{
   oPics.clear();

   // depending on the walker direction, the cart is ahead or behind
   switch (getDirection())
   {
   case D_WEST:
   case D_NORTH_WEST:
   case D_NORTH:
   case D_NORTH_EAST:
      oPics.push_back(&getCartPicture());
      oPics.push_back(&getMainPicture());
      break;
   case D_EAST:
   case D_SOUTH_EAST:
   case D_SOUTH:
   case D_SOUTH_WEST:
      oPics.push_back(&getMainPicture());
      oPics.push_back(&getCartPicture());
      break;
   default:
      break;
   }
}


void CartPusher::computeWalkerDestination()
{
   // get the list of buildings within reach
   PathWay pathWay;
   Propagator pathPropagator;
   pathPropagator.init(*_producerBuilding);
   pathPropagator.propagate(_maxDistance);

   Building *destBuilding = NULL;
   if (destBuilding == NULL)
   {
      // try send that good to a factory
      destBuilding = getWalkerDestination_factory(pathPropagator, pathWay);
   }

   if (destBuilding == NULL)
   {
      // try send that good to a granary
      destBuilding = getWalkerDestination_granary(pathPropagator, pathWay);
   }

   if (destBuilding == NULL)
   {
      // try send that good to a warehouse
      destBuilding = getWalkerDestination_warehouse(pathPropagator, pathWay);
   }

   if (destBuilding == NULL)
   {
      _isDeleted = true;  // no destination!
      return;
   }

   setConsumerBuilding(*destBuilding);
   setPathWay(pathWay);
   setIJ(_pathWay.getOrigin().getI(), _pathWay.getOrigin().getJ());
}


Building* CartPusher::getWalkerDestination_factory(Propagator &pathPropagator, PathWay &oPathWay)
{
   Building* res = NULL;
   GoodType goodType = _stock._goodType;
   BuildingType buildingType = BuildingDataHolder::instance().getBuildingTypeByInGood(goodType);

   if (buildingType == B_NONE)
   {
      // no factory can use this good
      return NULL;
   }

   std::map<Building*, PathWay> pathWayList;
   pathPropagator.getReachedBuildings(buildingType, pathWayList);

   for (std::map<Building*, PathWay>::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every factory within range
      Building* building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      Factory& factory= (Factory&) *building;
      _reservationID = factory.getGoodStore().reserveStorage(_stock);
      if (_reservationID != 0)
      {
         res= &factory;
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}


Warehouse* CartPusher::getWalkerDestination_warehouse(Propagator &pathPropagator, PathWay &oPathWay)
{
   Warehouse* res = NULL;

   std::map<Building*, PathWay> pathWayList;
   pathPropagator.getReachedBuildings(B_WAREHOUSE, pathWayList);

   for (std::map<Building*, PathWay>::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every warehouse within range
      Building* building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      Warehouse& warehouse= (Warehouse&) *building;
      _reservationID = warehouse.getGoodStore().reserveStorage(_stock);
      if (_reservationID != 0)
      {
         res= &warehouse;
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}


Granary* CartPusher::getWalkerDestination_granary(Propagator &pathPropagator, PathWay &oPathWay)
{
   Granary* res = NULL;

   GoodType goodType = _stock._goodType;
   if (!(goodType == G_WHEAT || goodType == G_FISH || goodType == G_MEAT || goodType == G_FRUIT || goodType == G_VEGETABLE))
   {
      // this good cannot be stored in a granary
      return NULL;
   }

   std::map<Building*, PathWay> pathWayList;
   pathPropagator.getReachedBuildings(B_GRANARY, pathWayList);

   // find a granary with enough storage
   for (std::map<Building*, PathWay>::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every granary within range
      Building* building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      Granary& granary= (Granary&) *building;
      _reservationID = granary.getGoodStore().reserveStorage(_stock);
      if (_reservationID != 0)
      {
         res= &granary;
         oPathWay = pathWay;
         break;
      }
   }

   return res;
}


void CartPusher::start()
{
   computeWalkerDestination();
}


