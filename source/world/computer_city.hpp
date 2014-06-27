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

  virtual std::string name() const;
  virtual Point location() const;
  virtual void setLocation( const Point& location );
  virtual city::Funds& funds();
  virtual unsigned int population() const;
  virtual bool isPaysTaxes() const;

  bool isDistantCity() const;
  bool isRomeCity() const;
  virtual bool isAvailable() const;
  virtual void setAvailable(bool value);

  virtual void timeStep( unsigned int time );

  virtual gfx::Picture picture() const;

  virtual void save( VariantMap& options ) const;
  virtual void load( const VariantMap& options );

  virtual const GoodStore& importingGoods() const;
  virtual const GoodStore& exportingGoods() const;

  virtual void delayTrade(unsigned int month);
  virtual void empirePricesChanged(Good::Type gtype, int bCost, int sCost);

  virtual unsigned int tradeType() const;

  virtual void addObject(ObjectPtr object);

  void changeTradeOptions( const VariantMap& stream );

protected:
  ComputerCity( EmpirePtr empire, const std::string& name );
  bool _mayTrade() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace world

#endif //__CAESARIA_EMPIRE_CITY_COMPUTER_H_INCLUDED__
