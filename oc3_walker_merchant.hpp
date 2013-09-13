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

#ifndef _OPENCAESAR3_WALKER_MERCHANT_INCLUDE_H_
#define _OPENCAESAR3_WALKER_MERCHANT_INCLUDE_H_

#include "oc3_walker.hpp"
#include "oc3_empire_trading.hpp"

/** This is the empire merchant which buy/sell goods at warehouses */
class Merchant : public Walker
{
public:
  static WalkerPtr create( CityPtr city, EmpireMerchantPtr merchant );

  virtual ~Merchant();

  void send2City();
  virtual void onDestination();

  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);
private:
  Merchant( CityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_OPENCAESAR3_WALKER_MERCHANT_INCLUDE_H_
