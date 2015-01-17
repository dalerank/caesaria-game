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

namespace good
{

struct ReserveInfo
{
  good::Stock stock;
  DateTime time;
  unsigned int id;

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
  const ReserveInfo& get(unsigned int id) const;

  unsigned int push( const good::Stock& stock, DateTime time );
  bool pop( unsigned int id );

  void removeExpired(DateTime currentDate, int monthNumber );

  VariantMap save() const;
  void load( const VariantMap& stream );

private:
  unsigned int _idCounter;
};

class SimpleStore;
class Store
{
public:
  Store();
  virtual ~Store();

  virtual int qty(const good::Product& goodType) const = 0;
  virtual int qty() const = 0;

  virtual void setCapacity(const int maxcap) = 0;
  virtual void setCapacity(const good::Product& goodType, const int maxQty) = 0;

  virtual int capacity() const = 0;
  virtual int capacity(const good::Product& goodType ) const = 0;

  virtual int freeQty( const good::Product& goodType ) const;
  virtual int freeQty() const;

  virtual bool empty() const;

  // returns the max quantity that can be stored now
  virtual int getMaxStore(const good::Product goodType) = 0;

  // returns the max quantity that can be retrieved now
  virtual int getMaxRetrieve(const good::Product goodType);

  // returns the reservationID if stock can be retrieved (else 0)
  virtual int reserveStorage( good::Stock& stock, DateTime time );
  virtual int reserveStorage( good::Product what, unsigned int qty, DateTime time);

  // returns the reservationID if stock can be retrieved (else 0)
  virtual int reserveRetrieval(good::Stock& stock, DateTime time);
  virtual int reserveRetrieval(good::Product what, unsigned int qty, DateTime time);

  // return the reservation
  good::Stock getStorageReservation(const int reservationID, const bool pop=false);
  good::Stock getRetrieveReservation(const int reservationID, const bool pop=false);

  // store/retrieve
  virtual void applyStorageReservation(good::Stock& stock, const int reservationID) = 0;
  virtual void applyRetrieveReservation(good::Stock& stock, const int reservationID) = 0;

  // store/retrieve to goodStore
  void applyStorageReservation(SimpleStore& goodStore, const int reservationID);
  void applyRetrieveReservation(SimpleStore& goodStore, const int reservationID);

  // immediate store/retrieve, exception if impossible
  virtual void store( good::Stock& stock, const int amount);
  virtual void retrieve( good::Stock& stock, const int amount);

  // store all goods from the given goodStore
  virtual void storeAll( Store &goodStore);

  virtual bool isDevastation() const;
  virtual void setDevastation( bool value );

  virtual VariantMap save() const;
  virtual void load( const VariantMap& stream );

  virtual void setOrder( const good::Product type, const Orders::Order order );
  virtual Orders::Order getOrder( const good::Product type ) const;

  virtual void removeExpired( DateTime date );

protected:
  Reservations& _getStoreReservations();
  Reservations& _getRetrieveReservations();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace good

#endif //__CAESARIA_GOODSTORE_H_INCLUDED__
