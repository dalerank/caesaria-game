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

#include "computer_city.hpp"
#include "empire.hpp"
#include "trading.hpp"
#include "good/storage.hpp"
#include "good/helper.hpp"
#include "game/gamedate.hpp"
#include "core/foreach.hpp"
#include "merchant.hpp"
#include "game/funds.hpp"
#include "game/resourcegroup.hpp"
#include "empiremap.hpp"
#include "game/player.hpp"
#include "world/barbarian.hpp"
#include "core/metric.hpp"
#include "core/variant_map.hpp"
#include "city/states.hpp"
#include "config.hpp"

using namespace gfx;
using namespace metric;

class CcStorage : public good::Storage
{
public:
  VariantMap save() const
  {
    VariantMap ret;
    VariantMap capacityVm;
    VariantMap valuesVm;
    foreach( gtype, good::all() )
    {
      std::string tname = good::Helper::getTypeName( *gtype );
      int maxCap = capacity( *gtype );
      if( maxCap > 0 )
      {
        capacityVm[ tname ] = Unit::fromQty( maxCap ).ivalue();
      }

      int value = qty( *gtype );
      if( value > 0 )
      {
        valuesVm[ tname ] = Unit::fromQty( value ).ivalue();
      }
    }

    ret[ "capacities" ] = capacityVm;
    ret[ "values" ] = valuesVm;

    return ret;
  }

  void load( const VariantMap& stream )
  {
    VariantMap capacitiesVm = stream.get( "capacities" ).toMap();
    VariantMap valuesVm = stream.get( "values" ).toMap();

    foreach( it, capacitiesVm )
    {
      good::Product gtype = good::Helper::getType( it->first );
      setCapacity( gtype, Unit::fromValue( it->second ).toQty() );
    }

    foreach( it, valuesVm )
    {
      good::Product gtype = good::Helper::getType( it->first );
      setQty( gtype, Unit::fromValue( it->second ).toQty() );
    }
  }
};

namespace world
{

class ComputerCity::Impl
{
public:
  unsigned int tradeType;
  bool distantCity, romecity;
  bool available;
  int strength;
  city::States states;
  unsigned int tradeDelay;

  CcStorage sellStore;
  good::Storage realSells;
  CcStorage buyStore;

  DateTime lastTimeUpdate;
  DateTime lastTimeMerchantSend;
  DateTime lastAttack;

