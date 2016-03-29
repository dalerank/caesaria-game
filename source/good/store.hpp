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
#include "stock.hpp"
#include "orders.hpp"
#include "core/variant.hpp"
#include "turnover.hpp"

#include <set>

namespace good
{

struct ReserveInfo
{
  good::Stock stock;
  DateTime time;
  unsigned int id;

  int qty() const { return stock.qty(); }
  good::Product type() const { return stock.type(); }

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

class Storage;
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
  virtual ProductMap details() const;
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
  virtual bool applyStorageReservation(good::Stock& stock, const int reservationID) = 0;
  virtual bool applyRetrieveReservation(good::Stock& stock, const int reservationID) = 0;

  virtual void confirmDeliver( good::Product type, int qty, unsigned int tag, const DateTime& time );

  virtual const ConsumerDetails& consumers() const;
  virtual const ProviderDetails& providers() const;

  // store/retrieve to goodStore
  void applyStorageReservation(Storage& goodStore, const int reservationID);
  void applyRetrieveReservation(Storage& goodStore, const int reservationID);

  ProductMap amounts() const;

  // immediate store/retrieve, exception if impossible
  virtual void store( good::Stock& stock, const int amounts);
  virtual void retrieve( good::Stock& stock, const int amounts);

  // store all goods from the given goodStore
  virtual void storeAll( Store& goodStore);

  virtual bool isDevastation() const;
  virtual void setDevastation( bool value );

  virtual VariantMap save() const;
  virtual void load( const VariantMap& stream );

  virtual TilePos owner() const;

  virtual void setOrder( const good::Product type, const Orders::Order order );
  virtual Orders::Order getOrder( const good::Product type ) const;

  virtual void removeExpired( DateTime date );

protected:
  Reservations& _getStoreReservations();
  Reservations& _getRetrieveReservations();
  ConsumerDetails& _consumers();
  ProviderDetails& _providers();

private:
  __DECLARE_IMPL(Store)
};

}//end namespace good

#endif //__CAESARIA_GOODSTORE_H_INCLUDED__
