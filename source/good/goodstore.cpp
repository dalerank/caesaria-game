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

#include "goodstore.hpp"
#include "goodstore_simple.hpp"
#include "goodorders.hpp"
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "goodhelper.hpp"

class GoodStore::Impl
{
public:  
  bool devastation;

  Reservations storeReservations;  // key=reservationID, value=stock
  Reservations retrieveReservations;  // key=reservationID, value=stock
  GoodOrders goodOrders;
};

GoodStore::GoodStore() : _d( new Impl )
{
  _d->devastation = false;
}


int GoodStore::getMaxRetrieve(const Good::Type goodType)
{
  // current good quantity
  int rqty = qty(goodType);

  // remove all retrieval reservations
  foreach( i, _d->retrieveReservations)
  {
    rqty -= i->stock.qty();
  }

  return rqty;
}


long GoodStore::reserveStorage(GoodStock &stock, DateTime time)
{
  if( getMaxStore(stock.type() ) < stock.qty() )   // current free capacity
    return 0;

  // the stock can be stored!
  return _d->storeReservations.push( stock, time );
}

long GoodStore::reserveStorage(Good::Type what, unsigned int qty, DateTime time)
{
  GoodStock tmpStock( what, qty, qty);
  return reserveStorage( tmpStock, time );
}

long GoodStore::reserveRetrieval(GoodStock &stock, DateTime time)
{
  // current good quantity
  if( getMaxRetrieve(stock.type()) < stock.qty())
    return 0;

  return _d->retrieveReservations.push( stock, time );
}

long GoodStore::reserveRetrieval(Good::Type what, unsigned int qty, DateTime time)
{
  GoodStock tmpStock( what, qty, qty);
  return reserveRetrieval( tmpStock, time );
}

GoodStock GoodStore::getStorageReservation(const long reservationID, const bool pop)
{
  ReserveInfo info = _d->storeReservations.get( reservationID );

  if( info.id == 0 )
  {
    Logger::warning( "Unknown stock for reservationID" );
    return GoodStock();
  }

  if( pop )
  {
    _d->storeReservations.pop( reservationID );
  }

  return info.stock;
}


GoodStock GoodStore::getRetrieveReservation(const long reservationID, const bool pop)
{
  ReserveInfo info = _d->retrieveReservations.get(reservationID);

  if( info.id == 0 )
  {
    Logger::warning( "GoodStore::getRetrieveReservation unknown reservationID");
    return GoodStock();
  }

  if( pop )
  {
    _d->retrieveReservations.pop( reservationID );
  }

  return info.stock;
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
  reservedStock.setCapacity( stock.capacity() );
  reservedStock.setQty( amount );

  long reservationID = reserveStorage( reservedStock, DateTime() );

  if( reservationID > 0 )
  {
    applyStorageReservation(stock, reservationID);
  }
  else
  {
    Logger::warning( "GoodStore: store impossible to store %d of %s",
                     stock.qty(),
                     GoodHelper::getName( stock.type() ).c_str() );
  }
}

void GoodStore::retrieve(GoodStock &stock, int amount)
{
  GoodStock reservedStock;
  reservedStock.setType( stock.type() );
  reservedStock.setQty( amount );

  long reservationID = reserveRetrieval( reservedStock, DateTime() );
  if( reservationID > 0 )
  {
    applyRetrieveReservation(stock, reservationID);
  }
  else
  {
    Logger::warning( "GoodStore:Impossible to retrieve %d of %s",
                     GoodHelper::getName( stock.type() ).c_str(),
                     stock.qty() );
  }
}

void GoodStore::storeAll( GoodStore& goodStore)
{
  for (int n = 1; n<Good::goodCount; ++n)
  {
    // for all types of good (except G_NONE)
    Good::Type goodType = (Good::Type) n;
    GoodStock stock( goodType, 9999, 0 );
    goodStore.retrieve( stock, goodStore.qty( goodType ) );
    if( !stock.empty() )
    {
      store(stock, stock.qty());
    }
  }
}

