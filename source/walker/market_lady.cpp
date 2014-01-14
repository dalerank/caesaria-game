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

#include "market_lady.hpp"
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
#include "good/goodstore_simple.hpp"
#include "city/helper.hpp"
#include "name_generator.hpp"
#include "objects/constants.hpp"
#include "game/gamedate.hpp"

using namespace constants;

class MarketLady::Impl
{
public:
  TilePos destBuildingPos;  // granary or warehouse
  Good::Type priorityGood;
  int maxDistance;
  MarketPtr market;
  SimpleGoodStore basket;
  long reservationID;
};

MarketLady::MarketLady(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
   _setAnimation( gfx::marketlady );
   _setType( walker::marketLady );
   _d->maxDistance = 25;
   _d->basket.setCapacity(800);  // this is a big basket!

   _d->basket.setCapacity(Good::wheat, 800);
   _d->basket.setCapacity(Good::fruit, 800);
   _d->basket.setCapacity(Good::vegetable, 800);
   _d->basket.setCapacity(Good::meat, 800);
   _d->basket.setCapacity(Good::fish, 800);

   _d->basket.setCapacity(Good::pottery, 300);
   _d->basket.setCapacity(Good::furniture, 300);
   _d->basket.setCapacity(Good::oil, 300);
   _d->basket.setCapacity(Good::wine, 300);

   setName( NameGenerator::rand( NameGenerator::female ) );
}

MarketLady::~MarketLady()
{
}

template< class T >
TilePos getWalkerDestination2( Propagator &pathPropagator, const TileOverlay::Type type,
                               MarketPtr market, SimpleGoodStore& basket, const Good::Type what,
                               Pathway &oPathWay, long& reservId )
{
  SmartPtr< T > res;

  Propagator::Routes pathWayList = pathPropagator.getRoutes( type );

  int max_qty = 0;

  // select the warehouse with the max quantity of requested goods
  for( Propagator::Routes::iterator pathWayIt= pathWayList.begin(); 
    pathWayIt != pathWayList.end(); ++pathWayIt)
  {
    // for every warehouse within range
    ConstructionPtr construction = pathWayIt->first;
    Pathway& pathWay= pathWayIt->second;

    SmartPtr< T > destBuilding = construction.as< T >();
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
    qty = std::min(qty, basket.capacity( what ) - basket.getQty( what ));
    // std::cout << "MarketLady reserves from warehouse, qty=" << qty << std::endl;
    reservId = res->getGoodStore().reserveRetrieval( what, qty, GameDate::current() );
    return res->getTilePos();
  }

  return TilePos(-1, -1);
}

