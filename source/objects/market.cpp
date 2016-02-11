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
#include "good/storage.hpp"
#include "city/city.hpp"
#include "walker/serviceman.hpp"
#include "objects/constants.hpp"
#include "game/gamedate.hpp"
#include "walker/helper.hpp"
#include "pathway/pathway_helper.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::market, Market)

class Market::Impl
{
public:
  good::Storage goodStore;

  bool isAnyGoodStored()
  {
    bool anyGoodStored = false;
    for( const auto& i : good::all() )
      anyGoodStored |= ( goodStore.qty( i ) >= 100 );

    return anyGoodStored;
  }

  void initStore()
  {
    goodStore.setCapacity(5000);
    goodStore.setCapacity(good::wheat, 800);
    goodStore.setCapacity(good::fish, 600);
    goodStore.setCapacity(good::fruit, 600);
    goodStore.setCapacity(good::meat, 600);
    goodStore.setCapacity(good::vegetable, 600);
    goodStore.setCapacity(good::pottery, 250);
    goodStore.setCapacity(good::furniture, 250);
    goodStore.setCapacity(good::oil, 250);
    goodStore.setCapacity(good::wine, 250);
  }

  bool checkStorageInWorkRange( PlayerCityPtr city, const Locations& enter, object::Type objTypr )
  {
    auto route = PathwayHelper::shortWay( city, enter, objTypr, PathwayHelper::roadOnly );
    bool invalidRoute = !route.isValid();
    bool tooFarFromStorage = (route.length() >= MarketBuyer::maxBuyDistance() );

    return !(invalidRoute || tooFarFromStorage);
  }
};

Market::Market() : ServiceBuilding(Service::market, object::market, Size(2,2) ),
  _d( new Impl )
{
  _fgPictures().resize(1);  // animation
  _d->initStore();
}

void Market::deliverService()
{
  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    // the marketBuyer is ready to buy something!
    auto marketBuyer = Walker::create<MarketBuyer>( _city() );
    marketBuyer->send2City( this );

    if( !marketBuyer->isDeleted() )
    {
      addWalker( marketBuyer.object() );
    }
    else if( _d->isAnyGoodStored() )
    {
      ServiceBuilding::deliverService();
    }
  }
}

unsigned int Market::walkerDistance() const { return 26; }
good::Store& Market::goodStore(){ return _d->goodStore; }

good::Products Market::mostNeededGoods()
{
  good::Products res;

  std::multimap<float, good::Product> mapGoods;  // ordered by demand

  for( auto& goodType : good::all() )
  {
    // for all types of good
    good::Stock &stock = _d->goodStore.getStock(goodType);
    int demand = stock.capacity() - stock.qty();
    if (demand > 200)
    {
      mapGoods.insert( std::make_pair(float(stock.qty())/float(stock.capacity()), goodType));
    }
  }

  for( auto& it : mapGoods )
  {
    res.insert(it.second);
  }

  return res;
}


int Market::getGoodDemand(const good::Product &goodType)
{
  int res = 0;
  good::Stock& stock = _d->goodStore.getStock(goodType);
  res = stock.capacity() - stock.qty();
  res = (res/100)*100;  // round at the lowest century
  return res;
}

void Market::save( VariantMap& stream) const 
{
  ServiceBuilding::save( stream );
  VARIANT_SAVE_CLASS_D( stream, _d, goodStore )
}

void Market::load( const VariantMap& stream)
{
  ServiceBuilding::load( stream );

  VARIANT_LOAD_CLASS_D( _d, goodStore, stream )

  _d->initStore();
}

bool Market::build(const city::AreaInfo& info)
{
  bool isOk = ServiceBuilding::build( info );
  bool isLoadingMode = info.city->getOption( PlayerCity::forceBuild ) > 0;
  if( isOk && !isLoadingMode )
  {
    Locations locations = roadside().locations();
    bool accessGranary = _d->checkStorageInWorkRange( info.city, locations, object::granery );
    bool accessWarehouse = _d->checkStorageInWorkRange( info.city, locations, object::warehouse );

    if( !accessGranary )
        _setError( "##market_too_far_from_granary##" );
    else if( !accessWarehouse )
        _setError( "##market_too_far_from_warehouse##" );
  }

  return isOk;
}

void Market::timeStep(const unsigned long time)
{
  if( game::Date::isDayChanged() )
  {
    int servicemen_n = walkers().count<ServiceWalker>();
    if( servicemen_n > 0 && _d->goodStore.qty() == 0 )
    {
      walkers().firstOrEmpty<ServiceWalker>()->return2Base();
    }
  }

  ServiceBuilding::timeStep( time );
}
