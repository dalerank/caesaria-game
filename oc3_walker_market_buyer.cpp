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



#include "oc3_walker_market_buyer.hpp"
#include "oc3_building_data.hpp"
#include "oc3_exception.hpp"
#include "oc3_scenario.hpp"
#include <iostream>


MarketBuyer::MarketBuyer()
{
   _walkerGraphic = WG_MARKETLADY;
   _walkerType = WT_MARKET_BUYER;
   _maxDistance = 25;
   _basket.setMaxQty(800);  // this is a big basket!

   _basket.setMaxQty(G_WHEAT, 800);
   _basket.setMaxQty(G_FRUIT, 800);
   _basket.setMaxQty(G_VEGETABLE, 800);
   _basket.setMaxQty(G_MEAT, 800);
   _basket.setMaxQty(G_FISH, 800);

   _basket.setMaxQty(G_POTTERY, 100);
   _basket.setMaxQty(G_FURNITURE, 100);
   _basket.setMaxQty(G_OIL, 100);
   _basket.setMaxQty(G_WINE, 100);
}

MarketBuyer::~MarketBuyer()
{
  
}

MarketBuyer* MarketBuyer::clone() const
{
   return new MarketBuyer(*this);
}

void MarketBuyer::setMarket(Market &market)
{
   _market = &market;
}

void MarketBuyer::computeWalkerDestination()
{
   std::list<GoodType> priorityGoods = _market->getMostNeededGoods();

   _destBuilding = NULL;  // no destination yet

   if (priorityGoods.size() > 0)
   {
      // we have something to buy!

      // get the list of buildings within reach
      PathWay pathWay;
      Propagator pathPropagator;
      pathPropagator.init(*_market);
      pathPropagator.propagate(_maxDistance);

      // try to find the most needed good
      for (std::list<GoodType>::iterator itGood = priorityGoods.begin(); itGood != priorityGoods.end(); ++itGood)
      {
         _priorityGood = *itGood;

         if (_priorityGood == G_WHEAT || _priorityGood == G_FISH || _priorityGood == G_MEAT || _priorityGood == G_FRUIT || _priorityGood == G_VEGETABLE)
         {
            // try get that good from a granary
            _destBuilding = getWalkerDestination_granary(pathPropagator, pathWay);
         }
         else
         {
            // try get that good from a warehouse
            _destBuilding = getWalkerDestination_warehouse(pathPropagator, pathWay);
         }

         if (_destBuilding != NULL)
         {
            // we found a destination!
            setPathWay(pathWay);
            break;
         }
      }
   }

   if (_destBuilding == NULL)
   {
      // we have nothing to buy, or cannot find what we need to buy
      _isDeleted = true;
      return;
   }

   setIJ(_pathWay.getOrigin().getI(), _pathWay.getOrigin().getJ());
   Scenario::instance().getCity().addWalker( *this );
}


Warehouse* MarketBuyer::getWalkerDestination_warehouse(Propagator &pathPropagator, PathWay &oPathWay)
{
   Warehouse* res = NULL;

   std::map<Building*, PathWay> pathWayList;
   pathPropagator.getReachedBuildings(B_WAREHOUSE, pathWayList);

   int max_qty = 0;

   // select the warehouse with the max quantity of requested goods
   for (std::map<Building*, PathWay>::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every warehouse within range
      Building* building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      Warehouse& warehouse= (Warehouse&) *building;
      int qty = warehouse.getGoodStore().getMaxRetrieve(_priorityGood);
      if (qty > max_qty)
      {
         res= &warehouse;
         oPathWay = pathWay;
         max_qty = qty;
      }
   }

   if (res != NULL)
   {
      // a warehouse has been found!
      // reserve some goods from that warehouse
      int qty = std::min(max_qty, _market->getGoodDemand(_priorityGood));
      qty = std::min(qty, _basket.getMaxQty(_priorityGood) - _basket.getCurrentQty(_priorityGood));
      // std::cout << "MarketBuyer reserves from warehouse, qty=" << qty << std::endl;
      GoodStock stock(_priorityGood, qty, qty);
      _reservationID = res->getGoodStore().reserveRetrieval(stock);
   }

   return res;
}


