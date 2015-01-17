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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_WAREHOUSESTORE_HPP_INCLUDE_
#define _CAESARIA_WAREHOUSESTORE_HPP_INCLUDE_

#include "warehouse.hpp"
#include "game/enums.hpp"
#include "good/goodstore.hpp"

class WarehouseStore : public good::Store
{
public:
  typedef std::map< good::Product, int > StockMap;

  WarehouseStore();

  void init(Warehouse &_warehouse);

  virtual int qty(const good::Product &goodType) const;
  virtual int qty() const;
  virtual int capacity() const;
  virtual void setCapacity( const int maxcap);
  virtual void setCapacity(const good::Product& goodType, const int maxQty);
  virtual int capacity(const good::Product& goodType ) const;

  // returns the max quantity that can be stored now
  virtual int getMaxStore(const good::Product goodType);

  // store/retrieve
  virtual void applyStorageReservation(good::Stock& stock, const int reservationID);
  virtual void applyRetrieveReservation(good::Stock& stock, const int reservationID);

  virtual void retrieve( good::Stock& stock, const int amount);
  virtual VariantMap save() const;
  virtual void load(const VariantMap &stream);

private:
  Warehouse* _warehouse;
  StockMap _capacities;
};

#endif //_CAESARIA_WAREHOUSESTORE_HPP_INCLUDE_
