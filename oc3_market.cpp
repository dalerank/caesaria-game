#include "oc3_market.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_walker_market_buyer.hpp"

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

  _animation.load( ResourceGroup::commerce, 2, 10 );
  _animation.setFrameDelay( 4 );
}

void Market::deliverService()
{
  if( getWorkers() > 0 && _getWalkerList().size() == 0 )
  {
    // the marketBuyer is ready to buy something!
    MarketBuyerPtr buyer = MarketBuyer::create( MarketPtr( this ) );
    buyer->send2City();

    if( !buyer->isDeleted() )
    {
      _addWalker( buyer.as<Walker>() );
    }
    else if( _d->isAnyGoodStored() )
    {
      ServiceBuilding::deliverService();
    }
  }
}

SimpleGoodStore& Market::getGoodStore()
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
//   ServiceBuilding::serialize(stream);
//   _d->goodStore.serialize(stream);
  //stream.write_objectID( _getWalkerList().begin().object() );
  //stream.write_int( _d->buyerDelay, 2, 0, 65535 );
}

void Market::load( const VariantMap& stream)
{
//   ServiceBuilding::unserialize(stream);
//   _d->goodStore.unserialize(stream);
  //stream.read_objectID((void**)&_marketBuyer);
  //_d->buyerDelay = stream.read_int(2, 0, 1000);
}
