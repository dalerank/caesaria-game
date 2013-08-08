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

#include "oc3_empire_trading.hpp"
#include "oc3_empire.hpp"
#include "oc3_empire_city.hpp"
#include "oc3_goodstore_simple.hpp"
#include "oc3_stringhelper.hpp"

class EmpireTradeRoute::Impl
{
oc3_signals public:
  Signal1<EmpireMerchantPtr> onMerchantArrivedSignal;
public:
  EmpireCityPtr begin;
  EmpireCityPtr end;
  typedef std::vector< EmpireMerchantPtr > MerchantList;
  MerchantList merchants;

  void resolveMerchantArrived( EmpireMerchantPtr merchant )
  {
    for( MerchantList::iterator it=merchants.begin(); it != merchants.end(); it++ )
    {
      if( *it == merchant )
      {
        (*it)->deleteLater();
        break;
      }
    }

    onMerchantArrivedSignal.emit( merchant );
  }
};

EmpireCityPtr EmpireTradeRoute::getBeginCity() const
{
  return _d->begin;  
}

EmpireCityPtr EmpireTradeRoute::getEndCity() const
{
  return _d->end;
}

void EmpireTradeRoute::update( unsigned int time )
{
  if( _d->merchants.size() == 0 )
  {
    SimpleGoodStore store;
    addMerchant( ((rand() % 2 == 1) ? _d->begin : _d->end )->getName(), store );
  }

  Impl::MerchantList::iterator it=_d->merchants.begin();
  while( it != _d->merchants.end() )
  {
    if( (*it)->isDeleted() )
    {
      it = _d->merchants.erase( it );
    }
    else
    {
      (*it)->update( time );
      it++;
    }
  }
}

void EmpireTradeRoute::addMerchant( const std::string& begin, GoodStore& store )
{
  EmpireMerchantPtr merchant = EmpireMerchant::create( *this, begin );
  _d->merchants.push_back( merchant );  

  CONNECT( merchant, onDestination(), _d.data(), Impl::resolveMerchantArrived );
}

EmpireTradeRoute::EmpireTradeRoute( EmpireCityPtr begin, EmpireCityPtr end )
: _d( new Impl )
{
  _d->begin = begin;
  _d->end = end;
}

EmpireTradeRoute::~EmpireTradeRoute()
{

}

EmpireMerchantPtr EmpireTradeRoute::getMerchant( unsigned int index )
{
  if( index >= _d->merchants.size() )
    return 0;

  Impl::MerchantList::iterator it = _d->merchants.begin();
  std::advance( it, index );
  return *it;
}

Signal1<EmpireMerchantPtr>& EmpireTradeRoute::onMerchantArrived()
{
  return _d->onMerchantArrivedSignal;
}

class EmpireTrading::Impl
{
public:
  EmpirePtr empire;
  typedef std::map< unsigned int, EmpireTradeRoutePtr > TradeRoutes;
  TradeRoutes routes;
};

EmpireTrading::EmpireTrading() : _d( new Impl )
{
  
}

void EmpireTrading::update( unsigned int time )
{
  if( time % 22 != 1 )
    return;

  for( Impl::TradeRoutes::iterator it=_d->routes.begin(); it != _d->routes.end(); it++ )
  {
    it->second->update( time );
  }
}

void EmpireTrading::init( EmpirePtr empire )
{
  _d->empire = empire;
}

EmpireTrading::~EmpireTrading()
{

}

void EmpireTrading::sendMerchant( const std::string& begin, const std::string& end, GoodStore& goods )
{
  EmpireTradeRoutePtr route = getRoute( begin, end );
  if( route != 0 )
  {
    StringHelper::debug( 0xff, "Trade route no exist [%s to %s]", begin.c_str(), end.c_str() );
    return;
  }

  route->addMerchant( begin, goods );
}

EmpireTradeRoutePtr EmpireTrading::getRoute( const std::string& begin, const std::string& end )
{
  unsigned int routeId = StringHelper::hash( begin ) + StringHelper::hash( end );
  Impl::TradeRoutes::iterator it = _d->routes.find( routeId );
  if( it == _d->routes.end() )
  {
    StringHelper::debug( 0xff, "Trade route no exist [%s to %s]", begin.c_str(), end.c_str() );
    return 0;
  }

  return it->second;
}

EmpireTradeRoutePtr EmpireTrading::getRoute( unsigned int index )
{
  if( index >= _d->routes.size() )
    return 0;

  Impl::TradeRoutes::iterator it = _d->routes.begin();
  std::advance( it, index );
  return it->second;
}

EmpireTradeRoutePtr EmpireTrading::createRoute( const std::string& begin, const std::string& end )
{
  EmpireTradeRoutePtr route = getRoute( begin, end );
  if( route != 0 )
  {
    StringHelper::debug( 0xff, "Trade route exist [%s to %s]", begin.c_str(), end.c_str() );
    return route;
  }

  EmpireCityPtr beginCity = _d->empire->getCity( begin );
  EmpireCityPtr endCity = _d->empire->getCity( end );
  unsigned int routeId = StringHelper::hash( begin ) + StringHelper::hash( end );

  route = EmpireTradeRoutePtr( new EmpireTradeRoute( beginCity, endCity ) );
  route->drop();
  _d->routes[ routeId ] = route;

  return route;
}

class EmpireMerchant::Impl
{
public:
  EmpireTradeRoute* route;
  SimpleGoodStore store;
  Point location;
  Point step;
  EmpireCityPtr destCity;
  bool isDeleted;

oc3_signals public:
  Signal1<EmpireMerchantPtr> onDestinationSignal;
};

EmpireMerchant::~EmpireMerchant()
{

}

EmpireMerchant::EmpireMerchant() : _d( new Impl )
{
  _d->isDeleted = false;
}

EmpireMerchantPtr EmpireMerchant::create( EmpireTradeRoute& route, const std::string& start )
{
  EmpireMerchantPtr ret( new EmpireMerchant() );
  ret->_d->route = &route;
  bool startCity = route.getBeginCity()->getName() == start;
  ret->_d->destCity = startCity ? route.getBeginCity() : route.getEndCity();
  ret->_d->location = ( startCity ? route.getEndCity() : route.getBeginCity() )->getLocation();
  ret->_d->step = ( ret->_d->destCity->getLocation() - ret->_d->location ) / 30;
  ret->drop();

  return ret;
}

Signal1<EmpireMerchantPtr>& EmpireMerchant::onDestination()
{
  return _d->onDestinationSignal;
}

void EmpireMerchant::update( unsigned int time )
{
  _d->location += _d->step;

  if( _d->destCity->getLocation().distanceTo( _d->location ) < 5 )
  {
    _d->onDestinationSignal.emit( this );

    _d->destCity->resolveMerchantArrived( this );
  }
}

Point EmpireMerchant::getLocation() const
{
  return _d->location;
}

bool EmpireMerchant::isDeleted() const
{
  return _d->isDeleted;
}

void EmpireMerchant::deleteLater()
{
  _d->isDeleted = true;
}