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
#include "core/utils.hpp"
#include "core/variant_map.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "traderoute.hpp"
#include "game/resourcegroup.hpp"

namespace world
{

class Merchant::Impl
{
public:
  TraderoutePtr route;
  good::SimpleStore sells;
  good::SimpleStore buys;
  PointsArray steps;
  unsigned int step;
  std::string destCity, baseCity;

signals public:
  Signal1<MerchantPtr> onDestinationSignal;
};

Merchant::~Merchant(){}

Merchant::Merchant( EmpirePtr empire )
  : Object( empire ), _d( new Impl )
{
  //default picture
  setPicture( gfx::Picture::load( ResourceGroup::empirebits, PicID::landTradeRoute ) );
}

MerchantPtr Merchant::create( EmpirePtr empire, TraderoutePtr route, const std::string& start,
                              good::Store &sell, good::Store &buy )
{
  MerchantPtr ret( new Merchant( empire ) );
  ret->drop();

  ret->_d->route = route;
  bool startCity = (route->beginCity()->name() == start);
  
  ret->_d->sells.resize( sell );
  ret->_d->sells.storeAll( sell );

  ret->_d->buys.resize( buy );
  ret->_d->buys.storeAll( buy );

  CityPtr baseCity = startCity ? route->beginCity() : route->endCity();
  CityPtr destCity = startCity ? route->endCity() : route->beginCity();

  ret->_d->baseCity = baseCity->name();
  ret->_d->destCity = destCity->name();

  ret->_d->steps = route->points( !startCity );
  ret->_d->step = 0;  

  if( ret->_d->steps.empty() )
  {
    return MerchantPtr();
  }

  ret->setPicture( gfx::Picture::load( ResourceGroup::empirebits,
                                       route->isSeaRoute() ? PicID::seaTradeRoute : PicID::landTradeRoute ));
  ret->setLocation( ret->_d->steps.front() );
  return ret;
}

Signal1<MerchantPtr>& Merchant::onDestination(){  return _d->onDestinationSignal;}

void Merchant::timeStep( unsigned int time )
{
  _d->step++;

  if( _d->step >= _d->steps.size() )
  {
    emit _d->onDestinationSignal( this );
  }
  else
  {
    setLocation( _d->steps[ _d->step ] );
  }
}

std::string Merchant::destinationCity() const {  return _d->destCity; }
bool Merchant::isSeaRoute() const{  return _d->route->isSeaRoute();}

void Merchant::save(VariantMap& stream) const
{
  Object::save( stream );

  stream[ "sells"    ]= _d->sells.save();
  stream[ "buys"     ]= _d->buys.save();
  stream[ "step"     ]= _d->step;
  stream[ "begin"    ]= Variant( _d->baseCity );
  stream[ "end"      ]= Variant( _d->destCity );

  VariantList vl_steps;

  foreach( p, _d->steps ) { vl_steps.push_back( *p ); }

  stream[ "steps"    ]= vl_steps;
}

void Merchant::load(const VariantMap& stream)
{
  Object::load( stream );
  _d->sells.load( stream.get( "sells" ).toMap() );
  _d->buys.load( stream.get( "buys" ).toMap() );
  _d->step = stream.get( "step" ).toInt();
  _d->baseCity = stream.get( "begin" ).toString();
  _d->destCity = stream.get( "end" ).toString();

  VariantList steps = stream.get( "steps" ).toList();
  foreach( v, steps ) { _d->steps.push_back( v->toPoint() ); }
}

std::string Merchant::baseCity() const{  return _d->baseCity;}
good::Store& Merchant::sellGoods(){  return _d->sells;}
good::Store &Merchant::buyGoods(){  return _d->buys;}

}//end namespace world
