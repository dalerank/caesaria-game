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

#include "oc3_goodstore.hpp"
#include "oc3_goodstore_simple.hpp"
#include "oc3_goodorders.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_foreach.hpp"

class GoodStore::Impl
{
public:
  long nextReservationID;
  bool devastation;

  _Reservations storeReservations;  // key=reservationID, value=stock
  _Reservations retrieveReservations;  // key=reservationID, value=stock
  GoodOrders goodOrders;
};

GoodStore::GoodStore() : _d( new Impl )
{
  _d->nextReservationID = 1;
  _d->devastation = false;
}


int GoodStore::getMaxRetrieve(const Good::Type goodType)
{
  // current good quantity
  int qty = getCurrentQty(goodType);

  // remove all retrieval reservations
  foreach( _Reservations::value_type& item, _d->retrieveReservations )
  {
    qty -= item.second._currentQty;
  }

  return qty;
}


long GoodStore::reserveStorage(GoodStock &stock)
{
  long reservationID = 0;

  // current free capacity
  if( getMaxStore(stock.type() ) >= stock._currentQty )
  {
    // the stock can be stored!
    reservationID = _d->nextReservationID;
    _d->storeReservations.insert(std::make_pair(reservationID, stock));
    _d->nextReservationID++;
  }
  // std::cout << "GoodStore, reserve store qty=" << stock._currentQty << " resID=" << reservationID << std::endl;

  return reservationID;
}


long GoodStore::reserveRetrieval(GoodStock &stock)
{
  long reservationID = 0;

  // current good quantity
  if (getMaxRetrieve(stock.type()) >= stock._currentQty)
  {
    // the stock can be retrieved!
    reservationID = _d->nextReservationID;
    _d->retrieveReservations.insert(std::make_pair(reservationID, stock));
    _d->nextReservationID++;
  }
  // std::cout << "GoodStore, reserve retrieve qty=" << stock._currentQty << " resID=" << reservationID << std::endl;

  return reservationID;
}


GoodStock GoodStore::getStorageReservation(const long reservationID, const bool pop)
{
  _Reservations::iterator mapIt = _d->storeReservations.find(reservationID);

  if (mapIt == _d->storeReservations.end())
  {
    _OC3_DEBUG_BREAK_IF( "Unknown reservationID" );
    return GoodStock();
  }

  GoodStock reservedStock = mapIt->second;

  if (pop)
  {
    _d->storeReservations.erase(mapIt);
  }

  return reservedStock;
}


GoodStock GoodStore::getRetrieveReservation(const long reservationID, const bool pop)
{
  _Reservations::iterator mapIt = _d->retrieveReservations.find(reservationID);

  if (mapIt == _d->retrieveReservations.end())
  {
    _OC3_DEBUG_BREAK_IF("Unknown reservationID");
    return GoodStock();
  }

  GoodStock reservedStock = mapIt->second;

  if (pop)
  {
    _d->retrieveReservations.erase(mapIt);
  }

  return reservedStock;
}


void GoodStore::applyStorageReservation(SimpleGoodStore &goodStore, const long reservationID)
{
  GoodStock reservedStock = getStorageReservation(reservationID);
  GoodStock &stock = goodStore.getStock(reservedStock.type() );

  applyStorageReservation(stock, reservationID);
}


void GoodStore::applyRetrieveReservation(SimpleGoodStore &goodStore, const long reservationID)
{
  GoodStock reservedStock = getRetrieveReservation(reservationID);
  GoodStock &stock = goodStore.getStock(reservedStock.type() );

  applyRetrieveReservation(stock, reservationID);
}

void GoodStore::store( GoodStock &stock, const int amount)
{
  GoodStock reservedStock;
  reservedStock.setType( stock.type() );
  reservedStock._currentQty = amount;

  long reservationID = reserveStorage(reservedStock);

  if( reservationID > 0 )
  {
    applyStorageReservation(stock, reservationID);
  }
  else
  {
    StringHelper::debug( 0xff, "GoodStore:Impossible to store goods RID=%d", reservationID );
  }
}

void GoodStore::retrieve(GoodStock &stock, int amount)
{
  GoodStock reservedStock;
  reservedStock.setType( stock.type() );
  reservedStock._currentQty = amount;

  long reservationID = reserveRetrieval(reservedStock);
  if( reservationID > 0 )
  {
    applyRetrieveReservation(stock, reservationID);
  }
  else
  {
    StringHelper::debug( 0xff, "GoodStore:Impossible to retrieve goods RID=%d", reservationID );
  }
}

void GoodStore::storeAll( GoodStore& goodStore)
{
  for (int n = 1; n<Good::goodCount; ++n)
  {
    // for all types of good (except G_NONE)
    Good::Type goodType = (Good::Type) n;
    GoodStock stock( goodType, 9999, 0 );
    goodStore.retrieve( stock, goodStore.getCurrentQty( goodType ) );
    if( !stock.empty() )
    {
      store(stock, stock._currentQty);
    }
  }
}

VariantMap GoodStore::save() const 
{
  VariantMap stream;

  stream[ "nextReservationId" ] = static_cast<int>(_d->nextReservationID);

  VariantList vm_storeReservations;
  foreach( _Reservations::value_type& item, _d->storeReservations )
  {
    vm_storeReservations.push_back( (int)item.first );
    vm_storeReservations.push_back( item.second.save() );
  }
  stream[ "storeReservations" ] = vm_storeReservations;
  stream[ "devastation" ] = _d->devastation;

  VariantList vm_retrieveReservations;
  foreach( _Reservations::value_type& item, _d->retrieveReservations)
  {
    vm_retrieveReservations.push_back( (int)item.first );
    vm_retrieveReservations.push_back( item.second.save() );
  }
  stream[ "retrieveReservation" ] = vm_retrieveReservations;

  return stream;
}

void GoodStore::load( const VariantMap& stream )
{
  _d->devastation = stream.get( "devastation" ).toBool();
  _d->nextReservationID = stream.get( "nextReservationId" ).toInt();

  VariantList vm_storeReservations = stream.get( "storeReservations" ).toList();
  for( VariantList::iterator it=vm_storeReservations.begin(); it != vm_storeReservations.end(); it++ )
  {
    int index = (*it).toInt(); it++;
    _d->storeReservations[ index ].load( (*it).toList() );
  }

  VariantList vm_retrieveReservations = stream.get( "retrieveReservation" ).toList();
  for( VariantList::iterator it=vm_retrieveReservations.begin(); it != vm_retrieveReservations.end(); it++ )
  {
    int index = (*it).toInt(); it++;
    _d->retrieveReservations[ index ].load( (*it).toList() );
  }
}

bool GoodStore::isDevastation() const
{
  return _d->devastation;
}

void GoodStore::setDevastation( bool value )
{
  _d->devastation = value;
}

GoodStore::~GoodStore()
{

}

void GoodStore::setOrder( const Good::Type type, const GoodOrders::Order order )
{
  _d->goodOrders.set( type, order );
}

GoodOrders::Order GoodStore::getOrder(const Good::Type type ) const
{
  return _d->goodOrders.get( type );
}

GoodStore::_Reservations& GoodStore::_getStoreReservations()
{
  return _d->storeReservations;
}

GoodStore::_Reservations& GoodStore::_getRetrieveReservations()
{
  return _d->retrieveReservations;
}

int GoodStore::getFreeQty( const Good::Type& goodType ) const
{
  return getMaxQty( goodType ) - getCurrentQty( goodType );
}

int GoodStore::getFreeQty() const
{
  return getMaxQty() - getCurrentQty();
}
