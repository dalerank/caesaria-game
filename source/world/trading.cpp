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

#include "trading.hpp"
#include "empire.hpp"
#include "city.hpp"
#include "good/storage.hpp"
#include "core/utils.hpp"
#include "core/foreach.hpp"
#include "merchant.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "good/helper.hpp"
#include "game/gamedate.hpp"

namespace world
{

enum { idxBuyPrice=0, idxSellPrice=1 };

class TradeRoutes::Impl
{
public:
  std::map<unsigned int, TraderoutePtr> routes;
  EmpirePtr empire;
};

class Prices : public std::map< good::Product, PriceInfo >
{
public:
  VariantMap save() const
  {
    VariantMap ret;
    for( auto& it : *this )
    {
      VariantList tmp( it.second.buy, it.second.sell );
      ret[ good::Helper::getTypeName( it.first ) ] = tmp;
    }

    return ret;
  }

  void load( const VariantMap& stream )
  {
    for( auto& it : stream )
    {
      good::Product gtype = good::Helper::getType( it.first );
      if( gtype != good::none )
      {
        VariantList vl = it.second.toList();
        setPrice( gtype, vl.get( idxBuyPrice, 0 ).toInt(), vl.get( idxSellPrice, 0 ).toInt() );
      }
    }
  }

  void setPrice( good::Product type, int buy, int sell )
  {
    (*this)[ type ].buy = buy;
    (*this)[ type ].sell = sell;
  }
};

class Trading::Impl
{
public:
  EmpirePtr empire;
  Prices empirePrices;

  void initStandartPrices();
};

Trading::Trading()
  : _d( new Impl )
{
  _d->initStandartPrices();
}

void Trading::timeStep( unsigned int time )
{
}

void Trading::init( EmpirePtr empire )
{
  _d->empire = empire;
}

VariantMap Trading::save() const
{
  VariantMap ret;

  VARIANT_SAVE_CLASS_D( ret, _d, empirePrices )

  return ret;
}

void Trading::load(const VariantMap& stream)
{
  VARIANT_LOAD_CLASS_D( _d, empirePrices, stream )
}

Trading::~Trading() {}

void Trading::sendMerchant( const std::string& begin, const std::string& end,
                            good::Store &sell, good::Store &buy )
{
  TraderoutePtr route = _d->empire->troutes().find( begin, end );
  if( !route.isValid() )
  {
    Logger::warning( "Trade route no exist [{0} to {1}]", begin, end );
    return;
  }

  route->addMerchant( begin, sell, buy );
}

TraderoutePtr TradeRoutes::find( const std::string& begin, const std::string& end )
{
  unsigned int routeId = Traderoute::getId( begin, end );

  auto it = _d->routes.find( routeId );
  if( it == _d->routes.end() )
  {
    Logger::warning( "!!! WARNING: Trade route no exist [{0} to {1}]", begin, end );
    return TraderoutePtr();
  }

  return it->second;
}

TraderoutePtr TradeRoutes::find( unsigned int index )
{
  bool invalidIndex = index >= _d->routes.size();
  if( invalidIndex )
    return TraderoutePtr();

  auto it = _d->routes.begin();
  std::advance( it, index );
  return it->second;
}

TraderoutePtr TradeRoutes::create( const std::string& begin, const std::string& end )
{
  TraderoutePtr route = find( begin, end );
  if( route.isValid() )
  {
    Logger::warning( "!!!WARNING: Want create route, but it exist [{0} to {1}]", begin, end );
    return route;
  }

  route = TraderoutePtr( new Traderoute( _d->empire, begin, end ) );
  unsigned int routeId = Traderoute::getId( begin, end );
  _d->routes[ routeId ] = route;
  route->drop();

  return route;
}

void Trading::setPrice(good::Product gtype, int bCost, int sCost)
{
  _d->empirePrices.setPrice( gtype, bCost, sCost );
}

PriceInfo Trading::getPrice(good::Product gtype)
{
  PriceInfo ret;
  Prices::const_iterator it = _d->empirePrices.find( gtype );

  if( it != _d->empirePrices.end() )
  {
    ret = it->second;
  }

  return ret;
}

TraderouteList TradeRoutes::from( const std::string& begin )
{
  TraderouteList ret;

  CityPtr city = _d->empire->findCity( begin );

  for( auto& route : _d->routes )
  {
    if( route.second->beginCity() == city || route.second->endCity() == city )
    {
      ret.push_back( route.second );
    }
  }

  return ret;
}

TraderouteList TradeRoutes::all()
{
  TraderouteList ret;
  for( auto& item : _d->routes )
    ret.push_back( item.second );

  return ret;
}

TradeRoutes::TradeRoutes()
  : _d( new Impl )
{
}

TradeRoutes::~TradeRoutes() {}

void TradeRoutes::init(EmpirePtr empire) { _d->empire = empire; }

void Trading::Impl::initStandartPrices()
{
  Prices& b = empirePrices;
  b.setPrice( good::wheat,      28,  22 );
  b.setPrice( good::vegetable,  38,  30 );
  b.setPrice( good::fruit,      38,  30 );
  b.setPrice( good::olive,      42,  34 );
  b.setPrice( good::grape,      44,  36 );
  b.setPrice( good::meat,       44,  36 );
  b.setPrice( good::wine,       215, 160);
  b.setPrice( good::oil,        180, 140);
  b.setPrice( good::iron,       60,  40 );
  b.setPrice( good::timber,     50,  35 );
  b.setPrice( good::clay,       40,  30 );
  b.setPrice( good::marble,     200, 140);
  b.setPrice( good::weapon,     250, 180);
  b.setPrice( good::furniture,  200, 150);
  b.setPrice( good::pottery,    180, 140);
}

VariantMap TradeRoutes::save() const
{
  VariantMap ret;
  for( auto& it : _d->routes )
    ret[ it.second->name() ] = it.second->save();

  return ret;
}

void TradeRoutes::load( const VariantMap& stream)
{
  for( auto& it : stream )
  {
    StringArray cityNames = utils::split( it.first,"<->" );
    if( !cityNames.empty() )
    {
      std::string beginCity = cityNames.valueOrEmpty( 0 );
      std::string endCity = cityNames.valueOrEmpty( 1 );
      TraderoutePtr route = create( beginCity, endCity );
      if( route.isValid() )
      {
        route->load( it.second.toMap() );
      }
      else
      {
        Logger::warning( "WARNING!!! Trading::load cant create route from {0} to {1}",
                         beginCity, endCity );
      }
    }
    else
    {
      Logger::warning( "WARNING!!! Trading::load cant create route from " + it.first );
    }
  }
}

}//end namespace world
