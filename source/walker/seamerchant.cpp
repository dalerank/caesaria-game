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
#include "city/statistic.hpp"
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
#include "game/gamedate.hpp"

using namespace constants;

class SeaMerchant::Impl
{
public:  
  typedef enum { stFindDock=0,
                 stGoOutFromCity,
                 stRequestGoods,
                 stWaitFreeDock,
                 stSellGoods,
                 stBuyGoods,
                 stNothing,
                 stWaitGoods,
                 stBackToBaseCity } State;

  TilePos destBuildingPos;  // warehouse
  SimpleGoodStore sell;
  SimpleGoodStore buy;
  int tryDockCount;
  int maxTryDockCount;
  int waitInterval;
  DateTime landingDate;
  std::string baseCityName;
  State nextState;
  bool anyBuy, anySell;

  void resolveState(PlayerCityPtr city, WalkerPtr wlk);
  Pathway findNearbyDock(const DockList& docks, TilePos position );
  void goAwayFromCity( PlayerCityPtr city, WalkerPtr walker );
  DockPtr findLandingDock(PlayerCityPtr city, WalkerPtr walker );
  Pathway findRandomRaid(const DockList& docks, TilePos position);
};

SeaMerchant::SeaMerchant(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::seaMerchant );
  _d->waitInterval = 0;
  _d->tryDockCount = 0;
  _d->maxTryDockCount = 3;
  _d->anyBuy = false;
  _d->anySell = false;

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

    Pathway pathway;
    // get the list of buildings within reach   
    if( tryDockCount < maxTryDockCount )
    {
      city::Helper helper( city );
      DockList docks = helper.find<Dock>( building::dock );

      if( !docks.empty() )
      {
        DockList freeDocks;
        foreach( dock, docks )
        {
          if( !(*dock)->isBusy() )
          {
            freeDocks.push_back( *dock );
          }
        }

        if( freeDocks.empty() )
        {
          pathway = findRandomRaid( docks, wlk->pos() );
          nextState = stWaitFreeDock;
        }
        else
        {
          pathway = findNearbyDock( freeDocks, wlk->pos() );
          nextState = stRequestGoods;
        }
      }
    }
    tryDockCount++;

    if( pathway.isValid() )
    {
      // we found a destination!      
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

  case stWaitFreeDock:
  {
    waitInterval = GameDate::days2ticks( 7 );
    nextState = stFindDock;
  }
  break;

  case stRequestGoods:
  {
    landingDate = GameDate::current();

    WalkerList walkers = city->walkers( walker::any, wlk->pos() );
    foreach( it, walkers )
    {
      if( *it == wlk ) { walkers.erase( it ); break; }
    }

    bool emptyDock = walkers.empty();

    DockPtr myDock = findLandingDock( city, wlk );
    if( myDock.isValid() && emptyDock )
    {
      city::TradeOptions& options = city->tradeOptions();
      city::Statistic::GoodsMap cityGoodsAvailable = city::Statistic::getGoodsMap( city );
      //request goods
      for( int n = Good::wheat; n<Good::goodCount; n++ )
      {
        Good::Type goodType = (Good::Type)n;
        int needQty = buy.freeQty( goodType );
        if (!options.isExporting(goodType))
        {
          continue;
        }
        int maySell = math::clamp<unsigned int>( cityGoodsAvailable[ goodType ] - options.exportLimit( goodType ) * 100, 0, needQty );

        if( maySell > 0)
        {
          GoodStock stock( goodType, maySell, maySell );
          myDock->requestGoods( stock );
          anyBuy = true;
        }
      }

      nextState = stSellGoods;
      resolveState( city, wlk );
    }
    else
    {
      nextState = stFindDock;
      resolveState( city, wlk );
    }
  }
  break;

  case stBuyGoods:
  {
    DockPtr myDock = findLandingDock( city, wlk );

    if( myDock.isValid() )
    {
      city::TradeOptions& options = city->tradeOptions();
      //try buy goods
      for( int n = Good::wheat; n<Good::goodCount; ++n )
      {
        Good::Type goodType = (Good::Type) n;
        if (!options.isExporting(goodType))
        {
          continue;
        }
        int needQty = buy.freeQty( goodType );

        if( needQty > 0 )
        {
          GoodStock& stock = buy.getStock( goodType );
          myDock->exportingGoods( stock, needQty );
          anyBuy = true;
        }
      }

      nextState = stWaitGoods;
      waitInterval = anyBuy ? GameDate::days2ticks( 7 ) : 0;

      if( 0 == buy.freeQty() ) //all done
      {
        nextState = stGoOutFromCity;
      }
    }
    else
    {
      nextState = stFindDock;
      resolveState( city, wlk );
    }
  }
  break;

  case stWaitGoods:
  {
    if( !anyBuy && !anySell ) //...bad city, nothing to trade
    {
      nextState = stGoOutFromCity;
      resolveState( city, wlk );
    }
    else
    {
      if( landingDate.monthsTo( GameDate::current() ) > 2 )
      {
        nextState = stGoOutFromCity;
        resolveState( city, wlk );
      }
      else
      {
        nextState = stBuyGoods;
        resolveState( city, wlk );
      }
    }
  }
  break;

  case stGoOutFromCity:
  {
    // we have nothing to buy/sell with city, or cannot find available warehouse -> go out
    waitInterval = GameDate::days2ticks( 7 );
    goAwayFromCity( city, wlk );
    nextState = stBackToBaseCity;
  }
  break;

  case stSellGoods:
  {    
    DockPtr myDock = findLandingDock( city, wlk );
    if( myDock.isValid() )
    {
      city::TradeOptions& options = city->tradeOptions();
      const GoodStore& importing = options.importingGoods();
      //try sell goods
      for( int n = Good::wheat; n<Good::goodCount; ++n)
      {
        Good::Type type = (Good::Type)n;
        if (!options.isImporting(type))
        {
          continue;
        }
        if( sell.qty(type) > 0 && importing.capacity(type) > 0)
        {
          myDock->importingGoods( sell.getStock(type) );
          anySell = true;
        }
      }
    }

    nextState = stBuyGoods;
    resolveState( city, wlk );
    waitInterval = anySell ? GameDate::days2ticks( 7 ) : 0;
  }
  break;

  case stBackToBaseCity:
  {
    if( sell.freeQty() == 0 && buy.qty() == 0 )
    {
      wlk->setThinks( "##seamerchant_noany_trade##" );
    }

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
    Logger::warning( "SeaMerchant: unknown state resolved" );
  }
}

