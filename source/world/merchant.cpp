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
#include "good/storage.hpp"
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
  good::Storage sells;
  good::Storage buys;
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

  VARIANT_SAVE_CLASS_D( stream, _d, sells )
  VARIANT_SAVE_CLASS_D( stream, _d, buys )
  VARIANT_SAVE_ANY_D( stream, _d, step )
  VARIANT_SAVE_STR_D( stream, _d, baseCity )
  VARIANT_SAVE_STR_D( stream, _d, destCity )

  stream[ "steps" ] = _d->steps.toVList();
}

void Merchant::load(const VariantMap& stream)
{
  Object::load( stream );
  VARIANT_LOAD_CLASS_D( _d, sells, stream )
  VARIANT_LOAD_CLASS_D( _d, buys, stream )
  VARIANT_LOAD_ANY_D( _d, step, stream )
  VARIANT_LOAD_STR_D( _d, baseCity, stream )
  VARIANT_LOAD_STR_D( _d, destCity, stream )

  _d->steps.fromVList( stream.get( "steps" ).toList() );
}

std::string Merchant::baseCity() const{  return _d->baseCity;}
good::Store& Merchant::sellGoods(){  return _d->sells;}
good::Store &Merchant::buyGoods(){  return _d->buys;}

}//end namespace world
