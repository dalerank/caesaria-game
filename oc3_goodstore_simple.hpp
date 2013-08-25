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

#ifndef __OPENCAESAR3_GOODSTORE_SIMPLE_H_INCLUDED__
#define __OPENCAESAR3_GOODSTORE_SIMPLE_H_INCLUDED__

#include "oc3_goodstore.hpp"
#include "oc3_scopedptr.hpp"

class SimpleGoodStore : public GoodStore
{
public:
  using GoodStore::applyStorageReservation;
  using GoodStore::applyRetrieveReservation;

  SimpleGoodStore();
  ~SimpleGoodStore();

  void setMaxQty(const int maxQty);
  int getMaxQty() const;
  int getCurrentQty() const;

  void resize( const GoodStore& other );

  GoodStock& getStock(const Good::Type &goodType);
  virtual int getCurrentQty(const Good::Type &goodType) const;
  int getMaxQty(const Good::Type &goodType) const;
  void setMaxQty(const Good::Type &goodType, const int maxQty);
  void setCurrentQty(const Good::Type &goodType, const int currentQty);

  // returns the max quantity that can be stored now
  int getMaxStore(const Good::Type goodType);

  // store/retrieve
  virtual void applyStorageReservation(GoodStock &stock, const long reservationID);
  virtual void applyRetrieveReservation(GoodStock &stock, const long reservationID);

  virtual VariantMap save() const;
  virtual void load( const VariantMap& stream );

private:
  std::vector<GoodStock> _goodStockList;
  int _maxQty;

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_GOODSTORE_SIMPLE_H_INCLUDED__
