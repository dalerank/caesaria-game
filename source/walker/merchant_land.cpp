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

#include "merchant_land.hpp"
#include "good/storage.hpp"
#include "objects/warehouse.hpp"
#include "pathway/pathway_helper.hpp"
#include "pathway/path_finding.hpp"
#include "city/statistic.hpp"
#include "core/variant_map.hpp"
#include "gfx/tile.hpp"
#include "world/empire.hpp"
#include "core/utils.hpp"
#include "pathway/astarpathfinding.hpp"
#include "game/funds.hpp"
#include "city/trade_options.hpp"
#include "name_generator.hpp"
#include "gfx/tilemap.hpp"
#include "events/event.hpp"
#include "good/helper.hpp"
#include "merchant_camel.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "world/merchant.hpp"
#include "core/stacktrace.hpp"
#include "merchant_camel.hpp"
#include "events/fundissue.hpp"
#include "game/gamedate.hpp"
#include "gfx/helper.hpp"
#include "walkers_factory.hpp"

using namespace gfx;
using namespace city;
using namespace events;

REGISTER_CLASS_IN_WALKERFACTORY(walker::merchant, LandMerchant)

namespace wh_picker
{
void checkForTradeCenters( DirectPRoutes& routes )
{
  DirectPRoutes tradecenterWayList;
  for( auto route : routes )
  {
    auto warehouse = route.first.as<Warehouse>();
    if( warehouse->isTradeCenter() )
      tradecenterWayList[ route.first ] = route.second;
  }

  //if we found trade centers in city, clear all other ways
  if( !tradecenterWayList.empty() )
    routes = tradecenterWayList;
}

DirectRoute get4Buys( Propagator &pathPropagator, good::Storage& basket, PlayerCityPtr city)
{
  DirectPRoutes routes = pathPropagator.getRoutes( object::warehouse );

  //try found trade centers in city
  checkForTradeCenters( routes );

  std::map< int, DirectRoute > warehouseRating;

  trade::Options& options = city->tradeOptions();

  // select the warehouse with the max quantity of requested goods
  DirectPRoutes::iterator routeIt = routes.begin();
  while( routeIt != routes.end() )
  {
    // for every warehouse within range
    auto warehouse = routeIt->first.as<Warehouse>();
    int rating = 0;
    for( auto& gtype : good::all() )
    {
      if (!options.isExporting( gtype ) )
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

DirectRoute get4Sells( Propagator &pathPropagator, good::Storage& basket )
{
  DirectPRoutes pathWayList = pathPropagator.getRoutes( object::warehouse );

  //try found trade centers in city
  checkForTradeCenters( pathWayList );

  // select the warehouse with the max quantity of requested goods
  DirectPRoutes::iterator pathWayIt = pathWayList.begin();
  while( pathWayIt != pathWayList.end() )
  {
    // for every warehouse within range
    auto warehouse = pathWayIt->first.as<Warehouse>();

    if( warehouse->store().freeQty() == 0 ) { pathWayList.erase( pathWayIt++ );}
    else { ++pathWayIt; }
  }

  //have only available warehouses, find nearest of it
  DirectRoute shortest = pathPropagator.getShortestRoute( pathWayList );

  return shortest;
}

}//end namespace wh_picker

class LandMerchant::Impl
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

  TilePos destination;  // warehouse
  good::Storage sell;
  good::Storage buy;
  int attemptCount;
  int waitInterval;
  std::string baseCityName;
  struct
  {
    int sell;
    int buys;
  } money;

  int maxDistance;
  MerchantCamelList camels;
  State nextState;

public:
  void resolveState( PlayerCityPtr city, WalkerPtr wlk, const TilePos& position );
  void setCamelsGo(int delay );
};

LandMerchant::LandMerchant(PlayerCityPtr city )
  : Merchant( city ), _d( new Impl )
{
  _setType( walker::merchant );
  _d->maxDistance = 60;
  _d->waitInterval = 0;
  _d->attemptCount = 0;
  _d->money.buys = 0;
  _d->money.sell = 0;

  setName( NameGenerator::rand( NameGenerator::male ) );
}

LandMerchant::~LandMerchant(){}

void LandMerchant::Impl::resolveState(PlayerCityPtr city, WalkerPtr wlk, const TilePos& position )
{
  switch( nextState )
  {
  case stFindWarehouseForSelling:
    {
      destination = gfx::tilemap::invalidLocation();  // no destination yet

      // get the list of buildings within reach
      Propagator pathPropagator( city );
      Tilemap& tmap = city->tilemap();
      pathPropagator.init( tmap.at( position ) );
      pathPropagator.setAllDirections( false );
      pathPropagator.propagate( maxDistance );
      DirectRoute route;

      //try found any available warehouse for selling our goods
      const good::Store& buyOrders = city->buys();

      if( buyOrders.capacity() > 0 )
      {
        route = wh_picker::get4Sells( pathPropagator, sell );
      }

      if( !route.first.isValid() )
      {
        Logger::warning( "!!! WARNING: LandMerchant can't found path to nearby warehouse. BaseCity=" + baseCityName );
        route = PathwayHelper::shortWay( city, position, object::warehouse, PathwayHelper::roadOnly );
      }

      if( route.first.isValid()  )
      {
        // we found a destination!
        nextState = stSellGoods;
        destination = route.first->pos();
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
      destination = gfx::tilemap::invalidLocation();  // no destination yet

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
        route = wh_picker::get4Buys( pathPropagator, buy, city );
      }
      
      if( route.first.isValid() )
      {
        // we found a destination!
        nextState = stBuyGoods;
        destination = route.first->pos();
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
      auto warehouse = city->getOverlay( destination ).as<Warehouse>();

      if( warehouse.isValid() )
      {
        float tradeKoeff = warehouse->tradeBuff( Warehouse::sellGoodsBuff );
        good::ProductMap cityGoodsAvailable = city->statistic().goods.inWarehouses();

        trade::Options& options = city->tradeOptions();
        good::Store& whStore = warehouse->store();
        //try buy goods
        for( auto& goodType : good::all() )
        {
          if (!options.isExporting(goodType))
          {
            continue;
          }

          int needQty = buy.freeQty( goodType );
          int exportLimit = options.tradeLimit( trade::exporting, goodType ).toQty();
          int citySellQty = math::clamp<unsigned int>( cityGoodsAvailable[ goodType ] - exportLimit, 0, 9999 );
          
          if( needQty > 0 && citySellQty > 0)
          {
            int mayBuy = std::min( needQty, whStore.getMaxRetrieve( goodType ) );
            mayBuy = std::min( mayBuy, citySellQty );
            if( mayBuy > 0 )
            {
              good::Stock& stock = buy.getStock( goodType );
              whStore.retrieve( stock, mayBuy );

              money.buys += good::Helper::exportPrice( city, goodType, mayBuy );

              GameEventPtr e = Payment::exportg( goodType, mayBuy, tradeKoeff );
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
        setCamelsGo( waitInterval );
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
    auto warehouse = city->getOverlay( destination ).as<Warehouse>();

    const good::Store& cityOrders = city->buys();

    if( warehouse.isValid() )
    {
      good::ProductMap storedGoods = city->statistic().goods.inWarehouses();
      trade::Options& options = city->tradeOptions();
      //try sell goods

      for( auto& goodType : good::all() )
      {
        if (!options.isImporting(goodType))
        {
          continue;
        }

        int importLimit = options.tradeLimit( trade::importing, goodType ).toQty();
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

            money.sell += good::Helper::importPrice( city, goodType, maySells );

            GameEventPtr e = Payment::import( goodType, maySells );
            e->dispatch();
          }
        }
      }
    }    

    nextState = stFindWarehouseForBuying;
    waitInterval = 60;
    setCamelsGo( waitInterval );

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

    for( auto it : camels )
      it->deleteLater();

    nextState = stNothing;
  }
  break;

  default:
    Logger::warning( "LandMerchant: unknown state resolved" );
  }
}

void LandMerchant::Impl::setCamelsGo( int delay )
{
  for( auto camel : camels )
  {
    camel->wait( delay );
    if( !delay )
      camel->go();
  }
}

void LandMerchant::_reachedPathway()
{
  Walker::_reachedPathway();
  _d->resolveState( _city(), this, pos() );
}

void LandMerchant::send2city()
{
  _d->nextState = Impl::stFindWarehouseForSelling;
  setPos( _city()->borderInfo().roadEntry );
  _d->resolveState( _city(), this, pos() );

  if( !isDeleted() )
  {
    attach();

    for( int i=0; i < 2; i++ )
    {
      MerchantCamelPtr camel = MerchantCamel::create( _city(), this, 15 * (i+1) );
      camel->attach();
    }
  }
}

void LandMerchant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, destination )
  VARIANT_SAVE_ANY_D( stream, _d, maxDistance )
  VARIANT_SAVE_STR_D( stream, _d, baseCityName )
  VARIANT_SAVE_ANY_D( stream, _d, waitInterval )
  VARIANT_SAVE_ANY_D( stream, _d, money.sell )
  VARIANT_SAVE_ANY_D( stream, _d, money.buys )
  VARIANT_SAVE_ENUM_D( stream, _d, nextState )
  VARIANT_SAVE_CLASS_D( stream, _d, sell )
  VARIANT_SAVE_CLASS_D( stream, _d, buy )
}

void LandMerchant::load( const VariantMap& stream)
{
  Walker::load( stream );
  VARIANT_LOAD_ANY_D( _d, destination, stream )
  VARIANT_LOAD_ANY_D( _d, maxDistance, stream )
  VARIANT_LOAD_STR_D( _d, baseCityName, stream )
  VARIANT_LOAD_ANY_D( _d, waitInterval, stream )
  VARIANT_LOAD_ANY_D( _d, money.sell, stream )
  VARIANT_LOAD_ANY_D( _d, money.buys, stream )
  VARIANT_LOAD_ENUM_D( _d, nextState, stream );
  VARIANT_LOAD_CLASS_D( _d, sell, stream )
  VARIANT_LOAD_CLASS_D( _d, buy, stream )

  if( _d->nextState == Impl::stUnknown )
  {
    _d->nextState = Impl::stBackToBaseCity;
    _d->resolveState( _city(), this, pos() );
  }
}

void LandMerchant::setPathway(const Pathway& pathway)
{
  Human::setPathway( pathway );

  for( auto camel : _d->camels )
  {
    Pathway newPath = PathwayHelper::create( camel->pos(), pathway.stopPos(), PathwayHelper::roadFirst );
    camel->setPathway( newPath );
    camel->wait( 0 );
  }
}

void LandMerchant::timeStep(const unsigned long time)
{
  if( _d->waitInterval > 0 )
  {
    _d->waitInterval--;
    return;
  }

  Walker::timeStep( time );
}

std::string LandMerchant::thoughts(Walker::Thought th) const
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
      if( _d->money.buys >0 || _d->money.sell > 0 )
      {
        return "##merchant_just_unloading_my_goods##";
      }
    break;

    case Impl::stBackToBaseCity:
      if( _d->money.sell - _d->money.buys < 0 )
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

good::ProductMap LandMerchant::sold() const { return _d->sell.details(); }
good::ProductMap LandMerchant::bougth() const { return _d->buy.details(); }
good::ProductMap LandMerchant::mayBuy() const { return _d->buy.amounts(); }

std::string LandMerchant::parentCity() const{ return _d->baseCityName; }

TilePos LandMerchant::places(Walker::Place type) const
{
  switch( type )
  {
  case plDestination: return _d->destination;
  default: break;
  }

  return Human::places( type );
}

void LandMerchant::addCamel(MerchantCamelPtr camel)
{
  _d->camels.push_back( camel );
}

void LandMerchant::_centerTile()
{
  Human::_centerTile();

  for( auto camel : _d->camels )
    camel->updateHeadLocation( pos() );
}

WalkerPtr LandMerchant::create(PlayerCityPtr city) {  return create( city, world::MerchantPtr() ).object(); }

LandMerchantPtr LandMerchant::create(PlayerCityPtr city, world::MerchantPtr merchant )
{
  LandMerchantPtr cityMerchant( new LandMerchant( city ) );
  cityMerchant->drop();

  if( merchant.isValid() )
  {
    cityMerchant->_d->sell.resize( merchant->sellGoods() );
    cityMerchant->_d->sell.storeAll( merchant->sellGoods() );
    cityMerchant->_d->buy.resize( merchant->buyGoods() );
    cityMerchant->_d->buy.storeAll( merchant->buyGoods() );
    cityMerchant->_d->baseCityName = merchant->baseCity();
  }

  return cityMerchant;
}
