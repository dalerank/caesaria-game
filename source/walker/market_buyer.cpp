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

#include "market_buyer.hpp"
#include "objects/metadata.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "objects/market.hpp"
#include "objects/granary.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tile.hpp"
#include "core/variant.hpp"
#include "pathway/path_finding.hpp"
#include "market_kid.hpp"
#include "good/storage.hpp"
#include "city/helper.hpp"
#include "name_generator.hpp"
#include "core/variant_map.hpp"
#include "objects/constants.hpp"
#include "game/gamedate.hpp"
#include "gfx/helper.hpp"
#include "walkers_factory.hpp"
#include "city/trade_options.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::marketBuyer, MarketBuyer)

enum {goodCapacity=300, foodCapacity=800, maxCapacity=800};

class MarketBuyer::Impl
{
public:
  TilePos destBuildingPos;  // granary or warehouse
  good::Product priorityGood;
  int maxDistance;
  MarketPtr market;
  good::Storage basket;
  int reservationID;
};

MarketBuyer::MarketBuyer(PlayerCityPtr city )
  : Human( city ), _d( new Impl )
{
   _setType( walker::marketBuyer );
   _d->maxDistance = 25;
   _d->basket.setCapacity(maxCapacity);  // this is a big basket!

   _d->basket.setCapacity(good::wheat, foodCapacity);
   _d->basket.setCapacity(good::fruit, foodCapacity);
   _d->basket.setCapacity(good::vegetable, foodCapacity);
   _d->basket.setCapacity(good::meat, foodCapacity);
   _d->basket.setCapacity(good::fish, foodCapacity);

   _d->basket.setCapacity(good::pottery, goodCapacity);
   _d->basket.setCapacity(good::furniture, goodCapacity);
   _d->basket.setCapacity(good::oil, goodCapacity);
   _d->basket.setCapacity(good::wine, goodCapacity);

   setName( NameGenerator::rand( NameGenerator::female ) );
}

MarketBuyer::~MarketBuyer(){}

template< class T >
TilePos getWalkerDestination2( Propagator &pathPropagator, const object::Type type,
                               MarketPtr market, good::Storage& basket, const good::Product what,
                               Pathway& oPathWay, int& reservId )
{
  SmartPtr< T > res;

  DirectPRoutes pathWayList = pathPropagator.getRoutes( type );

  int max_qty = 0;

  // select the warehouse with the max quantity of requested goods
  foreach( routeIt, pathWayList )
  {
    // for every warehouse within range
    ConstructionPtr construction = routeIt->first;
    //PathwayPtr pathWay= routeIt->second;

    SmartPtr< T > destBuilding = ptr_cast<T>( construction );
    int qty = destBuilding->store().getMaxRetrieve( what );
    if( qty > max_qty )
    {
      res = destBuilding;
      oPathWay = *routeIt->second.object();
      max_qty = qty;
    }
  }

  if( res.isValid() )
  {
    // a warehouse/granary has been found!
    // reserve some goods from that warehouse/granary
    int qty = std::min( max_qty, market->getGoodDemand( what ) );
    qty = std::min(qty, basket.capacity( what ) - basket.qty( what ));
    // std::cout << "MarketLady reserves from warehouse, qty=" << qty << std::endl;
    reservId = res->store().reserveRetrieval( what, qty, game::Date::current() );
    return res->pos();
  }

  return gfx::tilemap::invalidLocation();
}

void MarketBuyer::computeWalkerDestination( MarketPtr market )
{
  _d->market = market;
  good::Products priorityGoods;
  good::Products marketGoods = _d->market->mostNeededGoods();
  
  //only look at goods that shall not be stockpiled
  foreach(goodType, marketGoods )
  {
    if( !_city()->tradeOptions().isStacking(*goodType) )
      priorityGoods.push_back(*goodType);
  }

  _d->destBuildingPos =gfx::tilemap::invalidLocation();  // no destination yet

  if( priorityGoods.size() > 0 )
  {
    // we have something to buy!
    // get the list of buildings within reach
    Pathway pathWay;
    Propagator pathPropagator( _city() );
    pathPropagator.init( ptr_cast<Construction>( _d->market ) );
    pathPropagator.setAllDirections( false );
    pathPropagator.propagate( _d->maxDistance);

    // try to find the most needed good
    foreach( goodType, priorityGoods )
    {
      _d->priorityGood = *goodType;

      if( _d->priorityGood == good::wheat || _d->priorityGood == good::fish
          || _d->priorityGood == good::meat || _d->priorityGood == good::fruit
          || _d->priorityGood == good::vegetable)
      {
        // try get that good from a granary
        _d->destBuildingPos = getWalkerDestination2<Granary>( pathPropagator, object::granery, _d->market,
                                                              _d->basket, _d->priorityGood, pathWay, _d->reservationID );

        if( !gfx::tilemap::isValidLocation( _d->destBuildingPos ) )
        {
          _d->destBuildingPos = getWalkerDestination2<Warehouse>( pathPropagator, object::warehouse, _d->market,
                                                                _d->basket, _d->priorityGood, pathWay, _d->reservationID );
        }
      }
      else
      {
        // try get that good from a warehouse
        _d->destBuildingPos = getWalkerDestination2<Warehouse>( pathPropagator, object::warehouse, _d->market,
                                                                _d->basket, _d->priorityGood, pathWay, _d->reservationID );
      }

      if( gfx::tilemap::isValidLocation( _d->destBuildingPos ) )
      {
        // we found a destination!
        setPos( pathWay.startPos() );
        setPathway( pathWay );
         break;
      }
    }
  }

  if( !gfx::tilemap::isValidLocation( _d->destBuildingPos ) )
  {
    // we have nothing to buy, or cannot find what we need to buy
    deleteLater();
    return;
  }
}

