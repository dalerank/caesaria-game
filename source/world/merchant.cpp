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

#include "merchant.hpp"
#include "empire.hpp"
#include "city.hpp"
#include "game/goodstore_simple.hpp"
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"

namespace world
{

class Merchant::Impl
{
public:
  TradeRoute* route;
  SimpleGoodStore sells;
  SimpleGoodStore buys;
  Point location;
  Point step;
  CityPtr destCity, baseCity;
  bool isDeleted;

oc3_signals public:
  Signal1<MerchantPtr> onDestinationSignal;
};

Merchant::~Merchant()
{

}

Merchant::Merchant() : _d( new Impl )
{
  _d->isDeleted = false;
}

MerchantPtr Merchant::create( TradeRoute& route, const std::string& start,
                                          GoodStore& sell, GoodStore& buy )
{
  MerchantPtr ret( new Merchant() );
  ret->_d->route = &route;
  bool startCity = (route.getBeginCity()->getName() == start);
  
  ret->_d->sells.resize( sell );
  ret->_d->sells.storeAll( sell );

  ret->_d->buys.resize( buy );
  ret->_d->buys.storeAll( buy );

  ret->_d->baseCity = startCity ? route.getBeginCity() : route.getEndCity();
  ret->_d->destCity = startCity ? route.getEndCity() : route.getBeginCity();
  ret->_d->location = ret->_d->baseCity->getLocation();
  ret->_d->step = ( ret->_d->destCity->getLocation() - ret->_d->location ) / 3;
  ret->drop();

  return ret;
}

Signal1<MerchantPtr>& Merchant::onDestination()
{
  return _d->onDestinationSignal;
}

void Merchant::update( unsigned int time )
{
  _d->location += _d->step;

  if( _d->destCity->getLocation().distanceTo( _d->location ) < 5 )
  {
    _d->onDestinationSignal.emit( this );

    _d->destCity->resolveMerchantArrived( this );
  }
}

Point Merchant::getLocation() const
{
  return _d->location;
}

bool Merchant::isDeleted() const
{
  return _d->isDeleted;
}

void Merchant::deleteLater()
{
  _d->isDeleted = true;
}

CityPtr Merchant::getBaseCity() const
{
  return _d->baseCity;
}

GoodStore& Merchant::getSellGoods()
{
  return _d->sells;
}

GoodStore& Merchant::getBuyGoods()
{
  return _d->buys;
}

}//end namespace world