VariantMap GoodStore::save() const 
{
  VariantMap stream;

  stream[ "storeReservations" ] = _d->storeReservations.save();
  stream[ "devastation" ] = _d->devastation;
  stream[ "retrieveReservation" ] = _d->retrieveReservations.save();

  VariantList vm_orders;
  for( int i=Good::none; i < Good::goodCount; i++ )
  {
    vm_orders.push_back( (int)getOrder( (Good::Type)i ) );
  }
  stream[ "orders" ] = vm_orders;

  return stream;
}

void GoodStore::load( const VariantMap& stream )
{
  _d->devastation = (bool)stream.get( "devastation" );
  _d->storeReservations.load( stream.get( "storeReservations" ).toMap() );
  _d->retrieveReservations.load( stream.get( "retrieveReservation" ).toMap() );

  VariantList vm_orders = stream.get( "orders" ).toList();
  int index = 0;
  foreach( var, vm_orders )
  {
    setOrder( (Good::Type)index, (GoodOrders::Order)var->toInt() );
    index++;
  }
}

bool GoodStore::isDevastation() const{  return _d->devastation;}
void GoodStore::setDevastation( bool value ){  _d->devastation = value;}

GoodStore::~GoodStore() {}
void GoodStore::setOrder( const Good::Type type, const GoodOrders::Order order ){  _d->goodOrders.set( type, order );}
GoodOrders::Order GoodStore::getOrder(const Good::Type type ) const{  return _d->goodOrders.get( type );}

void GoodStore::removeExpired(DateTime date)
{
  _d->retrieveReservations.removeExpired( date, 2 );
  _d->storeReservations.removeExpired( date, 2 );
}

Reservations& GoodStore::_getStoreReservations() {  return _d->storeReservations; }
Reservations& GoodStore::_getRetrieveReservations(){   return _d->retrieveReservations;}
int GoodStore::freeQty( const Good::Type& goodType ) const{ return capacity( goodType ) - qty( goodType );}
int GoodStore::freeQty() const{  return capacity() - qty();}
bool GoodStore::empty() const{  return qty() == 0;}
const ReserveInfo Reservations::invalid = { GoodStock(), DateTime(), 0 };
Reservations::Reservations(){  _idCounter = 1; }

const ReserveInfo& Reservations::get(unsigned long id) const
{
  for( Reservations::const_iterator i=begin(); i != end(); ++i )
  {
    if( (*i).id == id )
      return *i;
  }

  return Reservations::invalid;
}

unsigned long Reservations::push(const GoodStock& stock, DateTime time)
{
  ReserveInfo info;
  info.stock = stock;
  info.time = time;
  info.id = _idCounter;

  _idCounter++;

  insert( info );

  return info.id;
}

bool Reservations::pop(unsigned int id)
{
  for( Reservations::iterator i=begin(); i != end(); ++i )
  {
    if( (*i).id == id )
    {
      erase( i );
      return true;
    }
  }

  return false;
}

void Reservations::removeExpired(DateTime currentDate, int monthNumber)
{
  for( iterator i=begin(); i != end(); )
  {
    DateTime date = i->time;
    if( date.getMonthToDate( currentDate ) > monthNumber ) { erase( i++ ); }
    else { ++i; }
  }
}

VariantMap Reservations::save() const
{
  VariantMap stream;

  stream[ "idCounter" ] = static_cast<int>(_idCounter);
  VariantList vm_reservations;
  for( const_iterator i=begin(); i != end(); ++i )
  {
    vm_reservations.push_back( (int)(*i).id );
    vm_reservations.push_back( (*i).stock.save() );
    vm_reservations.push_back( (*i).time );
  }
  stream[ "items" ] = vm_reservations;

  return stream;
}

void Reservations::load(const VariantMap& stream)
{
  _idCounter = (int)stream.get( "idCounter" );

  VariantList vm_reservations = stream.get( "items" ).toList();
  for( unsigned int i=0; i < vm_reservations.size(); i+=3 )
  {
    ReserveInfo info;
    info.id = vm_reservations.get( i ).toInt();
    info.stock.load( vm_reservations.get( i+1 ).toList() );
    info.time = vm_reservations.get( i+2 ).toDateTime();

    insert( info );
  }
}
