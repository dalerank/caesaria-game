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

#ifndef __CAESARIA_ROME_H_INCLUDED__
#define __CAESARIA_ROME_H_INCLUDED__

#include "city.hpp"

namespace world
{

class Rome : public City
{
public:
  static const char* defaultName;
  Rome( EmpirePtr empire );

  // performs one simulation step
  virtual bool isAvailable() const { return true; }

  virtual unsigned int tradeType() const;
  virtual city::Funds& funds();
  virtual std::string name() const;
  virtual unsigned int population() const;
  virtual bool isPaysTaxes() const;
  virtual unsigned int age() const;
  virtual void timeStep(const unsigned int time);
  virtual SmartPtr<Player> player() const;
  virtual bool haveOverduePayment() const;
  virtual void addObject(ObjectPtr);
  virtual world::Nation nation() const;
  virtual DateTime lastAttack() const;
  virtual int strength() const;
  virtual void delayTrade( unsigned int month );
  virtual void empirePricesChanged( good::Product gtype, int bCost, int sCost );
  virtual const good::Store& importingGoods() const;
  virtual const good::Store& exportingGoods() const;

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace world

#endif //__CAESARIA_ROME_H_INCLUDED__
