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

#ifndef __CAESARIA_GOODSTORE_H_INCLUDED__
#define __CAESARIA_GOODSTORE_H_INCLUDED__

#include "core/serializer.hpp"
#include "core/scopedptr.hpp"
#include "good.hpp"
#include "goodorders.hpp"
#include "core/variant.hpp"

#include <set>

struct ReserveInfo
{
  GoodStock stock;
  DateTime time;
  unsigned long id;

  bool operator<(const ReserveInfo& a ) const
  {
    return id < a.id;
  }
};

class Reservations : public std::set<ReserveInfo>
{
  static const ReserveInfo invalid;
public:
  Reservations();
  const ReserveInfo& get(unsigned long id) const;

  unsigned long push( const GoodStock& stock, DateTime time );
  bool pop( unsigned int id );

  void removeExpired(DateTime currentDate, int monthNumber );

  VariantMap save() const;
  void load( const VariantMap& stream );

private:
  unsigned long _idCounter;
};

class SimpleGoodStore;
class GoodStore
{
public:
  GoodStore();
  virtual ~GoodStore();

  virtual int qty(const Good::Type &goodType) const = 0;
  virtual int qty() const = 0;
  virtual void setCapacity(const int maxcap) = 0;
  virtual int capacity() const = 0;
  virtual int capacity(const Good::Type& goodType ) const = 0;
  virtual int freeQty( const Good::Type& goodType ) const;
  virtual int freeQty() const;

  // returns the max quantity that can be stored now
  virtual int getMaxStore(const Good::Type goodType) = 0;

  // returns the max quantity that can be retrieved now
  virtual int getMaxRetrieve(const Good::Type goodType);

  // returns the reservationID if stock can be retrieved (else 0)
  virtual long reserveStorage( GoodStock &stock, DateTime time );
  virtual long reserveStorage( Good::Type what, unsigned int qty, DateTime time);

  // returns the reservationID if stock can be retrieved (else 0)
  virtual long reserveRetrieval(GoodStock &stock, DateTime time);
  virtual long reserveRetrieval(Good::Type what, unsigned int qty, DateTime time);

  // return the reservation
  GoodStock getStorageReservation(const long reservationID, const bool pop=false);
  GoodStock getRetrieveReservation(const long reservationID, const bool pop=false);

  // store/retrieve
  virtual void applyStorageReservation(GoodStock &stock, const long reservationID) = 0;
  virtual void applyRetrieveReservation(GoodStock &stock, const long reservationID) = 0;

  // store/retrieve to goodStore
  void applyStorageReservation(SimpleGoodStore& goodStore, const long reservationID);
  void applyRetrieveReservation(SimpleGoodStore& goodStore, const long reservationID);

  // immediate store/retrieve, exception if impossible
  virtual void store( GoodStock &stock, const int amount);
  virtual void retrieve( GoodStock &stock, const int amount);

  // store all goods from the given goodStore
  virtual void storeAll( GoodStore &goodStore);

  virtual bool isDevastation() const;
  virtual void setDevastation( bool value );

  virtual VariantMap save() const;
  virtual void load( const VariantMap& stream );

  virtual void setOrder( const Good::Type type, const GoodOrders::Order order );
  virtual GoodOrders::Order getOrder( const Good::Type type ) const;

  virtual void removeExpired( DateTime date );

protected:
  Reservations& _getStoreReservations();
  Reservations& _getRetrieveReservations();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_GOODSTORE_H_INCLUDED__