Granary* MarketBuyer::getWalkerDestination_granary(Propagator &pathPropagator, PathWay &oPathWay)
{
   Granary* res = NULL;

   std::map<Building*, PathWay> pathWayList;
   pathPropagator.getReachedBuildings(B_GRANARY, pathWayList);

   int max_qty = 0;

   // select the granary with the max quantity of requested goods
   for (std::map<Building*, PathWay>::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every granary within range
      Building* building= pathWayIt->first;
      PathWay& pathWay= pathWayIt->second;

      Granary& granary= (Granary&) *building;
      int qty = granary.getGoodStore().getMaxRetrieve(_priorityGood);
      if (qty > max_qty)
      {
         res= &granary;
         oPathWay = pathWay;
         max_qty = qty;
      }
   }

   if (res != NULL)
   {
      // a granary has been found!
      // reserve some goods from that granary
      int qty = std::min(max_qty, _market->getGoodDemand(_priorityGood));
      qty = std::min(qty, _basket.getMaxQty(_priorityGood) - _basket.getCurrentQty(_priorityGood));
      // std::cout << "MarketBuyer reserves from granary, qty=" << qty << std::endl;
      GoodStock stock(_priorityGood, qty, qty);
      _reservationID = res->getGoodStore().reserveRetrieval(stock);
   }

   return res;
}


void MarketBuyer::onDestination()
{
   Walker::onDestination();
   if (_pathWay.isReverse())
   {
      // walker is back in the market
      _isDeleted= true;

      // put the content of the basket in the market
      _market->getGoodStore().storeAll(_basket);
   }
   else
   {
      // walker is near the granary/warehouse
      _pathWay.rbegin();
      _action._action=WA_MOVE;
      computeDirection();

      // get goods from destination building
      Granary *granary = dynamic_cast<Granary*> (_destBuilding);
      Warehouse *warehouse = dynamic_cast<Warehouse*> (_destBuilding);
      if (granary != NULL)
      {
         // this is a granary!
         // std::cout << "MarketBuyer arrives at granary, res=" << _reservationID << std::endl;
         granary->getGoodStore().applyRetrieveReservation(_basket, _reservationID);

         // take other goods if possible
         for (int n = 1; n<G_MAX; ++n)
         {
            // for all types of good (except G_NONE)
            GoodType goodType = (GoodType) n;
            int qty = _market->getGoodDemand(goodType) - _basket.getCurrentQty(goodType);
            if (qty != 0)
            {
               qty = std::min(qty, granary->getGoodStore().getMaxRetrieve(goodType));
               qty = std::min(qty, _basket.getMaxQty(_priorityGood) - _basket.getCurrentQty(_priorityGood));
               if (qty != 0)
               {
                  // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
                  GoodStock& stock = _basket.getStock(goodType);
                  granary->getGoodStore().retrieve(stock, qty);
               }
            }
         }
         granary->computePictures();
      }
      if (warehouse != NULL)
      {
         // this is a warehouse!
         // std::cout << "Market buyer takes IRON from warehouse" << std::endl;
         // warehouse->retrieveGoods(_basket.getStock(G_IRON));
         warehouse->getGoodStore().applyRetrieveReservation(_basket, _reservationID);

         // take other goods if possible
         for (int n = 1; n<G_MAX; ++n)
         {
            // for all types of good (except G_NONE)
            GoodType goodType = (GoodType) n;
            int qty = _market->getGoodDemand(goodType) - _basket.getCurrentQty(goodType);
            if (qty != 0)
            {
               qty = std::min(qty, warehouse->getGoodStore().getMaxRetrieve(goodType));
               qty = std::min(qty, _basket.getMaxQty(_priorityGood) - _basket.getCurrentQty(_priorityGood));
               if (qty != 0)
               {
                  // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
                  GoodStock& stock = _basket.getStock(goodType);
                  warehouse->getGoodStore().retrieve(stock, qty);
               }
            }
         }
         warehouse->computePictures();
      }
   }
}


void MarketBuyer::start()
{
   computeWalkerDestination();
}


void MarketBuyer::serialize(OutputSerialStream &stream)
{
   Walker::serialize(stream);
   stream.write_objectID(_destBuilding);
   stream.write_int((int)_priorityGood, 1, 0, G_MAX);
   stream.write_objectID(_market);
   _basket.serialize(stream);
   stream.write_int(_maxDistance, 2, 0, 65535);
   stream.write_int(_reservationID, 4, 0, 1000000);
}

void MarketBuyer::unserialize(InputSerialStream &stream)
{
   Walker::unserialize(stream);
   stream.read_objectID((void**)&_destBuilding);
   _priorityGood = (GoodType) stream.read_int(1, 0, G_MAX);
   stream.read_objectID((void**)&_market);
   _basket.unserialize(stream);
   _maxDistance = stream.read_int(2, 0, 65535);
   _reservationID = stream.read_int(4, 0, 1000000);
}

