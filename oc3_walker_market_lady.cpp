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

#include "oc3_walker_market_lady.hpp"
#include "oc3_building_data.hpp"
#include "oc3_exception.hpp"
#include "oc3_scenario.hpp"
#include "oc3_positioni.hpp"
#include "oc3_market.hpp"
#include "oc3_granary.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_walker_market_lady_helper.hpp"
#include "oc3_goodstore_simple.hpp"

class MarketLady::Impl
{
public:
  TilePos destBuildingPos;  // granary or warehouse
  GoodType priorityGood;
  int maxDistance;
  CityPtr city;
  MarketPtr market;
  SimpleGoodStore basket;
  long reservationID;
};

MarketLady::MarketLady() : _d( new Impl )
{
   _walkerGraphic = WG_MARKETLADY;
   _walkerType = WT_MARKETLADY;
   _d->maxDistance = 25;
   _d->basket.setMaxQty(800);  // this is a big basket!

   _d->basket.setMaxQty(G_WHEAT, 800);
   _d->basket.setMaxQty(G_FRUIT, 800);
   _d->basket.setMaxQty(G_VEGETABLE, 800);
   _d->basket.setMaxQty(G_MEAT, 800);
   _d->basket.setMaxQty(G_FISH, 800);

   _d->basket.setMaxQty(G_POTTERY, 100);
   _d->basket.setMaxQty(G_FURNITURE, 100);
   _d->basket.setMaxQty(G_OIL, 100);
   _d->basket.setMaxQty(G_WINE, 100);
}

MarketLady::~MarketLady()
{
}

template< class T >
TilePos getWalkerDestination2( Propagator &pathPropagator, const BuildingType type, 
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
    // std::cout << "MarketLady reserves from warehouse, qty=" << qty << std::endl;
    GoodStock stock( what, qty, qty);
    reservId = res->getGoodStore().reserveRetrieval( stock );
    return res->getTilePos();
  }

  return TilePos(-1, -1);
}

void MarketLady::computeWalkerDestination( MarketPtr market )
{
  _d->market = market;
  std::list<GoodType> priorityGoods = _d->market->getMostNeededGoods();

  _d->destBuildingPos = TilePos( -1, -1 );  // no destination yet

  if( priorityGoods.size() > 0 )
  {
     // we have something to buy!

     // get the list of buildings within reach
     PathWay pathWay;
     Propagator pathPropagator( _d->city );
     pathPropagator.init( _d->market.as<Construction>() );
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
           _d->destBuildingPos = getWalkerDestination2<Granary>( pathPropagator, B_GRANARY, _d->market,
                                                              _d->basket, _d->priorityGood, pathWay, _d->reservationID );
        }
        else
        {
           // try get that good from a warehouse
           _d->destBuildingPos = getWalkerDestination2<Warehouse>( pathPropagator, B_WAREHOUSE, _d->market, 
                                                                _d->basket, _d->priorityGood, pathWay, _d->reservationID );
        }

        if( _d->destBuildingPos.getI() >= 0 )
        {
           // we found a destination!
           setPathWay(pathWay);
           break;
        }
     }
  }

  if( _d->destBuildingPos.getI() < 0)
  {
     // we have nothing to buy, or cannot find what we need to buy
     deleteLater();
     return;
  }

  setIJ( _getPathway().getOrigin().getIJ() );
}

