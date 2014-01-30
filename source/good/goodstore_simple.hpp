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

#ifndef __CAESARIA_GOODSTORE_SIMPLE_H_INCLUDED__
#define __CAESARIA_GOODSTORE_SIMPLE_H_INCLUDED__

#include "goodstore.hpp"
#include "core/scopedptr.hpp"

class SimpleGoodStore : public GoodStore
{
public:
  //using GoodStore::applyStorageReservation;
  //using GoodStore::applyRetrieveReservation;

  SimpleGoodStore();
  ~SimpleGoodStore();

  void setCapacity(const int maxQty);
  int capacity() const;
  int getQty() const;

  void resize( const GoodStore& other );

  GoodStock& getStock(const Good::Type &goodType);  

  virtual int getQty(const Good::Type &goodType) const;
  int capacity(const Good::Type &goodType) const;
  void setCapacity(const Good::Type &goodType, const int maxQty);
  void setQty(const Good::Type &goodType, const int currentQty);

  // returns the max quantity that can be stored now
  int getMaxStore(const Good::Type goodType);

  // store/retrieve
  virtual void applyStorageReservation(GoodStock &stock, const long reservationID);
  virtual void applyRetrieveReservation(GoodStock &stock, const long reservationID);

  virtual VariantMap save() const;
  virtual void load( const VariantMap& stream );

private:
  class Impl;
  ScopedPtr< Impl > _gsd;
};

#endif //__CAESARIA_GOODSTORE_SIMPLE_H_INCLUDED__
