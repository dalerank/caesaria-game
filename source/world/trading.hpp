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

#ifndef _CAESARIA_WORLD_TRADING_INCLUDE_H_
#define _CAESARIA_WORLD_TRADING_INCLUDE_H_

//#include "cityservice.hpp"
#include "core/scopedptr.hpp"
#include "core/referencecounted.hpp"
#include "predefinitions.hpp"
#include "core/signals.hpp"
#include "core/position.hpp"
#include "traderoute.hpp"
#include "good/good.hpp"

namespace world
{

class Trading
{
public:
  Trading();
  ~Trading();

  void timeStep( unsigned int time );
  void init( EmpirePtr empire );

  VariantMap save() const;
  void load(const VariantMap& stream);

  TraderoutePtr findRoute( const std::string& begin, const std::string& end );
  TraderoutePtr findRoute( unsigned int index );
  TraderouteList routes( const std::string& begin );
  TraderouteList routes();
  TraderoutePtr createRoute( const std::string& begin, const std::string& end );

  void setPrice( good::Product gtype, int bCost, int sCost );
  void getPrice( good::Product gtype, int& bCost, int& sCost );

  void sendMerchant( const std::string& begin, const std::string& end, 
                     good::Store& sell, good::Store& buy );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace world

#endif //_CAESARIA_WORLD_TRADING_INCLUDE_H_
