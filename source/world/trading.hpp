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

#ifndef _OPENCAESAR3_WORLD_TRADING_INCLUDE_H_
#define _OPENCAESAR3_WORLD_TRADING_INCLUDE_H_

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

class Trading
{
public:
  Trading();
  ~Trading();

  void update( unsigned int time );
  void init( EmpirePtr empire );

  VariantMap save() const;
  void load(const VariantMap& stream);

  TradeRoutePtr getRoute( const std::string& begin, const std::string& end );
  TradeRoutePtr getRoute( unsigned int index );
  TradeRouteList getRoutes( const std::string& begin );
  TradeRouteList getRoutes();
  TradeRoutePtr createRoute( const std::string& begin, const std::string& end );

  void sendMerchant( const std::string& begin, const std::string& end, 
                     GoodStore& sell, GoodStore& buy );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace world

#endif //_OPENCAESAR3_WORLD_TRADING_INCLUDE_H_
