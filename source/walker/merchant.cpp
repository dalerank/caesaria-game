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

#include "merchant.hpp"
#include "good/goodstore_simple.hpp"
#include "objects/warehouse.hpp"
#include "pathway/pathway_helper.hpp"
#include "pathway/path_finding.hpp"
#include "city/statistic.hpp"
#include "core/variant_map.hpp"
#include "city/helper.hpp"
#include "gfx/tile.hpp"
#include "world/empire.hpp"
#include "core/utils.hpp"
#include "pathway/astarpathfinding.hpp"
#include "city/funds.hpp"
#include "city/trade_options.hpp"
#include "name_generator.hpp"
#include "gfx/tilemap.hpp"
#include "events/event.hpp"
#include "good/goodhelper.hpp"
#include "merchant_camel.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "world/merchant.hpp"
#include "core/stacktrace.hpp"
#include "merchant_camel.hpp"
#include "events/fundissue.hpp"
#include "game/gamedate.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;
using namespace city;

REGISTER_CLASS_IN_WALKERFACTORY(walker::merchant, Merchant)

class Merchant::Impl
{
public:
  typedef enum { stUnknown=0,
                 stFindWarehouseForSelling,
                 stFindWarehouseForBuying,
                 stGoOutFromCity,
                 stSellGoods,
                 stBuyGoods,
                 stNothing,
                 stBackToBaseCity } State;

  TilePos destBuildingPos;  // warehouse
  good::SimpleStore sell;
  good::SimpleStore buy;
  int attemptCount;
  int waitInterval;
  std::string baseCityName;
  int currentSell;
  int currentBuys;
  int maxDistance;
  MerchantCamelList camels;
  State nextState;

  void resolveState( PlayerCityPtr city, WalkerPtr wlk, const TilePos& position );
};

Merchant::Merchant(PlayerCityPtr city )
  : Human( city ), _d( new Impl )
{
  _setType( walker::merchant );
  _d->maxDistance = 60;
  _d->waitInterval = 0;
  _d->attemptCount = 0;
  _d->currentBuys = 0;
  _d->currentSell = 0;

  setName( NameGenerator::rand( NameGenerator::male ) );
}

Merchant::~Merchant(){}

DirectRoute getWarehouse4Buys( Propagator &pathPropagator, good::SimpleStore& basket, PlayerCityPtr city)
{
  DirectPRoutes routes = pathPropagator.getRoutes( objects::warehouse );

  std::map< int, DirectRoute > warehouseRating;

  trade::Options& options = city->tradeOptions();

  // select the warehouse with the max quantity of requested goods
  DirectPRoutes::iterator routeIt = routes.begin();
  while( routeIt != routes.end() )
  {
    // for every warehouse within range
    WarehousePtr warehouse = ptr_cast<Warehouse>( routeIt->first );
    int rating = 0;
    for( good::Product gtype=good::wheat; gtype<good::goodCount; ++gtype )
    {
      if (!options.isExporting(gtype))
      {
        continue;
      }
      int qty = warehouse->store().getMaxRetrieve( gtype );
      int need = basket.freeQty( gtype );
      rating = need > 0 ? ( qty ) : 0;
    }

    rating = math::clamp<int>( rating - routeIt->second->length(), 0, 999 );
    warehouseRating[ rating ] = DirectRoute( routeIt->first, *routeIt->second.object() );

    ++routeIt;
  }

  //have only available warehouses, find nearest of it
  return warehouseRating.size() > 0 ? warehouseRating.rbegin()->second : DirectRoute();
}

DirectRoute getWarehouse4Sells( Propagator &pathPropagator, good::SimpleStore& basket )
{
  DirectPRoutes pathWayList = pathPropagator.getRoutes( objects::warehouse );

  // select the warehouse with the max quantity of requested goods
  DirectPRoutes::iterator pathWayIt = pathWayList.begin();
  while( pathWayIt != pathWayList.end() )
  {
    // for every warehouse within range
    WarehousePtr warehouse;
    warehouse << pathWayIt->first;

    if( warehouse->store().freeQty() == 0 ) { pathWayList.erase( pathWayIt++ );}
    else { ++pathWayIt; }
  }

  //have only available warehouses, find nearest of it
  DirectRoute shortest = pathPropagator.getShortestRoute( pathWayList );

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
      Tilemap& tmap = city->tilemap();
      pathPropagator.init( tmap.at( position ) );
      pathPropagator.setAllDirections( false );
      pathPropagator.propagate( maxDistance );
      DirectRoute route;

      //try found any available warehouse for selling our goods
      const good::Store& buyOrders = city->importingGoods();

      if( buyOrders.capacity() > 0 )
      {
        route = getWarehouse4Sells( pathPropagator, sell );
      }

      if( !route.first.isValid() )
      {
        Logger::warning( "Walker_LandMerchant: can't found path to nearby warehouse. BaseCity=" + baseCityName );
        route = PathwayHelper::shortWay( city, position, objects::warehouse, PathwayHelper::roadOnly );
      }

      if( route.first.isValid()  )
      {
        // we found a destination!
        nextState = stSellGoods;
        destBuildingPos = route.first->pos();
        wlk->setPos( route.second.startPos() );
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
      Tilemap& tmap = city->tilemap();
      pathPropagator.init( tmap.at( position ) );
      pathPropagator.setAllDirections( false );
      pathPropagator.propagate( maxDistance );

      DirectRoute route;

      // try to find goods for city export 
      if( buy.capacity() > 0 )
      {
        route = getWarehouse4Buys( pathPropagator, buy, city );
      }
      
      if( route.first.isValid() )
      {
        // we found a destination!
        nextState = stBuyGoods;
        destBuildingPos = route.first->pos();    
        wlk->setPos( route.second.startPos() );
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
      WarehousePtr warehouse;
      warehouse << city->getOverlay( destBuildingPos );

      if( warehouse.isValid() )
      {
        float tradeKoeff = warehouse->tradeBuff( Warehouse::sellGoodsBuff );
        statistic::GoodsMap cityGoodsAvailable = statistic::getGoodsMap( city, false );

        trade::Options& options = city->tradeOptions();
        good::Store& whStore = warehouse->store();
        //try buy goods
        for( good::Product goodType = good::wheat; goodType<good::goodCount; ++goodType )
        {
          if (!options.isExporting(goodType))
          {
            continue;
          }
          int needQty = buy.freeQty( goodType );
          int exportLimit = options.tradeLimit( trade::exporting, goodType ) * 100;
          int maySell = math::clamp<unsigned int>( cityGoodsAvailable[ goodType ] - exportLimit, 0, 9999 );
          
          if( needQty > 0 && maySell > 0)
          {
            int mayBuy = std::min( needQty, whStore.getMaxRetrieve( goodType ) );
            mayBuy = std::min( mayBuy, maySell );
            if( mayBuy > 0 )
            {
              good::Stock& stock = buy.getStock( goodType );
              whStore.retrieve( stock, mayBuy );

              currentBuys += good::Helper::exportPrice( city, goodType, mayBuy );

              events::GameEventPtr e = events::FundIssueEvent::exportg( goodType, mayBuy, tradeKoeff );
              e->dispatch();
            }
          }
        }
      }

      if( buy.qty() == 0 )
      {
        Logger::warning( "LandMerchant: [%d,%d] wait while store buying goods on my animals", position.i(), position.j() );
        wlk->setThinks( "##landmerchant_say_about_store_goods##" );
        waitInterval = game::Date::days2ticks( 7 );
        foreach( it, camels )
        {
          (*it)->wait();
        }
      }

      nextState = stGoOutFromCity;
      resolveState( city, wlk, position );
    }
  break;

  case stGoOutFromCity:
    {
      if( sell.freeQty() == 0 && buy.qty() == 0 )
      {
        StringArray th;
        th << "##landmerchart_noany_trade2##";
        th << "##landmerchant_noany_trade##";
        wlk->setThinks( th.random() );
      }
      else
      {
        wlk->setThinks( "##landmerchant_good_deals##" );
      }

      // we have nothing to buy/sell with city, or cannot find available warehouse -> go out
      Pathway pathWay = PathwayHelper::create( position, city->borderInfo().roadExit, PathwayHelper::allTerrain );
      if( pathWay.isValid() )
      {
        wlk->setPos( pathWay.startPos() );
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
    WarehousePtr warehouse;
    warehouse << city->getOverlay( destBuildingPos );

    const good::Store& cityOrders = city->importingGoods();

    if( warehouse.isValid() )
    {
      statistic::GoodsMap storedGoods = statistic::getGoodsMap( city, false );
      trade::Options& options = city->tradeOptions();
      //try sell goods
      for (good::Product goodType = good::wheat; goodType<good::goodCount; ++goodType)
      {
        if (!options.isImporting(goodType))
        {
          continue;
        }

        int importLimit = options.tradeLimit( trade::importing, goodType ) * 100;
        if( importLimit == 0 )
        {
          importLimit = 9999;
        }
        else
        {
          importLimit = math::clamp<int>( importLimit - storedGoods[ goodType ], 0, 9999 );
        }

        int qty4sell = sell.qty( goodType );
        if( qty4sell > 0 && cityOrders.capacity( goodType ) > 0 )
        {
          int maySells = std::min( qty4sell, warehouse->store().getMaxStore( goodType ) );
          maySells = std::min( qty4sell, importLimit );

          if( maySells != 0 )
          {
            good::Stock& stock = sell.getStock( goodType );
            warehouse->store().store( stock, maySells );

            currentSell += good::Helper::importPrice( city, goodType, maySells );

            events::GameEventPtr e = events::FundIssueEvent::import( goodType, maySells );
            e->dispatch();
          }
        }
      }
    }

    nextState = stFindWarehouseForBuying;
    waitInterval = 60;
    resolveState( city, wlk, position );
  }
  break;

  case stBackToBaseCity:
  {
    // walker on exit from city
    wlk->deleteLater();
    world::EmpirePtr empire = city->empire();
    const std::string& ourCityName = city->name();
    world::TraderoutePtr route = empire->findRoute( ourCityName, baseCityName );
    if( route.isValid() )
    {
      route->addMerchant( ourCityName, sell, buy );
    }

    nextState = stNothing;
  }
  break;

  default:
    Logger::warning( "LandMerchant: unknown state resolved" );
  }
}

void Merchant::_reachedPathway()
{
  Walker::_reachedPathway();
  _d->resolveState( _city(), this, pos() );
}

void Merchant::send2city()
{
  _d->nextState = Impl::stFindWarehouseForSelling;
  setPos( _city()->borderInfo().roadEntry );
  _d->resolveState( _city(), this, pos() );

  if( !isDeleted() )
  {
    attach();

    for( int i=0; i < 2; i++ )
    {
      _d->camels << MerchantCamel::create( _city(), this, 15 * (i+1) );
      _d->camels.back()->attach();
    }
  }
}

void Merchant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, destBuildingPos )
  VARIANT_SAVE_ANY_D( stream, _d, maxDistance )
  VARIANT_SAVE_STR_D( stream, _d, baseCityName )
  VARIANT_SAVE_ANY_D( stream, _d, waitInterval )
  VARIANT_SAVE_ANY_D( stream, _d, currentSell )
  VARIANT_SAVE_ANY_D( stream, _d, currentBuys )
  VARIANT_SAVE_ENUM_D( stream, _d, nextState )

  stream[ "sell" ] = _d->sell.save();
}

void Merchant::load( const VariantMap& stream)
{
  Walker::load( stream );
  _d->sell.load( stream.get( "sell" ).toMap() );

  VARIANT_LOAD_ANY_D( _d, destBuildingPos, stream )
  VARIANT_LOAD_ANY_D( _d, maxDistance, stream )
  VARIANT_LOAD_STR_D( _d, baseCityName, stream )
  VARIANT_LOAD_ANY_D( _d, waitInterval, stream )
  VARIANT_LOAD_ANY_D( _d, currentSell, stream )
  VARIANT_LOAD_ANY_D( _d, currentBuys, stream )
  VARIANT_LOAD_ENUM_D( _d, nextState, stream );

  if( _d->nextState == Impl::stUnknown )
  {
    _d->nextState = Impl::stBackToBaseCity;
    _d->resolveState( _city(), this, pos() );
  }
}

void Merchant::timeStep(const unsigned long time)
{
  if( _d->waitInterval > 0 )
  {
    _d->waitInterval--;
    return;
  }

  Walker::timeStep( time );
}

std::string Merchant::thoughts(Walker::Thought th) const
{
  switch( th )
  {
  case thCurrent:
    switch( _d->nextState )
    {
    case Impl::stFindWarehouseForSelling:
    case Impl::stFindWarehouseForBuying:
      return "##heading_to_city_warehouses##";
    break;

    case Impl::stBuyGoods:
    case Impl::stSellGoods:
      return "##merchant_wait_for_deal##";
    break;

    case Impl::stGoOutFromCity:
      if( _d->currentBuys >0 || _d->currentSell > 0 )
      {
        return "##merchant_just_unloading_my_goods##";
      }
    break;

    case Impl::stBackToBaseCity:
      if( _d->currentSell - _d->currentBuys < 0 )
      {
        return "##merchant_notbad_city##";
      }
      else
      {
        return "##merchant_little_busy_now##";
      }
    break;

    default: break;
    }

  break;

  default: break;
  }

  return Human::thoughts( th );
}

std::string Merchant::parentCity() const{ return _d->baseCityName; }

TilePos Merchant::places(Walker::Place type) const
{
  switch( type )
  {
  case plDestination: return _d->destBuildingPos;
  default: break;
  }

  return Human::places( type );
}

WalkerPtr Merchant::create(PlayerCityPtr city) {  return create( city, world::MerchantPtr() ); }

WalkerPtr Merchant::create(PlayerCityPtr city, world::MerchantPtr merchant )
{
  Merchant* cityMerchant( new Merchant( city ) );
  if( merchant.isValid() )
  {
    cityMerchant->_d->sell.resize( merchant->sellGoods() );
    cityMerchant->_d->sell.storeAll( merchant->sellGoods() );
    cityMerchant->_d->buy.resize( merchant->buyGoods() );
    cityMerchant->_d->buy.storeAll( merchant->buyGoods() );
    cityMerchant->_d->baseCityName = merchant->baseCity();
  }

  WalkerPtr ret( cityMerchant );
  ret->drop();

  return ret;
}
