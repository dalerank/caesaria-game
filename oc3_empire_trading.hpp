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

#ifndef _OPENCAESAR3_EMPIRE_TRADING_INCLUDE_H_
#define _OPENCAESAR3_EMPIRE_TRADING_INCLUDE_H_

#include "oc3_cityservice.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_referencecounted.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_signals.hpp"
#include "oc3_positioni.hpp"

class GoodStore;

class EmpireTradeRoute : public ReferenceCounted
{
public:
  EmpireTradeRoute( EmpireCityPtr begin, EmpireCityPtr end );
  ~EmpireTradeRoute();

  EmpireCityPtr getBeginCity() const;
  EmpireCityPtr getEndCity() const;

  void update( unsigned int time );

  void addMerchant( const std::string& begin, GoodStore& sell, GoodStore& buy );
  EmpireMerchantPtr getMerchant( unsigned int index );

  VariantMap save() const;
  void load( const VariantMap& stream );

oc3_signals public:
  Signal1<EmpireMerchantPtr>& onMerchantArrived();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

class EmpireMerchant : public ReferenceCounted
{
public:
  static EmpireMerchantPtr create( EmpireTradeRoute& route, const std::string& start,
                                   GoodStore& sell, GoodStore& buy );
  ~EmpireMerchant();

  Point getLocation() const;
  void update( unsigned int time );

  EmpireCityPtr getBaseCity() const;
  GoodStore& getSellGoods();
  GoodStore& getBuyGoods();

  bool isDeleted() const;
  void deleteLater();

  VariantMap save() const;
  void load( const VariantMap& stream );

oc3_signals public:
  Signal1<EmpireMerchantPtr>& onDestination();

private:
  EmpireMerchant();

  class Impl;
  ScopedPtr< Impl > _d;
};

class EmpireTrading
{
public:
  EmpireTrading();
  ~EmpireTrading();

  void update( unsigned int time );
  void init( EmpirePtr empire );

  EmpireTradeRoutePtr getRoute( const std::string& begin, const std::string& end );
  EmpireTradeRoutePtr getRoute( unsigned int index );
  EmpireTradeRouteList getRoutes( const std::string& begin );
  EmpireTradeRoutePtr createRoute( const std::string& begin, const std::string& end );

  void sendMerchant( const std::string& begin, const std::string& end, 
                     GoodStore& sell, GoodStore& buy );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_OPENCAESAR3_EMPIRE_TRADING_INCLUDE_H_