  unsigned int merchantsNumber;
  econ::Treasury funds;
};

ComputerCity::ComputerCity( EmpirePtr empire, const std::string& name )
  : City( empire ), _d( new Impl )
{
  setName( name );
  _d->tradeDelay = 0;
  _d->distantCity = false;
  _d->merchantsNumber = 0;
  _d->available = true;
  _d->states.population = 0;
  _d->states.nation = world::nation::unknown;
  _d->sellStore.setCapacity( 99999 );
  _d->buyStore.setCapacity( 99999 );
  _d->realSells.setCapacity( 99999 );
  _d->states.age = 0;
  _d->states.romeCity = false;

  _initTextures();
}

bool ComputerCity::_mayTrade() const { return _d->tradeDelay <= 0; }

econ::Treasury& ComputerCity::treasury() { return _d->funds; }
bool ComputerCity::isPaysTaxes() const { return true; }
bool ComputerCity::haveOverduePayment() const { return false; }
bool ComputerCity::isDistantCity() const{  return _d->distantCity;}
bool ComputerCity::isAvailable() const{  return _d->available;}
void ComputerCity::setAvailable(bool value){  _d->available = value;}

SmartPtr<Player> ComputerCity::mayor() const { return 0; }

void ComputerCity::save( VariantMap& options ) const
{
  City::save( options );

  options[ "out" ] = _d->sellStore.save();
  options[ "in" ] = _d->buyStore.save();
  options[ "realSells" ] = _d->realSells.save();

  options[ "sea" ] = (_d->tradeType & EmpireMap::sea ? true : false);
  options[ "land" ] = (_d->tradeType & EmpireMap::land ? true : false);

  VARIANT_SAVE_ANY_D( options, _d, lastTimeMerchantSend )
  VARIANT_SAVE_ANY_D( options, _d, lastTimeUpdate )
  VARIANT_SAVE_ANY_D( options, _d, states.age )
  VARIANT_SAVE_ANY_D( options, _d, available )
  VARIANT_SAVE_ANY_D( options, _d, merchantsNumber )
  VARIANT_SAVE_ANY_D( options, _d, distantCity )
  VARIANT_SAVE_ANY_D( options, _d, states.romeCity )
  VARIANT_SAVE_ANY_D( options, _d, tradeDelay )
  VARIANT_SAVE_ANY_D( options, _d, lastAttack )
  VARIANT_SAVE_ANY_D( options, _d, states.population )
  VARIANT_SAVE_ANY_D( options, _d, strength )
}

void ComputerCity::load( const VariantMap& options )
{
  City::load( options );

  VARIANT_LOAD_TIME_D  ( _d, lastTimeUpdate,        options )
  VARIANT_LOAD_TIME_D  ( _d, lastTimeMerchantSend,  options )
  VARIANT_LOAD_ANY_D   ( _d, available,             options )
  VARIANT_LOAD_ANY_D   ( _d, merchantsNumber,       options )
  VARIANT_LOAD_ANY_D   ( _d, distantCity,           options )
  VARIANT_LOAD_ANY_D   ( _d, available,             options )
  VARIANT_LOAD_ANY_D   ( _d, states.romeCity,       options )
  VARIANT_LOAD_ANY_D   ( _d, states.age,            options )
  VARIANT_LOAD_ANY_D   ( _d, tradeDelay,            options )
  VARIANT_LOAD_TIME_D  ( _d, lastAttack,            options )
  VARIANT_LOAD_ANY_D   ( _d, strength,              options )
  VARIANT_LOAD_ANYDEF_D( _d, states.population, _d->states.population, options )

  foreach( gtype, good::all() )
  {
    _d->sellStore.setCapacity( *gtype, 0 );
    _d->buyStore.setCapacity( *gtype, 0 );
    _d->realSells.setCapacity( *gtype, 0 );
  }

  changeTradeOptions( options );

  _d->sellStore.load( options.get( "out" ).toMap() );
  _d->buyStore.load( options.get( "in" ).toMap() );

  _d->tradeType = (options.get( "sea" ).toBool() ? EmpireMap::sea : EmpireMap::unknown)
                  + (options.get( "land" ).toBool() ? EmpireMap::land : EmpireMap::unknown);

  Variant vm_rsold = options.get( "realSells" );
  if( vm_rsold.isValid() )
  {
    _d->realSells.load( vm_rsold.toMap() );
  }

  _initTextures();
}

const good::Store& ComputerCity::importingGoods() const { return _d->buyStore; }
const good::Store& ComputerCity::exportingGoods() const{ return _d->realSells; }
const city::States& ComputerCity::states() const { return _d->states; }
void ComputerCity::delayTrade(unsigned int month){  _d->tradeDelay = month;}

void ComputerCity::empirePricesChanged(good::Product gtype, int bCost, int sCost)
{
}

CityPtr ComputerCity::create( EmpirePtr empire, const std::string& name )
{
  CityPtr ret( new ComputerCity( empire, name ) );
  ret->drop();

  return ret;
}

void ComputerCity::addObject(ObjectPtr object )
{
  if( is_kind_of<Merchant>( object ) )
  {
    MerchantPtr merchant = ptr_cast<Merchant>( object );
    good::Store& sellGoods = merchant->sellGoods();
    good::Store& buyGoods = merchant->buyGoods();

    _d->buyStore.storeAll( buyGoods );

    foreach( gtype, good::all() )
    {
      int qty = sellGoods.freeQty( *gtype );
      good::Stock stock( *gtype, qty, qty );
      _d->realSells.store( stock, qty );
    }

    _d->sellStore.storeAll( sellGoods );

    _d->merchantsNumber = std::max<int>( 0, _d->merchantsNumber-1);
  }
  else if( is_kind_of<Barbarian>( object ) )
  {
    BarbarianPtr brb = ptr_cast<Barbarian>( object );
    _d->lastAttack = game::Date::current();
    int attack = std::max<int>( brb->strength() - strength(), 0 );
    if( !attack ) attack = 10;
    _d->strength = math::clamp<int>( _d->strength - math::random( attack ), 0, 100 );

    if( _d->strength > 0 )
    {
      int resist = std::max<int>( strength() - brb->strength(), 0 );
      brb->updateStrength( math::random( resist ) );
    }
    else
    {
      delayTrade( brb->strength() );
    }
  }
}

void ComputerCity::changeTradeOptions(const VariantMap& stream)
{
  VariantMap sells_vm = stream.get( "sells" ).toMap();
  foreach( it, sells_vm )
  {
    good::Product gtype = good::Helper::getType( it->first );
    _d->sellStore.setCapacity( gtype, Unit::fromValue( it->second ).toQty() );
    _d->realSells.setCapacity( gtype, Unit::fromValue( it->second ).toQty() );
  }

  VariantMap buys_vm = stream.get( "buys" ).toMap();
  foreach( it, buys_vm )
  {
    good::Product gtype = good::Helper::getType( it->first );
    _d->buyStore.setCapacity( gtype, Unit::fromValue( it->second ).toQty() );
  }
}

ComputerCity::~ComputerCity() {}

void ComputerCity::timeStep( unsigned int time )
{
  if( game::Date::isMonthChanged() )
  {
    _d->tradeDelay = math::clamp<int>( _d->tradeDelay-1, 0, 99 );
  }

  if( game::Date::isWeekChanged() )
  {
    _d->strength = math::clamp<int>( _d->strength+1, 0, _d->states.population / 100 );
  }

  if( game::Date::isYearChanged() )
  {
    _d->states.age++;

    //debug muleta
    if( _d->funds.money() < 1000 )
    {
      _d->funds.resolveIssue( econ::Issue( econ::Issue::donation, 1000 ) );
    }
  }

  //one year before step need
  if( _d->lastTimeUpdate.monthsTo( game::Date::current() ) > DateTime::monthsInYear-1 )
  {
    _d->merchantsNumber = math::clamp<int>( _d->merchantsNumber-1, 0, config::trade::maxMerchantsInRoute );
    _d->lastTimeUpdate = game::Date::current();

    foreach( gtype, good::all() )
    {
      _d->sellStore.setQty( *gtype, _d->sellStore.capacity( *gtype ) );
      _d->buyStore.setQty( *gtype, 0  );
      _d->realSells.setQty( *gtype, 0 );
    }
  }

  if( _d->lastTimeMerchantSend.monthsTo( game::Date::current() ) > config::trade::minMonthsMerchantSend )
  {
    TraderouteList routes = empire()->tradeRoutes( name() );

    if( routes.empty() )
      return;

    if( !_mayTrade() )
      return;

    _d->lastTimeMerchantSend = game::Date::current();

    if( _d->merchantsNumber >= routes.size() )
    {
      return;
    }

    good::Storage sellGoods, buyGoods;
    sellGoods.setCapacity( Merchant::defaultCapacity );
    buyGoods.setCapacity( Merchant::defaultCapacity );

    foreach( gtype, good::all() )
    {
      buyGoods.setCapacity( *gtype, _d->buyStore.capacity( *gtype ) );

      //how much space left
      int maxQty = (std::min)( _d->sellStore.capacity( *gtype ) / 4, sellGoods.freeQty() );

      //we want send merchants to all routes
      maxQty /= routes.size();

      int qty = math::clamp( _d->sellStore.qty( *gtype ), 0, maxQty );

      //have no goods to sell
      if( qty == 0 )
        continue;

      good::Stock& stock = sellGoods.getStock( *gtype );
      stock.setCapacity( qty );

      //move goods to merchant's storage
      _d->sellStore.retrieve( stock, qty );
    }

    //send merchants to all routes
    foreach( route, routes )
    {
      _d->merchantsNumber++;
      (*route)->addMerchant( name(), sellGoods, buyGoods );
    }
  }
}

DateTime ComputerCity::lastAttack() const { return _d->lastAttack; }

std::string ComputerCity::about(Object::AboutType type)
{
  std::string ret;
  switch(type)
  {
  case empireMap:
    {
      if( isDistantCity() ) ret = "##empmap_distant_romecity_tip##";
      else ret = name();
    }
  break;

  default:
      ret = "##compcity_unknown_about##";
  }

  return ret;
}

unsigned int ComputerCity::tradeType() const { return _d->tradeType; }
int ComputerCity::strength() const { return _d->strength; }

void ComputerCity::_initTextures()
{
  int index = PicID::otherCity;

  if( _d->distantCity ) { index = PicID::distantCity; }
  else if( _d->romecity ) { index = PicID::romeCity; }

  Picture pic = Picture::load( ResourceGroup::empirebits, index );
  setPicture( pic );
  _animation().load( ResourceGroup::empirebits, index+1, 6 );
  _animation().setLoop( true );
  _animation().setDelay( 2 );
}

}//end namespace world
