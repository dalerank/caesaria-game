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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_CART_SUPPLIER_H_INCLUDED__
#define __CAESARIA_CART_SUPPLIER_H_INCLUDED__

#include "human.hpp"
#include "core/predefinitions.hpp"

/** This walker delivers goods */
class CartSupplier : public Human
{
public:
  static CartSupplierPtr create( PlayerCityPtr city );

  void setDestinationBuilding( BuildingPtr building );
  void setBaseBuilding( BuildingPtr building );
    
  virtual void getPictures( gfx::Pictures& oPics);

  void send2city(BuildingPtr building, good::Product what, const int qty );

  void computeWalkerDestination( BuildingPtr building, const good::Product type, const int qty );
 
  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);
  virtual bool die();

  virtual void timeStep(const unsigned long time);
  virtual TilePos places(Place type) const;

protected:
  CartSupplier( PlayerCityPtr city );
  virtual const gfx::Animation& _cart();
  virtual void _changeDirection();
  virtual void _reachedPathway();

  void _reserveStorage();
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_CART_SUPPLIER_H_INCLUDED__
