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

#ifndef __OPENCAESAR3_EMPIRE_CITY_COMPUTER_H_INCLUDED__
#define __OPENCAESAR3_EMPIRE_CITY_COMPUTER_H_INCLUDED__

#include "city.hpp"

namespace world
{

class ComputerCity : public City
{
public:
  static CityPtr create( EmpirePtr empire, const std::string& name );

  ~ComputerCity();

  virtual std::string getName() const;
  virtual Point getLocation() const;
  virtual void setLocation( const Point& location );

  bool isDistantCity() const;
  virtual bool isAvailable() const;
  virtual void setAvailable(bool value);

  virtual void timeStep( unsigned int time );

  virtual void save( VariantMap& options ) const;
  virtual void load( const VariantMap& options );

  virtual const GoodStore& getSells() const;
  virtual const GoodStore& getBuys() const;

  virtual EmpirePtr getEmpire() const;

  void resolveMerchantArrived( MerchantPtr );

protected:
  ComputerCity( EmpirePtr empire, const std::string& name );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace world

#endif //__OPENCAESAR3_EMPIRE_CITY_COMPUTER_H_INCLUDED__