std::string MarketBuyer::thoughts( Thought th) const
{
  switch( th )
  {
  case thCurrent:
    if( !pathway().isReverse() )
    {
      return "##marketBuyer_find_goods##";
    }
    else
    {
      return "##marketBuyer_return##";
    }
  break;

  default:
  break;
  }

  return "";
}

TilePos MarketBuyer::places(Walker::Place type) const
{
  switch( type )
  {
  case plOrigin: return _d->market.isValid() ? _d->market->pos() : gfx::tilemap::invalidLocation();
  case plDestination: return _d->destBuildingPos;
  default: break;
  }

  return Human::places( type );
}


void MarketBuyer::_reachedPathway()
{
   Walker::_reachedPathway();
   if( _pathwayRef().isReverse() )
   {
     // walker is back in the market
     deleteLater();
     if( _d->market.isValid() )
     {
       // put the content of the basket in the market
       _d->market->goodStore().storeAll( _d->basket );
     }
   }
   else
   {
      // get goods from destination building
      OverlayPtr building = _city()->tilemap().at( _d->destBuildingPos ).overlay();
      
      if( is_kind_of<Granary>( building ) )
      {
        GranaryPtr granary = ptr_cast<Granary>( building );
        // this is a granary!
        // std::cout << "MarketLady arrives at granary, res=" << _reservationID << std::endl;
        granary->store().applyRetrieveReservation(_d->basket, _d->reservationID);

        // take other goods if possible
        for ( good::Product goodType = good::wheat; goodType<=good::vegetable; ++goodType)
        {
            if( !_city()->tradeOptions().isStacking(goodType) )
            {
                // for all types of good (except G_NONE)
                int qty = _d->market->getGoodDemand(goodType) - _d->basket.qty(goodType);
                if (qty > 0)
                {
                    qty = std::min(qty, granary->store().getMaxRetrieve(goodType));
                    qty = std::min(qty, _d->basket.capacity(_d->priorityGood) - _d->basket.qty(_d->priorityGood));
                    if (qty > 0)
                    {
                        // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
                        good::Stock& stock = _d->basket.getStock(goodType);
                        granary->store().retrieve(stock, qty);
                    }
                }
            }
        }
      }
      else if( is_kind_of<Warehouse>( building ) )
      {
        WarehousePtr warehouse = ptr_cast<Warehouse>( building );
        // this is a warehouse!
        warehouse->store().applyRetrieveReservation(_d->basket, _d->reservationID);

        // take other goods if possible
        foreach( goodType, good::all() )
        {
            if( !_city()->tradeOptions().isStacking(*goodType) )
            {
                // for all types of good (except G_NONE)
                int qty = _d->market->getGoodDemand(*goodType) - _d->basket.qty(*goodType);
                if (qty > 0)
                {
                    qty = std::min(qty, warehouse->store().getMaxRetrieve(*goodType));
                    qty = std::min(qty, _d->basket.capacity(_d->priorityGood) - _d->basket.qty(_d->priorityGood));
                    if (qty > 0)
                    {
                        // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
                        good::Stock& stock = _d->basket.getStock(*goodType);
                        warehouse->store().retrieve(stock, qty);
                    }
                }
            }
        }
      }

      unsigned long delay = 20;

      while( _d->basket.qty() > MarketKid::defaultCapacity )
      {
        foreach( gtype, good::all() )
        {
          good::Stock& currentStock = _d->basket.getStock( *gtype );
          if( currentStock.qty() > 0 )
          {
            MarketKidPtr boy = MarketKid::create( _city(), this );
            good::Stock& boyBasket =  boy->getBasket();
            boyBasket.setType( *gtype );
            boyBasket.setCapacity( MarketKid::defaultCapacity );
            _d->basket.retrieve( boyBasket, math::clamp<int>( currentStock.qty(), 0, MarketKid::defaultCapacity ) );
            boy->setDelay( delay );
            delay += 20;
            boy->send2City( _d->market );
            _d->market->addWalker( ptr_cast<Walker>( boy ) );
          }
        }
      }

      // walker is near the granary/warehouse
      _pathwayRef().move( Pathway::reverse );
      _centerTile();
      go();
   }
}

void MarketBuyer::send2City( MarketPtr market )
{
  computeWalkerDestination( market );
  attach();
}

void MarketBuyer::save( VariantMap& stream ) const
{
  Walker::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, destBuildingPos );
  stream[ "priorityGood" ] = _d->priorityGood;
  stream[ "marketPos" ] = _d->market->pos();

  stream[ "basket" ] = _d->basket.save();
  VARIANT_SAVE_ANY_D( stream, _d, maxDistance );
  VARIANT_SAVE_ANY_D( stream, _d, reservationID );
}

void MarketBuyer::load( const VariantMap& stream)
{
  Walker::load( stream );
  VARIANT_LOAD_ANY_D( _d, destBuildingPos, stream );
  _d->priorityGood = (good::Product)stream.get( "priorityGood" ).toInt();

  TilePos tpos = stream.get( "marketPos" ).toTilePos();

  _d->market << _city()->getOverlay( tpos );

  _d->basket.load( stream.get( "basket" ).toMap() );
  VARIANT_LOAD_ANY_D( _d, maxDistance, stream );
  VARIANT_LOAD_ANY_D( _d, reservationID, stream );
}

MarketBuyerPtr MarketBuyer::create( PlayerCityPtr city )
{
  MarketBuyerPtr ret( new MarketBuyer( city ) );
  ret->drop();

  return ret;
}
