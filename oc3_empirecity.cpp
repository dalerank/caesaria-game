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

#include "oc3_empirecity.hpp"
#include "oc3_goodstore_simple.hpp"
#include "oc3_goodhelper.hpp"

class EmpireCity::Impl
{
public:
  Point location;
  std::string name;
  bool distantCity;
  bool isTradeActive;
  SimpleGoodStore sellStore;
  SimpleGoodStore buyStore;
};

EmpireCity::EmpireCity( const std::string& name ) : _d( new Impl )
{
  _d->name = name;
  _d->distantCity = false;
  _d->isTradeActive = false;
}

std::string EmpireCity::getName() const
{
  return _d->name;
}

Point EmpireCity::getLocation() const
{
  return _d->location;
}

EmpireCity::~EmpireCity()
{

}

void EmpireCity::setLocation( const Point& location )
{
  _d->location = location;
}

bool EmpireCity::isDistantCity() const
{
  return _d->distantCity;
}

bool EmpireCity::isTradeActive() const
{
  return _d->isTradeActive;
}

void EmpireCity::save( VariantMap& options ) const
{

}

void EmpireCity::load( const VariantMap& options )
{
  setLocation( options.get( "location" ).toPoint() );

  const VariantMap& sells_vm = options.get( "sells" ).toMap();
  for( VariantMap::const_iterator it=sells_vm.begin(); it != sells_vm.end(); it++ )
  {
    GoodType gtype = GoodHelper::getType( it->first );
    _d->sellStore.setMaxQty( gtype, it->second.toInt() );
  }

  const VariantMap& buys_vm = options.get( "buys" ).toMap();
  for( VariantMap::const_iterator it=buys_vm.begin(); it != buys_vm.end(); it++ )
  {
    GoodType gtype = GoodHelper::getType( it->first );
    _d->buyStore.setMaxQty( gtype, it->second.toInt() );
  }
}

GoodStore& EmpireCity::getSells()
{
  return _d->sellStore;
}

GoodStore& EmpireCity::getBuys()
{
  return _d->buyStore;
}