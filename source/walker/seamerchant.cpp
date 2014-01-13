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

  void resolveState(PlayerCityPtr city, WalkerPtr wlk);
  Pathway findNearbyDock(const DockList& docks, TilePos position );
  void goAwayFromCity( PlayerCityPtr city, WalkerPtr walker );
  DockPtr findLandingDock(PlayerCityPtr city, WalkerPtr walker );
  Pathway findRandomRaid(const DockList& docks, TilePos position);
};

SeaMerchant::SeaMerchant(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setAnimation( gfx::seaMerchant );
  _setType( walker::seaMerchant );
  _d->waitInterval = 0;
  _d->tryDockCount = 0;
  _d->maxTryDockCount = 3;

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
      CityHelper helper( city );
      DockList docks = helper.find<Dock>( building::dock );

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

        if( freeDocks.empty() )
        {
          pathway = findRandomRaid( docks, wlk->getIJ() );
          nextState = stWaitFreeDock;
        }
        else
        {
          pathway = findNearbyDock( freeDocks, wlk->getIJ() );
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
    waitInterval = GameDate::getTickInMonth() / 2;
    nextState = stFindDock;
  }
  break;

  case stRequestGoods:
  {
    landingDate = GameDate::current();

    bool emptyDock = city->getWalkers( walker::any, wlk->getIJ() ).empty();

    DockPtr myDock = findLandingDock( city, wlk );
    if( myDock.isValid() && emptyDock )
    {
      CityTradeOptions& options = city->getTradeOptions();
      //request goods
      for( int n = Good::wheat; n<Good::goodCount; ++n )
      {
        Good::Type goodType = (Good::Type)n;
        int needQty = buy.getFreeQty( goodType );
        int maySell = math::clamp( options.getExportLimit( goodType ) * 100, 0, needQty );

        if( maySell > 0)
        {
          GoodStock stock( goodType, maySell, maySell );
          myDock->requestGoods( stock );
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
      //try buy goods
      for( int n = Good::wheat; n<Good::goodCount; ++n )
      {
        Good::Type goodType = (Good::Type) n;
        int needQty = buy.getFreeQty( goodType );

        if( needQty > 0 )
        {
          GoodStock& stock = buy.getStock( goodType );
          myDock->exportingGoods( stock, needQty );
        }
      }

      nextState = stWaitGoods;
      waitInterval = 60;
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
    if( landingDate.getMonthToDate( GameDate::current() ) > 2 )
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
  break;

  case stGoOutFromCity:
  {
    // we have nothing to buy/sell with city, or cannot find available warehouse -> go out
    waitInterval = GameDate::getTickInMonth() / 4;
    goAwayFromCity( city, wlk );
    nextState = stBackToBaseCity;
  }
  break;

  case stSellGoods:
  {    
    DockPtr myDock = findLandingDock( city, wlk );

    if( myDock.isValid() )
    {
      //try sell goods
      for( int n = Good::wheat; n<Good::goodCount; ++n)
      {
        Good::Type goodType = (Good::Type)n;
        if( sell.getQty( goodType ) > 0 )
        {
          GoodStock& stock = sell.getStock( goodType );
          myDock->importingGoods( stock );
        }
      }
    }

    nextState = stBuyGoods;
    resolveState( city, wlk );
    waitInterval = GameDate::getTickInMonth() / 4;
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
    Logger::warning( "SeaMerchant: unknown state resolved" );
  }
}

Pathway SeaMerchant::Impl::findRandomRaid(const DockList& docks, TilePos position)
{
  DockList::const_iterator i = docks.begin();
  DockPtr minQueueDock;
  int minQueue = 999;

  for( DockList::const_iterator it=docks.begin(); it != docks.end(); it++ )
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
    ret = PathwayHelper::create( position, (*i)->getQueueTile().getIJ(), PathwayHelper::water );
  }

  return ret;
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

DockPtr SeaMerchant::Impl::findLandingDock(PlayerCityPtr city, WalkerPtr walker)
{
  CityHelper helper( city );
  DockList docks = helper.find<Dock>( building::dock, walker->getIJ() - TilePos( 1, 1), walker->getIJ() + TilePos( 1, 1 ) );
  foreach( DockPtr dock, docks )
  {
    if( dock->getLandingTile().getIJ() == walker->getIJ() )
    {
      return dock;
    }
  }

  return DockPtr();
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
  stream[ "state" ] = (int)_d->nextState;
}

void SeaMerchant::load( const VariantMap& stream)
{
  Walker::load( stream );
  _d->destBuildingPos = stream.get( "destBuildPos" ).toTilePos();
  _d->sell.load( stream.get( "sell" ).toMap() );
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
    _d->resolveState( _getCity(), this );
  break;

  default: break;
  }

  Walker::timeStep( time );
}

bool SeaMerchant::isWaitFreeDock() const {  return Impl::stWaitFreeDock == _d->nextState; }
WalkerPtr SeaMerchant::create(PlayerCityPtr city) {  return create( city, world::MerchantPtr() ); }

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
