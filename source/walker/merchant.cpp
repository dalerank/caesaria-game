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

#include "merchant.hpp"
#include "game/goodstore_simple.hpp"
#include "building/warehouse.hpp"
#include "game/pathway_helper.hpp"
#include "game/path_finding.hpp"
#include "game/city.hpp"
#include "gfx/tile.hpp"
#include "world/empire.hpp"
#include "core/stringhelper.hpp"
#include "game/astarpathfinding.hpp"
#include "game/cityfunds.hpp"
#include "game/trade_options.hpp"
#include "game/name_generator.hpp"
#include "game/tilemap.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "building/constants.hpp"
#include "world/merchant.hpp"

using namespace constants;

class Merchant::Impl
{
public:
  typedef enum { stFindWarehouseForSelling=0,    
                 stFindWarehouseForBuying,
                 stGoOutFromCity,
                 stSellGoods,
                 stBuyGoods,
                 stNothing,
                 stBackToBaseCity } State;

  TilePos destBuildingPos;  // warehouse
  SimpleGoodStore sell;
  SimpleGoodStore buy;
  int attemptCount;
  std::string baseCityName;
  int maxDistance;
  State nextState;

  void resolveState( PlayerCityPtr city, WalkerPtr wlk, const TilePos& position );
};

Merchant::Merchant(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setAnimation( gfx::horseMerchant );
  _setType( walker::merchant );
  _d->maxDistance = 60;
  _d->attemptCount = 0;

  setName( NameGenerator::rand( NameGenerator::male ) );
}

Merchant::~Merchant()
{
}

Propagator::DirectRoute getWarehouse4Buys( Propagator &pathPropagator,
                                           SimpleGoodStore& basket )
{
  Propagator::Routes pathWayList = pathPropagator.getRoutes( building::warehouse );

  std::map< int, Propagator::DirectRoute > warehouseRating;

  // select the warehouse with the max quantity of requested goods
  Propagator::Routes::iterator pathWayIt = pathWayList.begin(); 
  while( pathWayIt != pathWayList.end() )
  {
    // for every warehouse within range
    WarehousePtr warehouse= pathWayIt->first.as< Warehouse >();

    int rating = 0;
    for( int i=Good::wheat; i<Good::goodCount; i++ )
    {
      Good::Type gtype = Good::Type(i);
      int qty = warehouse->getGoodStore().getMaxRetrieve( gtype );
      int need = basket.getFreeQty( gtype );
      rating = need > 0 ? ( qty ) : 0;
    }

    rating = math::clamp( rating - pathWayIt->second.getLength(), 0, 999 );
    warehouseRating[ rating ] = *pathWayIt; 

    pathWayIt++;
  }

  //have only available warehouses, find nearest of it
  return warehouseRating.size() > 0 ? warehouseRating.rbegin()->second : Propagator::DirectRoute();
}

Propagator::DirectRoute getWarehouse4Sells( Propagator &pathPropagator,
                                            SimpleGoodStore& basket )
{
  Propagator::Routes pathWayList = pathPropagator.getRoutes( building::warehouse );

  // select the warehouse with the max quantity of requested goods
  Propagator::Routes::iterator pathWayIt = pathWayList.begin(); 
  while( pathWayIt != pathWayList.end() )
  {
    // for every warehouse within range
    WarehousePtr warehouse= pathWayIt->first.as< Warehouse >();

    if( warehouse->getGoodStore().getFreeQty() == 0 ) { pathWayList.erase( pathWayIt++ );}
    else { pathWayIt++; }    
  }

  //have only available warehouses, find nearest of it
  Propagator::DirectRoute shortest = pathPropagator.getShortestRoute( pathWayList );

  return shortest;
}


