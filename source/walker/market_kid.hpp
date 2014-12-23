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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_MARKET_KID_H_INCLUDED__
#define __CAESARIA_MARKET_KID_H_INCLUDED__

#include "human.hpp"

class MarketKid : public Human
{
public:
  static MarketKidPtr create( PlayerCityPtr city );
  static MarketKidPtr create( PlayerCityPtr city, MarketBuyerPtr lady );

  good::Stock& getBasket();
  void setDelay( int delay );

  void send2City( MarketPtr destination );

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual void timeStep(const unsigned long time);
  virtual bool die();

protected: 
  virtual void _updateThoughts();
  virtual void _reachedPathway();

private:
  MarketKid( PlayerCityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_MARKET_KID_H_INCLUDED__
