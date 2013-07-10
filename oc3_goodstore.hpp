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

#ifndef __OPENCAESAR3_GOODSTORE_H_INCLUDED__
#define __OPENCAESAR3_GOODSTORE_H_INCLUDED__

#include "oc3_serializer.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_good.hpp"
#include "oc3_goodorders.hpp"

class SimpleGoodStore;
class GoodStore : public Serializable
{
public:
  GoodStore();
  virtual ~GoodStore();

  virtual int getCurrentQty(const GoodType &goodType) const = 0;
  virtual int getCurrentQty() const = 0;
  virtual int getMaxQty() const = 0;

  // returns the max quantity that can be stored now
  virtual int getMaxStore(const GoodType goodType) = 0;

  // returns the max quantity that can be retrieved now
  int getMaxRetrieve(const GoodType goodType);

  // returns the reservationID if stock can be retrieved (else 0)
  virtual long reserveStorage(GoodStock &stock);

  // returns the reservationID if stock can be retrieved (else 0)
  virtual long reserveRetrieval(GoodStock &stock);

  // return the reservation
  GoodStock getStorageReservation(const long reservationID, const bool pop=false);
  GoodStock getRetrieveReservation(const long reservationID, const bool pop=false);

  // store/retrieve
  virtual void applyStorageReservation(GoodStock &stock, const long reservationID) = 0;
  virtual void applyRetrieveReservation(GoodStock &stock, const long reservationID) = 0;

  // store/retrieve to goodStore
  void applyStorageReservation(SimpleGoodStore &goodStore, const long reservationID);
  void applyRetrieveReservation(SimpleGoodStore &goodStore, const long reservationID);

  // immediate store/retrieve, exception if impossible
  virtual void store(GoodStock &stock, const int amount);
  virtual void retrieve(GoodStock &stock, const int amount);

  // store all goods from the given goodStore
  virtual void storeAll(SimpleGoodStore &goodStore);

  bool isDevastation() const;
  void setDevastation( bool value );

  void save( VariantMap& stream ) const;
  void load( const VariantMap& stream );

  virtual void setOrder( const GoodType type, const GoodOrders::Order order );
  virtual GoodOrders::Order getOrder( const GoodType type ) const;

protected:
  typedef std::map<long, GoodStock> _Reservations;
  _Reservations& _getStoreReservations();
  _Reservations& _getRetrieveReservations();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_GOODSTORE_H_INCLUDED__