Pathway SeaMerchant::Impl::findRandomRaid(const DockList& docks, TilePos position)
{
  DockList::const_iterator i = docks.begin();
  DockPtr minQueueDock;
  int minQueue = 999;

  for( DockList::const_iterator it=docks.begin(); it != docks.end(); ++it )
  {
    int currentQueueSize = (*it)->getQueueSize();
    if( currentQueueSize < minQueue )
    {
      minQueue = currentQueueSize;
      minQueueDock = *it;
    }
  }

  Pathway ret;
  if( minQueueDock.isValid() )
  {
    ret = PathwayHelper::create( position, (*i)->getQueueTile().pos(), PathwayHelper::deepWater );
  }

  return ret;
}

Pathway SeaMerchant::Impl::findNearbyDock(const DockList& docks, TilePos position)
{
  DockList::const_iterator i = docks.begin();
  Pathway ret = PathwayHelper::create( position, (*i)->landingTile().pos(), PathwayHelper::deepWater );

  ++i;
  for( ; i != docks.end(); ++i )
  {
    Pathway tmp = PathwayHelper::create( position, (*i)->landingTile().pos(), PathwayHelper::deepWater );
    if( tmp.length() < ret.length() )
    {
      ret = tmp;
    }
  }
  return ret;
}

void SeaMerchant::_reachedPathway()
{
  Walker::_reachedPathway();
  _d->resolveState( _city(), this );
}

void SeaMerchant::Impl::goAwayFromCity( PlayerCityPtr city, WalkerPtr walker )
{
  Pathway pathway = PathwayHelper::create( walker->pos(), city->borderInfo().boatExit, PathwayHelper::deepWater );
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

DockPtr SeaMerchant::Impl::findLandingDock(PlayerCityPtr city, WalkerPtr walker)
{
  city::Helper helper( city );
  DockList docks = helper.find<Dock>( building::dock, walker->pos() - TilePos( 1, 1), walker->pos() + TilePos( 1, 1 ) );
  foreach( dock, docks )
  {
    if( (*dock)->landingTile().pos() == walker->pos() )
    {
      return *dock;
    }
  }

  return DockPtr();
}

void SeaMerchant::send2city()
{
  _d->nextState = Impl::stFindDock;
  setPos( _city()->borderInfo().boatEntry );
  _d->resolveState( _city(), this );

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}

void SeaMerchant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destBuildPos" ] = _d->destBuildingPos;
  stream[ "sell"  ] = _d->sell.save();
  stream[ "baseCity" ] = Variant( _d->baseCityName );
  stream[ "wait"  ] = _d->waitInterval;
  stream[ "buy"   ] = _d->buy.save();
  stream[ "state" ] = (int)_d->nextState;
}

void SeaMerchant::load( const VariantMap& stream)
{
  Walker::load( stream );
  _d->destBuildingPos = stream.get( "destBuildPos" ).toTilePos();
  _d->sell.load( stream.get( "sell" ).toMap() );
  _d->buy.load( stream.get( "buy").toMap() );
  _d->baseCityName = stream.get( "baseCity" ).toString();
  _d->waitInterval = stream.get( "wait" );
  _d->nextState = (Impl::State)stream.get( "state" ).toInt();
}

void SeaMerchant::timeStep(const unsigned long time)
{
  if( _d->waitInterval > 0 )
  {
    _d->waitInterval--;
    return;
  }

  switch( _d->nextState )
  {
  case Impl::stWaitGoods:
  case Impl::stWaitFreeDock:
    _d->resolveState( _city(), this );
  break;

  default: break;
  }

  Walker::timeStep( time );
}

bool SeaMerchant::isWaitFreeDock() const {  return Impl::stWaitFreeDock == _d->nextState; }

std::string SeaMerchant::currentThinks() const
{
  switch( _d->nextState )
  {
  case Impl::stWaitFreeDock: return "##waiting_for_free_dock##";
  case Impl::stBuyGoods: return "##docked_buying_selling_goods##";

  default: break;
  }

  return Walker::currentThinks();
}

WalkerPtr SeaMerchant::create(PlayerCityPtr city) {  return create( city, world::MerchantPtr() ); }

WalkerPtr SeaMerchant::create(PlayerCityPtr city, world::MerchantPtr merchant )
{
  SeaMerchant* cityMerchant( new SeaMerchant( city ) );
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
