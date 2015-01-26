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

#include "market.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "walker/market_buyer.hpp"
#include "core/variant_map.hpp"
#include "good/goodstore_simple.hpp"
#include "city/city.hpp"
#include "walker/serviceman.hpp"
#include "objects/constants.hpp"
#include "game/gamedate.hpp"
#include "walker/helper.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::market, Market)

class Market::Impl
{
public:
  good::SimpleStore store;

  bool isAnyGoodStored()
  {
    bool anyGoodStored = false;
    for( good::Product i = good::none; i < good::goodCount; ++i)
    {
      anyGoodStored |= ( store.qty( i ) >= 100 );
    }

    return anyGoodStored;
  }

  void initStore()
  {
    store.setCapacity(5000);
    store.setCapacity(good::wheat, 800);
    store.setCapacity(good::fish, 600);
    store.setCapacity(good::fruit, 600);
    store.setCapacity(good::meat, 600);
    store.setCapacity(good::vegetable, 600);
    store.setCapacity(good::pottery, 250);
    store.setCapacity(good::furniture, 250);
    store.setCapacity(good::oil, 250);
    store.setCapacity(good::wine, 250);
  }
};

Market::Market() : ServiceBuilding(Service::market, constants::objects::market, Size(2) ),
  _d( new Impl )
{
  _fgPicturesRef().resize(1);  // animation
  _d->initStore();

  _animationRef().load( ResourceGroup::commerce, 2, 10 );
  _animationRef().setDelay( 4 );
}

void Market::deliverService()
{
  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    // the marketBuyer is ready to buy something!
    MarketBuyerPtr buyer = MarketBuyer::create( _city() );
    buyer->send2City( this );

    if( !buyer->isDeleted() )
    {
      addWalker( buyer.object() );
    }
    else if( _d->isAnyGoodStored() )
    {
      ServiceBuilding::deliverService();
    }
  }
}

unsigned int Market::walkerDistance() const {  return 26; }
good::Store &Market::goodStore(){  return _d->store; }

std::list<good::Product> Market::mostNeededGoods()
{
  std::list<good::Product> res;

  std::multimap<float, good::Product> mapGoods;  // ordered by demand

  for( good::Product goodType = good::none; goodType < good::goodCount; ++goodType)
  {
    // for all types of good
    good::Stock &stock = _d->store.getStock(goodType);
    int demand = stock.capacity() - stock.qty();
    if (demand > 200)
    {
      mapGoods.insert( std::make_pair(float(stock.qty())/float(stock.capacity()), goodType));
    }
  }

  for( std::multimap<float, good::Product>::iterator itMap = mapGoods.begin(); itMap != mapGoods.end(); ++itMap)
  {
    good::Product goodType = itMap->second;
    res.push_back(goodType);
  }

  return res;
}


int Market::getGoodDemand(const good::Product &goodType)
{
  int res = 0;
  good::Stock &stock = _d->store.getStock(goodType);
  res = stock.capacity() - stock.qty();
  res = (res/100)*100;  // round at the lowest century
  return res;
}

void Market::save( VariantMap& stream) const 
{
  ServiceBuilding::save( stream );
  stream[ "goodStore" ] = _d->store.save();
}

void Market::load( const VariantMap& stream)
{
  ServiceBuilding::load( stream );

  _d->store.load( stream.get( "goodStore" ).toMap() );

  _d->initStore();
}

void Market::timeStep(const unsigned long time)
{
  if( game::Date::isWeekChanged() )
  {
    ServiceWalkerList servicemen;
    servicemen << walkers();
    if( servicemen.size() > 0 && _d->store.qty() == 0 )
    {
      servicemen.front()->return2Base();
    }
  }

  ServiceBuilding::timeStep( time );
}
