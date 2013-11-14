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
  Point location, destLocation;
  Point step;
  std::string destCity, baseCity;
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

  CityPtr baseCity = startCity ? route.getBeginCity() : route.getEndCity();
  CityPtr destCity = startCity ? route.getEndCity() : route.getBeginCity();

  ret->_d->baseCity = baseCity->getName();
  ret->_d->destCity = destCity->getName();
  ret->_d->location = baseCity->getLocation();
  ret->_d->destLocation = destCity->getLocation();
  ret->_d->step = ( ret->_d->destLocation - ret->_d->location ) / 8;
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

  if( _d->destLocation.distanceTo( _d->location ) < 20 )
  {
    _d->onDestinationSignal.emit( this );
  }
}

std::string Merchant::getDestCityName() const
{
  return _d->destCity;
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

VariantMap Merchant::save() const
{
  VariantMap ret;
  ret[ "sells"    ]= _d->sells.save();
  ret[ "buys"     ]= _d->buys.save();
  ret[ "location" ]= _d->location;
  ret[ "step"     ]= _d->step;
  ret[ "deleted"  ]= _d->isDeleted;
  ret[ "destLocation" ] = _d->destLocation;
  ret[ "begin"    ]= Variant( _d->baseCity );
  ret[ "end"      ]= Variant( _d->destCity );

  return ret;
}

void Merchant::load(const VariantMap& stream)
{
  _d->sells.load( stream.get( "sells" ).toMap() );
  _d->buys.load( stream.get( "buys" ).toMap() );
  _d->step = stream.get( "step" );
  _d->isDeleted = stream.get( "deleted" );
  _d->baseCity = stream.get( "begin" ).toString();
  _d->destCity = stream.get( "end" ).toString();

  bool startCity = ( _d->route->getBeginCity()->getName() == _d->baseCity);
  CityPtr baseCity = startCity ? _d->route->getBeginCity() : _d->route->getEndCity();
  CityPtr destCity = startCity ? _d->route->getEndCity() : _d->route->getBeginCity();

  _d->location = baseCity->getLocation();
  _d->destLocation = destCity->getLocation();

  _d->step = ( _d->destLocation - _d->location ) / 8;
  _d->location = stream.get( "location" );
}

std::string Merchant::getBaseCityName() const
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
