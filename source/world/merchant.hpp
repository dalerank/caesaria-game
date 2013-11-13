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

#ifndef _OPENCAESAR3_WORLD_MERCHANT_INCLUDE_H_
#define _OPENCAESAR3_WORLD_MERCHANT_INCLUDE_H_

//#include "cityservice.hpp"
#include "core/scopedptr.hpp"
#include "core/referencecounted.hpp"
#include "predefinitions.hpp"
#include "core/signals.hpp"
#include "core/position.hpp"
#include "traderoute.hpp"

class GoodStore;

namespace world
{

class Merchant : public ReferenceCounted
{
public:
  static MerchantPtr create( TradeRoute& route, const std::string& start,
                                   GoodStore& sell, GoodStore& buy );
  ~Merchant();

  Point getLocation() const;
  void update( unsigned int time );

  CityPtr getBaseCity() const;
  GoodStore& getSellGoods();
  GoodStore& getBuyGoods();

  bool isDeleted() const;
  void deleteLater();

  VariantMap save() const;
  void load( const VariantMap& stream );

oc3_signals public:
  Signal1<MerchantPtr>& onDestination();

private:
  Merchant();

  class Impl;
  ScopedPtr< Impl > _d;
};

} //end namespace world

#endif //_OPENCAESAR3_WORLD_TRADING_INCLUDE_H_
