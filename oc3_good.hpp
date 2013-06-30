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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef GOOD_HPP
#define GOOD_HPP

#include "oc3_enums.hpp"
#include <string>
#include <vector>
#include <map>
#include "oc3_serializer.hpp"

class VariantList;

class Good
{
  friend class GoodHelper;
public:
  typedef enum { accept=0, reject, deliver, none } Order;

  std::string getName();
  int getImportPrice();
  int getExportPrice();
  bool isAllowUsage();
  bool isAllowStorage();
  bool isAllowImport();
  bool isAllowExport();

private:
  void init(const GoodType &goodType);

  GoodType _goodType;
  BuildingType _outFactoryType;  // type of factory, if any (ex: G_IRON => B_WEAPONS)
  std::string _name;
  int _importPrice;
  int _exportPrice;

  bool _allowUsage;
  bool _allowStorage;
  bool _allowImport;
  bool _allowExport;

  int _usageQuota;   // percent of (rich) population to give access to this product
  int _importTreshold;  // number of units in warehouses above which no import is made
  int _exportTreshold;  // number of units in warehouses under which no export is made
};


class GoodStock 
{
public:
  GoodStock();
  GoodStock(const GoodType &goodType, const int maxQty, const int currentQty=0);

  /** amount: if -1, amount=stock._currentQty */
  void addStock(GoodStock &stock, const int amount = -1);

  void save( VariantList& stream ) const;
  void load( const VariantList& options );

  GoodType _goodType;
  int _maxQty;
  int _currentQty;
};


class SimpleGoodStore;
class GoodStore : public Serializable
{
public:
  GoodStore();
  virtual ~GoodStore();

  virtual int getCurrentQty(const GoodType &goodType) const = 0;
  virtual int getCurrentQty() const = 0;

  // returns the max quantity that can be stored now
  virtual int getMaxStore(const GoodType goodType) = 0;

  // returns the max quantity that can be retrieved now
  int getMaxRetrieve(const GoodType goodType);

  // returns the reservationID if stock can be retrieved (else 0)
  long reserveStorage(GoodStock &stock);

  // returns the reservationID if stock can be retrieved (else 0)
  long reserveRetrieval(GoodStock &stock);

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
  void store(GoodStock &stock, const int amount);
  void retrieve(GoodStock &stock, const int amount);

  // store all goods from the given goodStore
  void storeAll(SimpleGoodStore &goodStore);

  bool isDevastation() const;
  void setDevastation( bool value );

  void save( VariantMap& stream ) const;
  void load( const VariantMap& stream );

protected:
  long _nextReservationID;
  bool _devastation;
  std::map<long, GoodStock> _storeReservations;  // key=reservationID, value=stock
  std::map<long, GoodStock> _retrieveReservations;  // key=reservationID, value=stock
};


class SimpleGoodStore : public GoodStore
{
public:
  using GoodStore::applyStorageReservation;
  using GoodStore::applyRetrieveReservation;

  SimpleGoodStore();

  void setMaxQty(const int maxQty);
  int getMaxQty();
  int getCurrentQty() const;
  void computeCurrentQty();

  GoodStock& getStock(const GoodType &goodType);
  int getCurrentQty(const GoodType &goodType) const;
  int getMaxQty(const GoodType &goodType);
  void setMaxQty(const GoodType &goodType, const int maxQty);
  void setCurrentQty(const GoodType &goodType, const int currentQty);

  // returns the max quantity that can be stored now
  int getMaxStore(const GoodType goodType);

  // store/retrieve
  void applyStorageReservation(GoodStock &stock, const long reservationID);
  void applyRetrieveReservation(GoodStock &stock, const long reservationID);

  void save( VariantMap& stream ) const;
  void load( const VariantMap& stream );

private:
  std::vector<GoodStock> _goodStockList;
  int _maxQty;
  int _currentQty;
};

typedef std::map< GoodType, Good::Order > GoodOrders;

#endif
