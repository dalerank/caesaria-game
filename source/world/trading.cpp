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
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"

namespace world
{

class Trading::Impl
{
public:
  EmpirePtr empire;
  typedef std::map< unsigned int, TraderoutePtr > TradeRoutes;
  TradeRoutes routes;
};

Trading::Trading() : _d( new Impl )
{
  
}

void Trading::update( unsigned int time )
{
  if( time % 22 != 1 )
    return;

  for( Impl::TradeRoutes::iterator it=_d->routes.begin(); it != _d->routes.end(); it++ )
  {
    it->second->update( time );
  }
}

void Trading::init( EmpirePtr empire )
{
  _d->empire = empire;
}

VariantMap Trading::save() const
{
  VariantMap ret;
  VariantMap routes;
  for( Impl::TradeRoutes::iterator it=_d->routes.begin(); it != _d->routes.end(); it++ )
  {
    routes[ it->second->getName() ] = it->second->save();
  }

  ret[ "routes" ] = routes;
  return ret;
}

void Trading::load(const VariantMap& stream)
{
  VariantMap routes = stream.get( "routes" ).toMap();
  for( VariantMap::iterator it=routes.begin(); it != routes.end(); it++ )
  {
    std::string routeName = it->first;
    unsigned int delimPos = routeName.find( "<->" );
    if( delimPos != std::string::npos )
    {
      std::string beginCity = routeName.substr( 0, delimPos );
      std::string endCity = routeName.substr( delimPos+3 );
      TraderoutePtr route = createRoute( beginCity, endCity );
      route->load( it->second.toMap() );
    }
  }
}

Trading::~Trading()
{

}

void Trading::sendMerchant( const std::string& begin, const std::string& end,
                                  GoodStore& sell, GoodStore& buy )
{
  TraderoutePtr route = getRoute( begin, end );
  if( route != 0 )
  {
    Logger::warning( "Trade route no exist [%s to %s]", begin.c_str(), end.c_str() );
    return;
  }

  route->addMerchant( begin, sell, buy );
}

TraderoutePtr Trading::getRoute( const std::string& begin, const std::string& end )
{
  unsigned int routeId = StringHelper::hash( begin ) + StringHelper::hash( end );
  Impl::TradeRoutes::iterator it = _d->routes.find( routeId );
  if( it == _d->routes.end() )
  {
    Logger::warning( "Trade route no exist [%s to %s]", begin.c_str(), end.c_str() );
    return 0;
  }

  return it->second;
}

TraderoutePtr Trading::getRoute( unsigned int index )
{
  if( index >= _d->routes.size() )
    return 0;

  Impl::TradeRoutes::iterator it = _d->routes.begin();
  std::advance( it, index );
  return it->second;
}

TraderoutePtr Trading::createRoute( const std::string& begin, const std::string& end )
{
  TraderoutePtr route = getRoute( begin, end );
  if( route != 0 )
  {
    Logger::warning( "Trade route exist [%s to %s]", begin.c_str(), end.c_str() );
    return route;
  }

  unsigned int routeId = StringHelper::hash( begin ) + StringHelper::hash( end );

  route = TraderoutePtr( new Traderoute( _d->empire, begin, end ) );
  _d->routes[ routeId ] = route;
  route->drop();

  return route;
}

TraderouteList Trading::getRoutes( const std::string& begin )
{
  TraderouteList ret;

  CityPtr city = _d->empire->getCity( begin );

  for( Impl::TradeRoutes::iterator it = _d->routes.begin(); it != _d->routes.end(); it++ )
  {
    if( it->second->getBeginCity() == city || it->second->getEndCity() == city )
    {
      ret.push_back( it->second );
    }
  }

  return ret;
}

TraderouteList Trading::getRoutes()
{
  TraderouteList ret;
  foreach( item, _d->routes ) { ret.push_back( item->second ); }

  return ret;
}
}//end namespace world
