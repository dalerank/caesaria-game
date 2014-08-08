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

#include "merchant.hpp"
#include "good/goodstore_simple.hpp"
#include "objects/warehouse.hpp"
#include "pathway/pathway_helper.hpp"
#include "pathway/path_finding.hpp"
#include "city/statistic.hpp"
#include "city/helper.hpp"
#include "gfx/tile.hpp"
#include "world/empire.hpp"
#include "core/stringhelper.hpp"
#include "pathway/astarpathfinding.hpp"
#include "city/funds.hpp"
#include "city/trade_options.hpp"
#include "name_generator.hpp"
#include "gfx/tilemap.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "world/merchant.hpp"
#include "core/stacktrace.hpp"
#include "events/fundissue.hpp"
#include "game/gamedate.hpp"

using namespace constants;
using namespace gfx;

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
  int waitInterval;
  std::string baseCityName;
  int maxDistance;
  State nextState;

  void resolveState( PlayerCityPtr city, WalkerPtr wlk, const TilePos& position );
};

Merchant::Merchant(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::merchant );
  _d->maxDistance = 60;
  _d->waitInterval = 0;
  _d->attemptCount = 0;

  setName( NameGenerator::rand( NameGenerator::male ) );
}

Merchant::~Merchant(){}

DirectRoute getWarehouse4Buys( Propagator &pathPropagator, SimpleGoodStore& basket, PlayerCityPtr city)
{
  DirectRoutes routes = pathPropagator.getRoutes( building::warehouse );

  std::map< int, DirectRoute > warehouseRating;

  city::TradeOptions& options = city->tradeOptions();

  // select the warehouse with the max quantity of requested goods
  DirectRoutes::iterator routeIt = routes.begin();
  while( routeIt != routes.end() )
  {
    // for every warehouse within range
    WarehousePtr warehouse = ptr_cast<Warehouse>( routeIt->first );
    int rating = 0;
    for( int i=Good::wheat; i<Good::goodCount; i++ )
    {
      Good::Type gtype = Good::Type(i);
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

DirectRoute getWarehouse4Sells( Propagator &pathPropagator,
                                            SimpleGoodStore& basket )
{
  DirectRoutes pathWayList = pathPropagator.getRoutes( building::warehouse );

  // select the warehouse with the max quantity of requested goods
  DirectRoutes::iterator pathWayIt = pathWayList.begin();
  while( pathWayIt != pathWayList.end() )
  {
    // for every warehouse within range
    WarehousePtr warehouse= ptr_cast<Warehouse>( pathWayIt->first );

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
      const GoodStore& buyOrders = city->importingGoods();

      if( buyOrders.capacity() > 0 )
      {
        route = getWarehouse4Sells( pathPropagator, sell );
      }

      if( !route.first.isValid() )
      {
        Logger::warning( "Walker_LandMerchant: can't found path to nearby warehouse. BaseCity=" + baseCityName );
        route = PathwayHelper::shortWay( city, position, building::warehouse, PathwayHelper::roadOnly );
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
      city::Helper helper( city );
      WarehousePtr warehouse = helper.find<Warehouse>( building::warehouse, destBuildingPos );

      if( warehouse.isValid() )
      {
        city::Statistic::GoodsMap cityGoodsAvailable = city::Statistic::getGoodsMap( city );

        city::TradeOptions& options = city->tradeOptions();
        GoodStore& whStore = warehouse->store();
        //try buy goods
        for( int n = Good::wheat; n<Good::goodCount; ++n )
        {
          Good::Type goodType = (Good::Type) n;
          if (!options.isExporting(goodType))
          {
            continue;
          }
          int needQty = buy.freeQty( goodType );
          int maySell = math::clamp<unsigned int>( cityGoodsAvailable[ goodType ] - options.exportLimit( goodType ) * 100, 0, 9999 );
          
          if( needQty > 0 && maySell > 0)
          {
            int mayBuy = std::min( needQty, whStore.getMaxRetrieve( goodType ) );
            mayBuy = std::min( mayBuy, maySell );
            if( mayBuy > 0 )
            {
              GoodStock& stock = buy.getStock( goodType );
              whStore.retrieve( stock, mayBuy );

              events::GameEventPtr e = events::FundIssueEvent::exportg( goodType, mayBuy );
              e->dispatch();
            }
          }
        }
      }

      if( buy.qty() == 0 )
      {
        Logger::warning( "LandMerchant: [%d,%d] wait while store buying goods on my animals", position.i(), position.j() );
        wlk->setThinks( "##landmerchant_say_about_store_goods##" );
        waitInterval = GameDate::days2ticks( 7 );
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
      city::Helper helper( city );
      WarehousePtr warehouse = helper.find<Warehouse>( building::warehouse, destBuildingPos );

      const GoodStore& cityOrders = city->importingGoods();

      if( warehouse.isValid() )
      {
        city::TradeOptions& options = city->tradeOptions();
        //try sell goods
        for (int n = Good::wheat; n<Good::goodCount; ++n)
        {
          Good::Type goodType = (Good::Type)n;
          if (!options.isImporting(goodType))
          {
            continue;
          }
          int qty4sell = sell.qty( goodType );
          if( qty4sell > 0 && cityOrders.capacity( goodType ) > 0 )
          {
            int maySells = std::min( qty4sell, warehouse->store().getMaxStore( goodType ) );
            if( maySells != 0 )
            {
              // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
              GoodStock& stock = sell.getStock( goodType );
              warehouse->store().store( stock, maySells );
              
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
    _city()->addWalker( this );
  }
}

void Merchant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destBuildPos" ] = _d->destBuildingPos;
  stream[ "sell" ] = _d->sell.save();
  stream[ "maxDistance" ] = _d->maxDistance;
  stream[ "baseCity" ] = Variant( _d->baseCityName );
  stream[ "wait" ] = _d->waitInterval;
  stream[ "nextState" ] = (int)_d->nextState;
}

void Merchant::load( const VariantMap& stream)
{
  Walker::load( stream );
  _d->destBuildingPos = stream.get( "destBuildPos" ).toTilePos();
  _d->sell.load( stream.get( "sell" ).toMap() );
  _d->maxDistance = stream.get( "maxDistance" );
  _d->baseCityName = stream.get( "baseCity" ).toString();
  _d->waitInterval = stream.get( "wait" );

  Variant vNext = stream.get( "nextState" );
  if( vNext.isValid() )
    _d->nextState = (Impl::State)vNext.toInt();
  else
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

std::string Merchant::getParentCity() const{  return _d->baseCityName; }
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
