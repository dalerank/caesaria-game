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

#include "oc3_market.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_walker_market_buyer.hpp"
#include "oc3_variant.hpp"
#include "oc3_scenario.hpp"
#include "oc3_goodstore_simple.hpp"

class Market::Impl
{
public:
  SimpleGoodStore goodStore;

  bool isAnyGoodStored()
  {
    bool anyGoodStored = false;
    for( int i = 0; i < G_MAX; ++i)
    {
      anyGoodStored |= ( goodStore.getCurrentQty( GoodType(i) ) >= 100 );
    }

    return anyGoodStored;
  }
};

Market::Market() : ServiceBuilding(S_MARKET, B_MARKET, Size(2) ),
  _d( new Impl )
{
  setMaxWorkers(5);
  setWorkers(5);

  setPicture( Picture::load( ResourceGroup::commerce, 1) );
  _fgPictures.resize(1);  // animation

  _d->goodStore.setMaxQty(5000);
  _d->goodStore.setMaxQty(G_WHEAT, 400);
  _d->goodStore.setMaxQty(G_POTTERY, 300);
  _d->goodStore.setCurrentQty(G_WHEAT, 200);

  _getAnimation().load( ResourceGroup::commerce, 2, 10 );
  _getAnimation().setFrameDelay( 4 );
}

void Market::deliverService()
{
  if( getWorkers() > 0 && getWalkerList().size() == 0 )
  {
    // the marketBuyer is ready to buy something!
    MarketBuyerPtr buyer = MarketBuyer::create( Scenario::instance().getCity() );
    buyer->send2City( MarketPtr( this ) );

    if( !buyer->isDeleted() )
    {
      addWalker( buyer.as<Walker>() );
    }
    else if( _d->isAnyGoodStored() )
    {
      ServiceBuilding::deliverService();
    }
  }
}

GoodStore& Market::getGoodStore()
{
  return _d->goodStore;
}

std::list<GoodType> Market::getMostNeededGoods()
{
  std::list<GoodType> res;

  std::multimap<float, GoodType> mapGoods;  // ordered by demand

  for (int n = 0; n<G_MAX; ++n)
  {
    // for all types of good
    GoodType goodType = (GoodType) n;
    GoodStock &stock = _d->goodStore.getStock(goodType);
    int demand = stock._maxQty - stock._currentQty;
    if (demand > 99)
    {
      mapGoods.insert( std::make_pair(float(stock._currentQty)/float(stock._maxQty), goodType));
    }
  }

  for (std::multimap<float, GoodType>::iterator itMap = mapGoods.begin(); itMap != mapGoods.end(); ++itMap)
  {
    GoodType goodType = itMap->second;
    res.push_back(goodType);
  }

  return res;
}


int Market::getGoodDemand(const GoodType &goodType)
{
  int res = 0;
  GoodStock &stock = _d->goodStore.getStock(goodType);
  res = stock._maxQty - stock._currentQty;
  res = (res/100)*100;  // round at the lowest century
  return res;
}

void Market::save( VariantMap& stream) const 
{
  ServiceBuilding::save( stream );
  VariantMap vm_goodstore;  
  _d->goodStore.save( vm_goodstore );
  stream[ "goodStore" ] = vm_goodstore;

  //stream.write_objectID( getWalkerList().begin().object() );
}

void Market::load( const VariantMap& stream)
{
//   ServiceBuilding::unserialize(stream);
//   _d->goodStore.unserialize(stream);
  //stream.read_objectID((void**)&_marketBuyer);
  //_d->buyerDelay = stream.read_int(2, 0, 1000);
}
