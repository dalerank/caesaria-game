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

#ifndef __OPENCAESAR3_MARKET_H_INCLUDED__
#define __OPENCAESAR3_MARKET_H_INCLUDED__

#include "oc3_building_service.hpp"
#include "core/predefinitions.hpp"

class GoodStore;

class Market: public ServiceBuilding
{
public:
  Market();

  GoodStore& getGoodStore();
  std::list<Good::Type> getMostNeededGoods();  // used by the market buyer

  // returns the quantity of needed goods
  int getGoodDemand(const Good::Type &goodType);  // used by the market buyer

  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);

  void timeStep(const unsigned long time);

  void deliverService();

  virtual unsigned int getWalkerDistance() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif
