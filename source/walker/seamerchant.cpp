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

#include "seamerchant.hpp"
#include "good/goodstore_simple.hpp"
#include "pathway/pathway_helper.hpp"
#include "city/helper.hpp"
#include "gfx/tile.hpp"
#include "world/empire.hpp"
#include "core/stringhelper.hpp"
#include "city/funds.hpp"
#include "city/trade_options.hpp"
#include "name_generator.hpp"
#include "gfx/tilemap.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "world/merchant.hpp"
#include "objects/dock.hpp"

using namespace constants;

class SeaMerchant::Impl
{
public:
  typedef enum { stFindDock=0,
                 stGoOutFromCity,
                 stSellGoods,
                 stBuyGoods,
                 stNothing,
                 stBackToBaseCity } State;

  TilePos destBuildingPos;  // warehouse
  SimpleGoodStore sell;
  SimpleGoodStore buy;
  int waitInterval;
  std::string baseCityName;
  State nextState;

  void resolveState(PlayerCityPtr city, WalkerPtr wlk);
  Pathway findNearbyDock(const DockList& docks, TilePos position );
  void goAwayFromCity( PlayerCityPtr city, WalkerPtr walker );
};

SeaMerchant::SeaMerchant(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setAnimation( gfx::seaMerchant );
  _setType( walker::seaMerchant );
  _d->waitInterval = 0;

  setName( NameGenerator::rand( NameGenerator::male ) );
}

SeaMerchant::~SeaMerchant()
{
}

void SeaMerchant::Impl::resolveState(PlayerCityPtr city, WalkerPtr wlk )
{
  switch( nextState )
  {
  case stFindDock:
  {
    destBuildingPos = TilePos( -1, -1 );  // no destination yet

    // get the list of buildings within reach
    CityHelper helper( city );
    DockList docks = helper.find<Dock>( building::dock );
    Pathway pathway;

    if( !docks.empty() )
    {
      DockList freeDocks;
      for( DockList::iterator dock=docks.begin(); dock != docks.end(); dock++ )
      {
        if( !(*dock)->isBusy() )
        {
          freeDocks.push_back( *dock );
        }
      }

      pathway = findNearbyDock( freeDocks.empty() ? docks : freeDocks, wlk->getIJ() );
    }

    if( pathway.isValid() )
    {
      // we found a destination!
      nextState = stSellGoods;
      wlk->setPathway( pathway );
      wlk->go();
    }
    else
    {
      nextState = stGoOutFromCity;
      resolveState( city, wlk );
    }
  }
  break;

  case stBuyGoods:
  {
    /*CityHelper helper( city );
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
          cityGoodsAvailable[ goodType ] += wh->getGoodStore().getQty( goodType );
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
    waitInterval = 60;
    resolveState( city, wlk, position );*/
  }
  break;

  case stGoOutFromCity:
  {
    // we have nothing to buy/sell with city, or cannot find available warehouse -> go out
    waitInterval = 60;
    goAwayFromCity( city, wlk );
    nextState = stBackToBaseCity;
  }
  break;

  case stSellGoods:
  {
    nextState = stGoOutFromCity;
    resolveState( city, wlk );
    /*CityHelper helper( city );
    WarehousePtr warehouse = helper.find<Warehouse>( building::warehouse, destBuildingPos );

    const GoodStore& cityOrders = city->getBuys();

    if( warehouse.isValid() )
    {
      //try sell goods
      for (int n = Good::wheat; n<Good::goodCount; ++n)
      {
        Good::Type goodType = (Good::Type)n;
        int qty4sell = sell.getQty( goodType );
        if( qty4sell > 0 && cityOrders.capacity( goodType ) > 0 )
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
    waitInterval = 60;
    resolveState( city, wlk, position );*/
  }
  break;

  case stBackToBaseCity:
  {
    // walker on exit from city
    wlk->deleteLater();
    world::EmpirePtr empire = city->getEmpire();
    const std::string& ourCityName = city->getName();
    world::TraderoutePtr route = empire->getTradeRoute( ourCityName, baseCityName );
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

Pathway SeaMerchant::Impl::findNearbyDock(const DockList& docks, TilePos position)
{
  DockList::const_iterator i = docks.begin();
  Pathway ret = PathwayHelper::create( position, (*i)->getLandingTile().getIJ(), PathwayHelper::water );

  i++;
  for( ; i != docks.end(); i++ )
  {
    Pathway tmp = PathwayHelper::create( position, (*i)->getLandingTile().getIJ(), PathwayHelper::water );
    if( tmp.getLength() < ret.getLength() )
    {
      ret = tmp;
    }
  }
  return ret;
}

void SeaMerchant::_reachedPathway()
{
  Walker::_reachedPathway();
  _d->resolveState( _getCity(), this );
}

void SeaMerchant::Impl::goAwayFromCity( PlayerCityPtr city, WalkerPtr walker )
{
  Pathway pathway = PathwayHelper::create( walker->getIJ(), city->getBorderInfo().boatExit, PathwayHelper::water );
  if( !pathway.isValid() )
  {
    walker->deleteLater();
  }
  else
  {
    walker->setPathway( pathway );
    walker->go();
  }
}

void SeaMerchant::send2city()
{
  _d->nextState = Impl::stFindDock;
  setIJ( _getCity()->getBorderInfo().boatEntry );
  _d->resolveState( _getCity(), this );

  if( !isDeleted() )
  {
    _getCity()->addWalker( this );
  }
}

void SeaMerchant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destBuildPos" ] = _d->destBuildingPos;
  stream[ "sell" ] = _d->sell.save();
  stream[ "baseCity" ] = Variant( _d->baseCityName );
  stream[ "wait" ] = _d->waitInterval;
}

void SeaMerchant::load( const VariantMap& stream)
{
  Walker::load( stream );
  _d->destBuildingPos = stream.get( "destBuildPos" ).toTilePos();
  _d->sell.load( stream.get( "sell" ).toMap() );
  _d->baseCityName = stream.get( "baseCity" ).toString();
  _d->waitInterval = stream.get( "wait" );
}

void SeaMerchant::timeStep(const unsigned long time)
{
  if( _d->waitInterval > 0 )
  {
    _d->waitInterval--;
    return;
  }

  Walker::timeStep( time );
}

WalkerPtr SeaMerchant::create(PlayerCityPtr city)
{
  return create( city, world::MerchantPtr() );
}

WalkerPtr SeaMerchant::create(PlayerCityPtr city, world::MerchantPtr merchant )
{
  SeaMerchant* cityMerchant( new SeaMerchant( city ) );
  if( merchant.isValid() )
  {
    cityMerchant->_d->sell.resize( merchant->getSellGoods() );
    cityMerchant->_d->sell.storeAll( merchant->getSellGoods() );
    cityMerchant->_d->buy.resize( merchant->getBuyGoods() );
    cityMerchant->_d->buy.storeAll( merchant->getBuyGoods() );
    cityMerchant->_d->baseCityName = merchant->getBaseCityName();
  }

  WalkerPtr ret( cityMerchant );
  ret->drop();

  return ret;
}