void MarketLady::onDestination()
{
   Walker::onDestination();
   if( _getPathway().isReverse() )
   {
      // walker is back in the market
      deleteLater();
      // put the content of the basket in the market
      _d->market->getGoodStore().storeAll( _d->basket );
   }
   else
   {
      // walker is near the granary/warehouse
      _getPathway().rbegin();
      _setAction( WA_MOVE );
      computeDirection();

      // get goods from destination building
      LandOverlayPtr building = _d->city->getTilemap().at( _d->destBuildingPos ).getTerrain().getOverlay();
      
      if( building.is<Granary>() )
      {
        GranaryPtr granary = building.as<Granary>();
        // this is a granary!
        // std::cout << "MarketLady arrives at granary, res=" << _reservationID << std::endl;
        granary->getGoodStore().applyRetrieveReservation(_d->basket, _d->reservationID);

        // take other goods if possible
        for (int n = 1; n<G_MAX; ++n)
        {
           // for all types of good (except G_NONE)
           GoodType goodType = (GoodType) n;
           int qty = _d->market->getGoodDemand(goodType) - _d->basket.getCurrentQty(goodType);
           if (qty != 0)
           {
              qty = std::min(qty, granary->getGoodStore().getMaxRetrieve(goodType));
              qty = std::min(qty, _d->basket.getMaxQty(_d->priorityGood) - _d->basket.getCurrentQty(_d->priorityGood));
              if (qty != 0)
              {
                 // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
                 GoodStock& stock = _d->basket.getStock(goodType);
                 granary->getGoodStore().retrieve(stock, qty);
              }
           }
        }
      }
      else if( building.is<Warehouse>() )
      {
        WarehousePtr warehouse = building.as<Warehouse>();
        // this is a warehouse!
        // std::cout << "Market buyer takes IRON from warehouse" << std::endl;
        // warehouse->retrieveGoods(_basket.getStock(G_IRON));
        warehouse->getGoodStore().applyRetrieveReservation(_d->basket, _d->reservationID);

        // take other goods if possible
        for (int n = 1; n<G_MAX; ++n)
        {
           // for all types of good (except G_NONE)
           GoodType goodType = (GoodType) n;
           int qty = _d->market->getGoodDemand(goodType) - _d->basket.getCurrentQty(goodType);
           if (qty != 0)
           {
              qty = std::min(qty, warehouse->getGoodStore().getMaxRetrieve(goodType));
              qty = std::min(qty, _d->basket.getMaxQty(_d->priorityGood) - _d->basket.getCurrentQty(_d->priorityGood));
              if (qty != 0)
              {
                 // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
                 GoodStock& stock = _d->basket.getStock(goodType);
                 warehouse->getGoodStore().retrieve(stock, qty);
              }
           }
        }
      }

      unsigned long delay = 20;

      while( _d->basket.getCurrentQty() > 100 )
      {
        for( int gtype=G_WHEAT; gtype <= G_WINE; gtype++ )
        {
          GoodStock& currentStock = _d->basket.getStock( (GoodType)gtype );
          if( currentStock._currentQty > 0 )
          {
            MarketLadyHelperPtr boy = MarketLadyHelper::create( this );
            GoodStock& boyBasket =  boy->getBasket();
            boyBasket._goodType = (GoodType)gtype;
            boyBasket._maxQty = 100;
            _d->basket.retrieve( boyBasket, math::clamp( currentStock._currentQty, 0, 100 ) );
            boy->setDelay( delay );
            delay += 20;
            boy->send2City( _d->city, _d->market );
          }
        }
      }
   }
}

void MarketLady::send2City( MarketPtr market )
{
  computeWalkerDestination( market );

  if( !isDeleted() )
  {
    _d->city->addWalker( WalkerPtr( this ) );   
  }
}

void MarketLady::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destBuildPos" ] = _d->destBuildingPos;
  stream[ "priorityGood" ] = (int)_d->priorityGood;
  stream[ "marketPos" ] = _d->market->getTilePos();

  stream[ "basket" ] = _d->basket.save();
  stream[ "maxDistance" ] = _d->maxDistance;
  stream[ "reservationId" ] = static_cast<unsigned int>(_d->reservationID);
}

void MarketLady::load( const VariantMap& stream)
{
  Walker::load( stream );
  _d->destBuildingPos = stream.get( "destBuildPos" ).toTilePos();
  _d->priorityGood = (GoodType)stream.get( "priorityGood" ).toInt();
  TilePos tpos = stream.get( "marketPos" ).toTilePos();
  CityHelper helper( _d->city );
  _d->market = helper.getBuilding<Market>( tpos );
  _d->basket.load( stream.get( "basket" ).toMap() );
  _d->maxDistance = stream.get( "maxDistance" ).toInt();
  _d->reservationID = stream.get( "reserationId" ).toInt();
}

MarketLadyPtr MarketLady::create( CityPtr city )
{
  MarketLadyPtr ret( new MarketLady() );
  ret->_d->city = city;
  ret->drop();

  return ret;
}