void MarketLady::computeWalkerDestination( MarketPtr market )
{
  _d->market = market;
  std::list<Good::Type> priorityGoods = _d->market->getMostNeededGoods();

  _d->destBuildingPos = TilePos( -1, -1 );  // no destination yet

  if( priorityGoods.size() > 0 )
  {
     // we have something to buy!

     // get the list of buildings within reach
     Pathway pathWay;
     Propagator pathPropagator( _getCity() );
     pathPropagator.init( _d->market.as<Construction>() );
     pathPropagator.setAllDirections( false );
     pathPropagator.propagate( _d->maxDistance);

     // try to find the most needed good
     foreach( Good::Type goodType, priorityGoods )
     {
        _d->priorityGood = goodType;

        if( _d->priorityGood == Good::wheat || _d->priorityGood == Good::fish
            || _d->priorityGood == Good::meat || _d->priorityGood == Good::fruit
            || _d->priorityGood == Good::vegetable)
        {
           // try get that good from a granary
           _d->destBuildingPos = getWalkerDestination2<Granary>( pathPropagator, building::granary, _d->market,
                                                              _d->basket, _d->priorityGood, pathWay, _d->reservationID );
        }
        else
        {
           // try get that good from a warehouse
           _d->destBuildingPos = getWalkerDestination2<Warehouse>( pathPropagator, building::warehouse, _d->market,
                                                                _d->basket, _d->priorityGood, pathWay, _d->reservationID );
        }

        if( _d->destBuildingPos.getI() >= 0 )
        {
           // we found a destination!
           setIJ( pathWay.getOrigin().getIJ() );
           setPathway(pathWay);
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
}

std::string MarketLady::getThinks() const
{
  if( !_pathwayRef().isReverse() )
  {
    return "##market_lady_find_goods##";
  }
  else
  {
    return "##market_lady_return##";
  }
}

void MarketLady::_reachedPathway()
{
   Walker::_reachedPathway();
   if( _pathwayRef().isReverse() )
   {
      // walker is back in the market
      deleteLater();
      // put the content of the basket in the market
      _d->market->getGoodStore().storeAll( _d->basket );
   }
   else
   {
      // walker is near the granary/warehouse
      _pathwayRef().rbegin();
      _computeDirection();
      go();

      // get goods from destination building
      TileOverlayPtr building = _getCity()->getTilemap().at( _d->destBuildingPos ).getOverlay();
      
      if( building.is<Granary>() )
      {
        GranaryPtr granary = building.as<Granary>();
        // this is a granary!
        // std::cout << "MarketLady arrives at granary, res=" << _reservationID << std::endl;
        granary->getGoodStore().applyRetrieveReservation(_d->basket, _d->reservationID);

        // take other goods if possible
        for (int n = 1; n<Good::goodCount; ++n)
        {
           // for all types of good (except G_NONE)
           Good::Type goodType = (Good::Type) n;
           int qty = _d->market->getGoodDemand(goodType) - _d->basket.getQty(goodType);
           if (qty > 0)
           {
              qty = std::min(qty, granary->getGoodStore().getMaxRetrieve(goodType));
              qty = std::min(qty, _d->basket.capacity(_d->priorityGood) - _d->basket.getQty(_d->priorityGood));
              if (qty > 0)
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
        for (int n = 1; n<Good::goodCount; ++n)
        {
           // for all types of good (except G_NONE)
           Good::Type goodType = (Good::Type) n;
           int qty = _d->market->getGoodDemand(goodType) - _d->basket.getQty(goodType);
           if (qty > 0)
           {
              qty = std::min(qty, warehouse->getGoodStore().getMaxRetrieve(goodType));
              qty = std::min(qty, _d->basket.capacity(_d->priorityGood) - _d->basket.getQty(_d->priorityGood));
              if (qty > 0)
              {
                 // std::cout << "extra retrieve qty=" << qty << " basket=" << _basket.getStock(goodType)._currentQty << std::endl;
                 GoodStock& stock = _d->basket.getStock(goodType);
                 warehouse->getGoodStore().retrieve(stock, qty);
              }
           }
        }
      }

      unsigned long delay = 20;

      while( _d->basket.getQty() > 100 )
      {
        for( int gtype=Good::none; gtype <= Good::goodCount ; gtype++ )
        {
          GoodStock& currentStock = _d->basket.getStock( (Good::Type)gtype );
          if( currentStock.qty() > 0 )
          {
            MarketKidPtr boy = MarketKid::create( _getCity(), this );
            GoodStock& boyBasket =  boy->getBasket();
            boyBasket.setType( (Good::Type)gtype );
            boyBasket.setCapacity( 100 );
            _d->basket.retrieve( boyBasket, math::clamp( currentStock.qty(), 0, 100 ) );
            boy->setDelay( delay );
            delay += 20;
            boy->send2City( _d->market );
            _d->market->addWalker( boy.as<Walker>() );
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
    _getCity()->addWalker( WalkerPtr( this ) );
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
  _d->priorityGood = (Good::Type)stream.get( "priorityGood" ).toInt();
  TilePos tpos = stream.get( "marketPos" ).toTilePos();
  CityHelper helper( _getCity() );
  _d->market = helper.find<Market>( building::market, tpos );
  _d->basket.load( stream.get( "basket" ).toMap() );
  _d->maxDistance = stream.get( "maxDistance" ).toInt();
  _d->reservationID = stream.get( "reserationId" ).toInt();
}

MarketLadyPtr MarketLady::create( PlayerCityPtr city )
{
  MarketLadyPtr ret( new MarketLady( city ) );
  ret->drop();

  return ret;
}
