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
#include "good/goodstore_simple.hpp"
#include "core/utils.hpp"
#include "core/foreach.hpp"
#include "merchant.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "good/goodhelper.hpp"
#include "game/gamedate.hpp"

namespace world
{

class Trading::Impl
{
public:
  struct PriceInfo
  {
    unsigned int sellPrice;
    unsigned int buyPrice;
  };

  typedef std::map< good::Product, PriceInfo > Prices;
  typedef std::map< unsigned int, TraderoutePtr > TradeRoutes;

  EmpirePtr empire;
  TradeRoutes routes;
  Prices empirePrices;

  void setPrice( good::Product type, int buy, int sell );
  void initStandartPrices();
};

Trading::Trading() : _d( new Impl )
{
  _d->initStandartPrices();
}

void Trading::timeStep( unsigned int time )
{
  if( game::Date::isDayChanged() )
  {
    foreach( it,_d->routes )
    {
      it->second->update( time );
    }
  }
}

void Trading::init( EmpirePtr empire )
{
  _d->empire = empire;
}

VariantMap Trading::save() const
{
  VariantMap ret;
  VariantMap routesVm;
  foreach( it, _d->routes )
  {
    routesVm[ it->second->getName() ] = it->second->save();
  }

  ret[ "routes" ] = routesVm;

  VariantMap pricesVm;
  foreach( it, _d->empirePrices )
  {
    VariantList tmp;
    tmp << it->second.buyPrice << it->second.sellPrice;
    pricesVm[ good::Helper::getTypeName( it->first ) ] = tmp;
  }

  ret[ "prices" ] = pricesVm;

  return ret;
}

void Trading::load(const VariantMap& stream)
{
  VariantMap routes = stream.get( "routes" ).toMap();
  foreach( it, routes )
  {
    std::string routeName = it->first;
    std::string::size_type delimPos = routeName.find( "<->" );
    if( delimPos != std::string::npos )
    {
      std::string beginCity = routeName.substr( 0, delimPos );
      std::string endCity = routeName.substr( delimPos+3 );
      TraderoutePtr route = createRoute( beginCity, endCity );
      if( route.isValid() )
      {
        route->load( it->second.toMap() );
      }
      else
      {
        Logger::warning( "WARNING!!! Trading::load cant create route from %s to %s",
                         beginCity.c_str(), endCity.c_str() );
      }
    }
  }

  VariantMap prices = stream.get( "prices" ).toMap();
  foreach( it, prices )
  {
    good::Product gtype = good::Helper::getType( it->first );
    if( gtype != good::none )
    {
      VariantList vl = it->second.toList();
      _d->setPrice( gtype, vl.get( 0, 0 ).toInt(), vl.get( 1, 0 ).toInt() );
    }
  }
}

Trading::~Trading()
{

}

void Trading::sendMerchant(const std::string& begin, const std::string& end,
                            good::Store &sell, good::Store &buy )
{
  TraderoutePtr route = findRoute( begin, end );
  if( route != 0 )
  {
    Logger::warning( "Trade route no exist [%s to %s]", begin.c_str(), end.c_str() );
    return;
  }

  route->addMerchant( begin, sell, buy );
}

TraderoutePtr Trading::findRoute( const std::string& begin, const std::string& end )
{
  unsigned int routeId = utils::hash( begin ) + utils::hash( end );
  Impl::TradeRoutes::iterator it = _d->routes.find( routeId );
  if( it == _d->routes.end() )
  {
    Logger::warning( "Trade route no exist [%s to %s]", begin.c_str(), end.c_str() );
    return 0;
  }

  return it->second;
}

TraderoutePtr Trading::findRoute( unsigned int index )
{
  if( index >= _d->routes.size() )
    return 0;

  Impl::TradeRoutes::iterator it = _d->routes.begin();
  std::advance( it, index );
  return it->second;
}

TraderoutePtr Trading::createRoute( const std::string& begin, const std::string& end )
{
  TraderoutePtr route = findRoute( begin, end );
  if( route != 0 )
  {
    Logger::warning( "Trade route exist [%s to %s]", begin.c_str(), end.c_str() );
    return route;
  }

  unsigned int routeId = utils::hash( begin ) + utils::hash( end );

  route = TraderoutePtr( new Traderoute( _d->empire, begin, end ) );
  _d->routes[ routeId ] = route;
  route->drop();

  return route;
}

void Trading::setPrice(good::Product gtype, int bCost, int sCost)
{
  _d->setPrice( gtype, bCost, sCost );
}

void Trading::getPrice(good::Product gtype, int& bCost, int& sCost)
{
  Impl::Prices::const_iterator it = _d->empirePrices.find( gtype );

  if( it != _d->empirePrices.end() )
  {
    bCost = it->second.buyPrice;
    sCost = it->second.sellPrice;
  }
  else
  {
    bCost = sCost = 0;
  }
}

TraderouteList Trading::routes( const std::string& begin )
{
  TraderouteList ret;

  CityPtr city = _d->empire->findCity( begin );

  foreach( it, _d->routes )
  {
    if( it->second->beginCity() == city || it->second->endCity() == city )
    {
      ret.push_back( it->second );
    }
  }

  return ret;
}

TraderouteList Trading::routes()
{
  TraderouteList ret;
  foreach( item, _d->routes ) { ret.push_back( item->second ); }

  return ret;
}

void Trading::Impl::setPrice(good::Product type, int bCost, int sCost)
{
  empirePrices[ type ].buyPrice = bCost;
  empirePrices[ type ].sellPrice = sCost;
}

void Trading::Impl::initStandartPrices()
{
  setPrice( good::wheat, 28, 22 );
  setPrice( good::vegetable, 38, 30 );
  setPrice( good::fruit, 38, 30 );
  setPrice( good::olive, 42, 34 );
  setPrice( good::grape, 44, 36 );
  setPrice( good::meat, 44, 36 );
  setPrice( good::wine, 215, 160 );
  setPrice( good::oil, 180, 140 );
  setPrice( good::iron, 60, 40 );
  setPrice( good::timber, 50, 35 );
  setPrice( good::clay, 40, 30 );
  setPrice( good::marble, 200, 140 );
  setPrice( good::weapon, 250, 180 );
  setPrice( good::furniture, 200, 150 );
  setPrice( good::pottery, 180, 140 );
}

}//end namespace world
