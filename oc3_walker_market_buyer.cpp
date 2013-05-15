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
#include "oc3_positioni.hpp"
#include "oc3_market.hpp"
#include "oc3_granary.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"


class MarketBuyer::Impl
{
public:
  BuildingPtr destBuilding;  // granary or warehouse
  GoodType priorityGood;
  int maxDistance;
};

MarketBuyer::MarketBuyer() : _d( new Impl )
{
   _walkerGraphic = WG_MARKETLADY;
   _walkerType = WT_MARKET_BUYER;
   _d->maxDistance = 25;
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

template< class T >
BuildingPtr getWalkerDestination2( Propagator &pathPropagator, const BuildingType type, 
                                    MarketPtr market, SimpleGoodStore& basket, const GoodType what, 
                                    PathWay &oPathWay, long& reservId )
{
  SmartPtr< T > res;

  Propagator::ReachedBuldings pathWayList;
  pathPropagator.getReachedBuildings(type, pathWayList);

  int max_qty = 0;

  // select the warehouse with the max quantity of requested goods
  for( Propagator::ReachedBuldings::iterator pathWayIt= pathWayList.begin(); 
    pathWayIt != pathWayList.end(); ++pathWayIt)
  {
    // for every warehouse within range
    BuildingPtr building= pathWayIt->first;
    PathWay& pathWay= pathWayIt->second;

    SmartPtr< T > destBuilding = building.as< T >();
    int qty = destBuilding->getGoodStore().getMaxRetrieve( what );
    if( qty > max_qty )
    {
      res = destBuilding;
      oPathWay = pathWay;
      max_qty = qty;
    }
  }

  if( res.isValid() )
  {
    // a warehouse/granary has been found!
    // reserve some goods from that warehouse/granary
    int qty = std::min( max_qty, market->getGoodDemand( what ) );
    qty = std::min(qty, basket.getMaxQty( what ) - basket.getCurrentQty( what ));
    // std::cout << "MarketBuyer reserves from warehouse, qty=" << qty << std::endl;
    GoodStock stock( what, qty, qty);
    reservId = res->getGoodStore().reserveRetrieval( stock );
  }

  return res.template as<Building>();
}

void MarketBuyer::computeWalkerDestination()
{
   std::list<GoodType> priorityGoods = _market->getMostNeededGoods();

   _d->destBuilding = BuildingPtr();  // no destination yet

   if( priorityGoods.size() > 0 )
   {
      // we have something to buy!

      // get the list of buildings within reach
      PathWay pathWay;
      Propagator pathPropagator;
      pathPropagator.init( *_market.object() );
      pathPropagator.propagate( _d->maxDistance);

      // try to find the most needed good
      for (std::list<GoodType>::iterator itGood = priorityGoods.begin(); itGood != priorityGoods.end(); ++itGood)
      {
         _d->priorityGood = *itGood;

         if( _d->priorityGood == G_WHEAT || _d->priorityGood == G_FISH 
             || _d->priorityGood == G_MEAT || _d->priorityGood == G_FRUIT 
             || _d->priorityGood == G_VEGETABLE)
         {
            // try get that good from a granary
            _d->destBuilding = getWalkerDestination2<Granary>( pathPropagator, B_GRANARY, _market,
                                                            _basket, _d->priorityGood, pathWay, _reservationID );
         }
         else
         {
            // try get that good from a warehouse
            _d->destBuilding = getWalkerDestination2<Warehouse>( pathPropagator, B_WAREHOUSE, _market, 
                                                              _basket, _d->priorityGood, pathWay, _reservationID );
         }

         if( _d->destBuilding != NULL )
         {
            // we found a destination!
            setPathWay(pathWay);
            break;
         }
      }
   }

   if( _d->destBuilding == NULL )
   {
      // we have nothing to buy, or cannot find what we need to buy
      deleteLater();
      return;
   }

   setIJ(_pathWay.getOrigin().getIJ() );
}

void MarketBuyer::onDestination()
{
   Walker::onDestination();
   if (_pathWay.isReverse())
   {
      // walker is back in the market
      deleteLater();
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
      GranaryPtr granary = _d->destBuilding.as<Granary>();
      WarehousePtr warehouse = _d->destBuilding.as<Warehouse>();
      
      if( granary.isValid() )
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
               qty = std::min(qty, _basket.getMaxQty(_d->priorityGood) - _basket.getCurrentQty(_d->priorityGood));
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
      else if( warehouse.isValid() )
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
               qty = std::min(qty, _basket.getMaxQty(_d->priorityGood) - _basket.getCurrentQty(_d->priorityGood));
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


void MarketBuyer::send2City()
{
  computeWalkerDestination();

  if( !isDeleted() )
    Scenario::instance().getCity().addWalker( WalkerPtr( this ) );
}


void MarketBuyer::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destBuildI" ] = _d->destBuilding->getTile().getI();
  stream[ "destBuildJ" ] = _d->destBuilding->getTile().getJ();
  stream[ "priorityGood" ] = (int)_d->priorityGood;
  stream[ "marketI" ] = _market->getTile().getI();

  VariantMap vm_basket;
  _basket.save( vm_basket );
  stream[ "basket" ] = vm_basket;

  stream[ "maxDistance" ] = _d->maxDistance;
  stream[ "reservationId" ] = static_cast<unsigned int>(_reservationID);
}

void MarketBuyer::load( const VariantMap& stream)
{
//    Walker::unserialize(stream);
//    //stream.read_objectID((void**)&_destBuilding);
// 
//    _d->priorityGood = (GoodType) stream.read_int(1, 0, G_MAX);
//    stream.read_objectID((void**)&_market);
//    _basket.unserialize(stream);
//    _d->maxDistance = stream.read_int(2, 0, 65535);
//    _reservationID = stream.read_int(4, 0, 1000000);
}

MarketBuyerPtr MarketBuyer::create( MarketPtr market )
{
  MarketBuyerPtr ret( new MarketBuyer() );
  ret->drop();
  ret->_market = market;

  return ret;
}
