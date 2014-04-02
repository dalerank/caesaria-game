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

#ifndef __CAESARIA_EMPIRECITY_H_INCLUDED__
#define __CAESARIA_EMPIRECITY_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "core/referencecounted.hpp"
#include "core/position.hpp"
#include "predefinitions.hpp"
#include "core/serializer.hpp"

class GoodStore;

namespace world
{

class City : public ReferenceCounted, public Serializable
{
public:
  virtual std::string getName() const = 0;
  virtual Point location() const = 0;
  virtual void setLocation( const Point& location ) = 0;

  // performs one simulation step
  virtual void timeStep( unsigned int time ) = 0;  
  virtual bool isAvailable() const { return true; }
  virtual void setAvailable( bool value ) {}
  virtual void arrivedMerchant( MerchantPtr ) = 0;
  virtual unsigned int getTradeType() const = 0;
  virtual EmpirePtr empire() const = 0;

  virtual const GoodStore& getSells() const = 0;
  virtual const GoodStore& getBuys() const = 0;
};

}//end namespace world

#endif //__CAESARIA_EMPIRECITY_H_INCLUDED__
