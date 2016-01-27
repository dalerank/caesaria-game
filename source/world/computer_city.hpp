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

#ifndef __CAESARIA_EMPIRE_CITY_COMPUTER_H_INCLUDED__
#define __CAESARIA_EMPIRE_CITY_COMPUTER_H_INCLUDED__

#include "city.hpp"

namespace world
{

class ComputerCity : public City
{
public:
  static CityPtr create( EmpirePtr empire, const std::string& name );

  virtual ~ComputerCity();

  virtual econ::Treasury& treasury();
  virtual bool isPaysTaxes() const;
  virtual bool haveOverduePayment() const;
  virtual void setModeAI(AiMode mode);
  virtual AiMode modeAI() const;

  virtual bool isAvailable() const;
  virtual void setAvailable(bool value);
  virtual SmartPtr<Player> mayor() const;
  virtual void timeStep( unsigned int time );
  virtual DateTime lastAttack() const;
  virtual std::string about(AboutType type);
  virtual void save( VariantMap& options ) const;
  virtual void load( const VariantMap& options );
  virtual const good::Store& sells() const;
  virtual const good::Store& buys() const;
  virtual const city::States& states() const;
  virtual void delayTrade( unsigned int month );
  virtual void empirePricesChanged(good::Product gtype, const PriceInfo& prices);
  virtual unsigned int tradeType() const;
  virtual int strength() const;
  virtual void addObject(ObjectPtr object);

  void changeTradeOptions( const VariantMap& stream );
  void __debugSendMerchant();

protected:
  ComputerCity( EmpirePtr empire, const std::string& name );
  bool _mayTrade() const;
  void _initTextures();
  void _resetGoodState(good::Product pr );
  void _checkMerchantsDeadline();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace world

#endif //__CAESARIA_EMPIRE_CITY_COMPUTER_H_INCLUDED__
