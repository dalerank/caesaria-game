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

#include "oc3_empire_city_computer.hpp"
#include "oc3_empire.hpp"
#include "oc3_empire_trading.hpp"
#include "oc3_goodstore_simple.hpp"
#include "oc3_goodhelper.hpp"

class ComputerCity::Impl
{
public:
  Point location;
  std::string name;
  EmpirePtr empire;
  bool distantCity;
  SimpleGoodStore sellStore;
  SimpleGoodStore buyStore;
};

ComputerCity::ComputerCity( EmpirePtr empire, const std::string& name ) : _d( new Impl )
{
  _d->name = name;
  _d->distantCity = false;
  _d->empire = empire;
}

std::string ComputerCity::getName() const
{
  return _d->name;
}

Point ComputerCity::getLocation() const
{
  return _d->location;
}

void ComputerCity::setLocation( const Point& location )
{
  _d->location = location;
}

bool ComputerCity::isDistantCity() const
{
  return _d->distantCity;
}

void ComputerCity::save( VariantMap& options ) const
{
  options[ "location" ] = _d->location;

  VariantMap vm_sells;
  VariantMap vm_sold;
  VariantMap vm_buys;
  VariantMap vm_bought;
  for( int i=G_NONE; i < G_MAX; i ++ )
  {
    GoodType gtype = GoodType ( i );
    std::string tname = GoodHelper::getTypeName( gtype );
    int maxSellStock = _d->sellStore.getMaxQty( gtype );
    if( maxSellStock > 0 )
    {
      vm_sells[ tname ] = maxSellStock;
    }

    int sold = _d->sellStore.getCurrentQty( gtype );
    if( sold > 0 )
    {
      vm_sold[ tname ] = sold;
    }

    int maxBuyStock = _d->buyStore.getMaxQty( gtype );
    if( maxBuyStock > 0 )
    {
      vm_buys[ tname ] = maxBuyStock;
    }

    int bought = _d->buyStore.getCurrentQty( gtype );
    if( bought > 0 )
    {
      vm_bought[ tname ] = bought;
    }
  }

  options[ "sells" ] = vm_sells;
  options[ "buys" ] = vm_buys;
  options[ "sold" ] = vm_sold;
  options[ "bought" ] = vm_bought;
}

void ComputerCity::load( const VariantMap& options )
{
  setLocation( options.get( "location" ).toPoint() );

  const VariantMap& sells_vm = options.get( "sells" ).toMap();
  for( VariantMap::const_iterator it=sells_vm.begin(); it != sells_vm.end(); it++ )
  {
    GoodType gtype = GoodHelper::getType( it->first );
    _d->sellStore.setMaxQty( gtype, it->second.toInt() );
  }

  const VariantMap& sold_vm = options.get( "sold" ).toMap();
  for( VariantMap::const_iterator it=sold_vm.begin(); it != sold_vm.end(); it++ )
  {
    GoodType gtype = GoodHelper::getType( it->first );
    _d->sellStore.setCurrentQty( gtype, it->second.toInt() );
  }

  const VariantMap& buys_vm = options.get( "buys" ).toMap();
  for( VariantMap::const_iterator it=buys_vm.begin(); it != buys_vm.end(); it++ )
  {
    GoodType gtype = GoodHelper::getType( it->first );
    _d->buyStore.setMaxQty( gtype, it->second.toInt() );
  }

  const VariantMap& bought_vm = options.get( "bought" ).toMap();
  for( VariantMap::const_iterator it=bought_vm.begin(); it != bought_vm.end(); it++ )
  {
    GoodType gtype = GoodHelper::getType( it->first );
    _d->buyStore.setCurrentQty( gtype, it->second.toInt() );
  }
}

const GoodStore& ComputerCity::getSells() const
{
  return _d->sellStore;
}

const GoodStore& ComputerCity::getBuys() const
{
  return _d->buyStore;
}

EmpireCityPtr ComputerCity::create( EmpirePtr empire, const std::string& name )
{
  EmpireCityPtr ret( new ComputerCity( empire, name ) );
  ret->drop();

  return ret;
}

void ComputerCity::resolveMerchantArrived( EmpireMerchantPtr )
{

}

ComputerCity::~ComputerCity()
{

}