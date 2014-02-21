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

#include "merchant.hpp"
#include "empire.hpp"
#include "city.hpp"
#include "good/goodstore_simple.hpp"
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "traderoute.hpp"

namespace world
{

class Merchant::Impl
{
public:
  TraderoutePtr route;
  SimpleGoodStore sells;
  SimpleGoodStore buys;
  PointsArray steps;
  unsigned int step;
  Point location;
  std::string destCity, baseCity;
  bool isDeleted;

oc3_signals public:
  Signal1<MerchantPtr> onDestinationSignal;
};

Merchant::~Merchant(){}

Merchant::Merchant() : _d( new Impl )
{
  _d->isDeleted = false;
}

MerchantPtr Merchant::create( TraderoutePtr route, const std::string& start,
                              GoodStore& sell, GoodStore& buy )
{
  MerchantPtr ret( new Merchant() );
  ret->drop();

  ret->_d->route = route;
  bool startCity = (route->getBeginCity()->getName() == start);
  
  ret->_d->sells.resize( sell );
  ret->_d->sells.storeAll( sell );

  ret->_d->buys.resize( buy );
  ret->_d->buys.storeAll( buy );

  CityPtr baseCity = startCity ? route->getBeginCity() : route->getEndCity();
  CityPtr destCity = startCity ? route->getEndCity() : route->getBeginCity();

  ret->_d->baseCity = baseCity->getName();
  ret->_d->destCity = destCity->getName();

  ret->_d->steps = route->getPoints( !startCity );
  ret->_d->step = 0;  

  if( ret->_d->steps.empty() )
  {
    return MerchantPtr();
  }

  ret->_d->location = ret->_d->steps.front();
  return ret;
}

Signal1<MerchantPtr>& Merchant::onDestination(){  return _d->onDestinationSignal;}

void Merchant::update( unsigned int time )
{
  _d->step++;

  if( _d->step >= _d->steps.size() )
  {
    _d->onDestinationSignal.emit( this );
  }
  else
  {
    PointsArray::iterator it = _d->steps.begin();
    std::advance( it, _d->step );
    _d->location = *it;
  }
}

std::string Merchant::getDestCityName() const {  return _d->destCity; }
Point Merchant::getLocation() const{  return _d->location;}
bool Merchant::isDeleted() const{  return _d->isDeleted;}
void Merchant::deleteLater(){  _d->isDeleted = true;}
bool Merchant::isSeaRoute() const{  return _d->route->isSeaRoute();}

VariantMap Merchant::save() const
{
  VariantMap ret;
  ret[ "sells"    ]= _d->sells.save();
  ret[ "buys"     ]= _d->buys.save();
  ret[ "step"     ]= _d->step;
  ret[ "deleted"  ]= _d->isDeleted;
  ret[ "begin"    ]= Variant( _d->baseCity );
  ret[ "end"      ]= Variant( _d->destCity );
  ret[ "location" ]= _d->location;
  VariantList vl_steps;

  foreach( p, _d->steps ) { vl_steps.push_back( *p ); }

  ret[ "steps"    ]= vl_steps;

  return ret;
}

void Merchant::load(const VariantMap& stream)
{
  _d->sells.load( stream.get( "sells" ).toMap() );
  _d->buys.load( stream.get( "buys" ).toMap() );
  _d->step = stream.get( "step" ).toInt();
  _d->isDeleted = stream.get( "deleted" );
  _d->baseCity = stream.get( "begin" ).toString();
  _d->destCity = stream.get( "end" ).toString();
  _d->location = stream.get( "location" ).toPoint();

  VariantList steps = stream.get( "steps" ).toList();
  foreach( v, steps ) { _d->steps.push_back( v->toPoint() ); }
}

std::string Merchant::getBaseCityName() const{  return _d->baseCity;}
GoodStore& Merchant::getSellGoods(){  return _d->sells;}
GoodStore& Merchant::getBuyGoods(){  return _d->buys;}

}//end namespace world