void Merchant::Impl::resolveState(PlayerCityPtr city, WalkerPtr wlk, const TilePos& position )
{
  switch( nextState )
  {
  case stFindWarehouseForSelling:
    {
      destBuildingPos = TilePos( -1, -1 );  // no destination yet

      // get the list of buildings within reach
      Propagator pathPropagator( city );
      Tilemap& tmap = city->getTilemap();
      pathPropagator.init( tmap.at( position ) );
      pathPropagator.propagate( maxDistance );
      Propagator::DirectRoute route;

      //try found any available warehouse for selling our goods
      const GoodStore& buyOrders = city->getBuys();

      if( buyOrders.getMaxQty() > 0 )
      {
        route = getWarehouse4Sells( pathPropagator, sell );
      }

      if( !route.first.isValid() )
      {
        route = pathPropagator.getShortestRoute( building::warehouse );
      }

      if( route.first.isValid()  )
      {
        // we found a destination!
        nextState = stSellGoods;
        destBuildingPos = route.first->getTilePos();
        wlk->setIJ( route.second.getOrigin().getIJ() );
        wlk->setPathway( route.second );      
        wlk->go();
      }
      else
      {
        nextState = stGoOutFromCity;
        resolveState( city, wlk, position );
      }
    }
  break;

  case stFindWarehouseForBuying:
    {
      destBuildingPos = TilePos( -1, -1 );  // no destination yet

      // get the list of buildings within reach
      Propagator pathPropagator( city );
      Tilemap& tmap = city->getTilemap();
      pathPropagator.init( tmap.at( position ) );
      pathPropagator.propagate( maxDistance );
      Propagator::DirectRoute route;

      // try to find goods for city export 
      if( buy.getMaxQty() > 0 )
      {
        route = getWarehouse4Buys( pathPropagator, buy );
      }
      
      if( route.first.isValid() )
      {
        // we found a destination!
        nextState = stBuyGoods;
        destBuildingPos = route.first->getTilePos();    
        wlk->setIJ( route.second.getOrigin().getIJ() );
        wlk->setPathway( route.second );
        wlk->go();
      }
      else
      {
        nextState = stGoOutFromCity;
        resolveState( city, wlk, position );
      }
    }
  break;

  case stBuyGoods:
    {
      CityHelper helper( city );
      WarehousePtr warehouse = helper.find<Warehouse>( building::warehouse, destBuildingPos );

      if( warehouse.isValid() )
      {
        std::map< Good::Type, int > cityGoodsAvailable;
        WarehouseList warehouses = helper.find<Warehouse>( building::warehouse );
        foreach( WarehousePtr wh, warehouses )
        {
          for( int i=Good::wheat; i < Good::goodCount; i++ )
          {
            Good::Type goodType = (Good::Type)i;
            cityGoodsAvailable[ goodType ] += wh->getGoodStore().getCurrentQty( goodType );
          }
        }
        
        //const GoodStore& cityOrders = city->getSells();
        CityTradeOptions& options = city->getTradeOptions();
        //try buy goods
        for( int n = Good::wheat; n<Good::goodCount; ++n )
        {
          Good::Type goodType = (Good::Type) n;
          int needQty = buy.getFreeQty( goodType );
          int maySell = math::clamp( cityGoodsAvailable[ goodType ] - options.getExportLimit( goodType ) * 100, 0, 9999 );
          
          if( needQty > 0 && maySell > 0)
          {
            int mayBuy = std::min( needQty, warehouse->getGoodStore().getMaxRetrieve( goodType ) );
            mayBuy = std::min( mayBuy, maySell );
            if( mayBuy > 0 )
            {
              // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
              GoodStock& stock = buy.getStock( goodType );
              warehouse->getGoodStore().retrieve( stock, mayBuy );

              events::GameEventPtr e = events::FundIssueEvent::exportg( goodType, mayBuy );
              e->dispatch();
            }
          }
        }
      }

      nextState = stGoOutFromCity;
      resolveState( city, wlk, position );
    }
  break;

  case stGoOutFromCity:
    {
      // we have nothing to buy/sell with city, or cannot find available warehouse -> go out
      Pathway pathWay = PathwayHelper::create( position, city->getBorderInfo().roadExit, PathwayHelper::allTerrain );
      if( pathWay.isValid() )
      {
        wlk->setIJ( pathWay.getOrigin().getIJ() );
        wlk->setPathway( pathWay );
        wlk->go();
      }
      else
      {
        wlk->deleteLater();
      }

      nextState = stBackToBaseCity;
    }
  break;

  case stSellGoods:
    {
      CityHelper helper( city );
      WarehousePtr warehouse = helper.find<Warehouse>( building::warehouse, destBuildingPos );

      const GoodStore& cityOrders = city->getBuys();

      if( warehouse.isValid() )
      {
        //try sell goods
        for (int n = Good::wheat; n<Good::goodCount; ++n)
        {
          Good::Type goodType = (Good::Type)n;
          int qty4sell = sell.getCurrentQty( goodType );
          if( qty4sell > 0 && cityOrders.getMaxQty( goodType ) > 0 )
          {
            int maySells = std::min( qty4sell, warehouse->getGoodStore().getMaxStore( goodType ) );
            if( maySells != 0 )
            {
              // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
              GoodStock& stock = sell.getStock( goodType );
              warehouse->getGoodStore().store( stock, maySells );
              
              events::GameEventPtr e = events::FundIssueEvent::import( goodType, maySells );
              e->dispatch();
            }
          }
        }        
      }

      nextState = stFindWarehouseForBuying;
      resolveState( city, wlk, position );
    }
  break;

  case stBackToBaseCity:
  {
    // walker on exit from city
    wlk->deleteLater();
    world::EmpirePtr empire = city->getEmpire();
    const std::string& ourCityName = city->getName();
    world::TradeRoutePtr route = empire->getTradeRoute( ourCityName, baseCityName );
    if( route.isValid() )
    {
      route->addMerchant( ourCityName, sell, buy );
    }

    nextState = stNothing;
  }
  break;

  default:
    Logger::warning( "Merchant: unknown state resolved" );
  }
}

void Merchant::_reachedPathway()
{
  Walker::_reachedPathway();
  _d->resolveState( _getCity(), this, getIJ() );
}

void Merchant::send2City()
{
  _d->nextState = Impl::stFindWarehouseForSelling;
  _d->resolveState( _getCity(), this, _getCity()->getBorderInfo().roadEntry );

  if( !isDeleted() )
  {
    _getCity()->addWalker( this );
  }
}

void Merchant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destBuildPos" ] = _d->destBuildingPos;
  stream[ "sell" ] = _d->sell.save();
  stream[ "maxDistance" ] = _d->maxDistance;
  stream[ "baseCity" ] = Variant( _d->baseCityName );
}

void Merchant::load( const VariantMap& stream)
{
  Walker::load( stream );
  _d->destBuildingPos = stream.get( "destBuildPos" ).toTilePos();
  _d->sell.load( stream.get( "sell" ).toMap() );
  _d->maxDistance = stream.get( "maxDistance" ).toInt();
  _d->baseCityName = stream.get( "baseCity" ).toString();
}

WalkerPtr Merchant::create(PlayerCityPtr city, world::MerchantPtr merchant )
{
  Merchant* cityMerchant( new Merchant( city ) );
  cityMerchant->_d->sell.resize( merchant->getSellGoods() );
  cityMerchant->_d->sell.storeAll( merchant->getSellGoods() );
  cityMerchant->_d->buy.resize( merchant->getBuyGoods() );
  cityMerchant->_d->buy.storeAll( merchant->getBuyGoods() );
  cityMerchant->_d->baseCityName = merchant->getBaseCityName();
  
  WalkerPtr ret( cityMerchant );
  ret->drop();

  return ret;
}
