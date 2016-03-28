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

#include "store.hpp"
#include "storage.hpp"
#include "orders.hpp"
#include "core/utils.hpp"
#include "core/foreach.hpp"
#include "productmap.hpp"
#include "core/logger.hpp"
#include "core/variant_list.hpp"
#include "gfx/tilemap_config.hpp"
#include "core/variant_map.hpp"
#include "helper.hpp"

namespace good
{

enum { noId=0 };

class Store::Impl
{
public:  
  bool devastation;
  ConsumerDetails consumers;
  ProviderDetails providers;
  Reservations storeReservations;  // key=reservationID, value=stock
  Reservations retrieveReservations;  // key=reservationID, value=stock
  Orders goodOrders;
};

Store::Store() : __INIT_IMPL(Store)
{
  _dfunc()->devastation = false;
}

ConsumerDetails& Store::_consumers() { return _dfunc()->consumers; }
ProviderDetails& Store::_providers() { return _dfunc()->providers; }

int Store::getMaxRetrieve(const good::Product goodType)
{
  // current good quantity
  int rqty = qty(goodType);

  // remove all retrieval reservations
  for( const auto& reserve : _dfunc()->retrieveReservations )
    rqty -= reserve.qty();

  return rqty;
}


int Store::reserveStorage(good::Stock &stock, DateTime time)
{
  if( getMaxStore(stock.type() ) < stock.qty() )   // current free capacity
    return noId;

  // the stock can be stored!
  return _dfunc()->storeReservations.push( stock, time );
}

int Store::reserveStorage(good::Product what, unsigned int qty, DateTime time)
{
  good::Stock tmpStock( what, qty, qty);
  return reserveStorage( tmpStock, time );
}

int Store::reserveRetrieval(good::Stock &stock, DateTime time)
{
  // current good quantity
  if( getMaxRetrieve(stock.type()) < stock.qty())
    return noId;

  return _dfunc()->retrieveReservations.push( stock, time );
}

int Store::reserveRetrieval(good::Product what, unsigned int qty, DateTime time)
{
  good::Stock tmpStock( what, qty, qty);
  return reserveRetrieval( tmpStock, time );
}

good::Stock Store::getStorageReservation(const int reservationID, const bool pop)
{
  __D_REF(d,Store)
  ReserveInfo info = d.storeReservations.get( reservationID );

  if( info.id == noId )
  {
    Logger::warning( "Unknown stock for reservationID" );
    return good::Stock();
  }

  if( pop )
  {
    d.storeReservations.pop( reservationID );
  }

  return info.stock;
}


good::Stock Store::getRetrieveReservation(const int reservationID, const bool pop)
{
  __D_REF(d,Store)
  ReserveInfo info = d.retrieveReservations.get(reservationID);

  if( info.id == noId )
  {
    Logger::warning( "GoodStore::getRetrieveReservation unknown reservationID");
    return good::Stock();
  }

  if( pop )
  {
    d.retrieveReservations.pop( reservationID );
  }

  return info.stock;
}

void Store::confirmDeliver(Product gtype, int qty, unsigned int tag, const DateTime& time)
{
  _dfunc()->consumers.append( gtype, qty, tag, time );
}

void Store::applyStorageReservation(Storage &goodStore, const int reservationID)
{
  good::Stock reservedStock = getStorageReservation(reservationID);
  good::Stock &stock = goodStore.getStock(reservedStock.type() );

  applyStorageReservation(stock, reservationID);
}


void Store::applyRetrieveReservation(Storage &goodStore, const int reservationID)
{
  good::Stock reservedStock = getRetrieveReservation(reservationID);
  good::Stock &stock = goodStore.getStock(reservedStock.type() );

  applyRetrieveReservation(stock, reservationID);
}

void Store::store( good::Stock& stock, const int amount)
{
  good::Stock reservedStock;
  reservedStock.setType( stock.type() );
  reservedStock.setCapacity( stock.capacity() );
  reservedStock.setQty( amount );

  long reservationID = reserveStorage( reservedStock, DateTime() );

  if( reservationID > noId )
  {
    applyStorageReservation(stock, reservationID);
  }
  else
  {
    Logger::warning( "GoodStore: store impossible to store {} of {}",
                     stock.qty(),
                     good::Helper::name( stock.type() ) );
  }
}

void Store::retrieve(good::Stock &stock, int amount)
{
  good::Stock reservedStock;
  reservedStock.setType( stock.type() );
  reservedStock.setQty( amount );

  long reservationID = reserveRetrieval( reservedStock, DateTime() );
  if( reservationID > noId )
  {
    applyRetrieveReservation(stock, reservationID);
  }
  else
  {
    Logger::warning( "GoodStore:Impossible to retrieve {} of {}",
                     good::Helper::name( stock.type() ),
                     stock.qty() );
  }
}

TilePos Store::owner() const { return TilePos::invalid(); }

void Store::storeAll( Store& goodStore )
{
  for( auto& goodType : good::all() )
  {
    // for all types of good (except G_NONE)
    good::Stock stock( goodType, 9999, 0 );
    goodStore.retrieve( stock, goodStore.qty( goodType ) );
    if( !stock.empty() )
    {
      store(stock, stock.qty());
    }
  }
}

VariantMap Store::save() const
{
  __D_REF(d,Store)
  VariantMap stream;

  VARIANT_SAVE_CLASS_D( stream, &d, storeReservations )
  VARIANT_SAVE_ANY_D( stream, &d, devastation )
  VARIANT_SAVE_CLASS_D( stream, &d, retrieveReservations )
  VARIANT_SAVE_CLASS_D( stream, &d, consumers )
  VARIANT_SAVE_CLASS_D( stream, &d, providers )

  VariantList vm_orders;
  for( auto& goodType : good::all() )
    vm_orders.push_back( (int)getOrder( goodType ) );

  stream[ "orders" ] = vm_orders;

  return stream;
}

void Store::load( const VariantMap& stream )
{
  __D_REF(d,Store)
  if( stream.empty() )
  {
    Logger::warning( "!!! cant load store config from empty stream" );
    return;
  }

  VARIANT_LOAD_ANY_D( &d, devastation, stream )
  VARIANT_LOAD_CLASS_D( &d, storeReservations, stream )
  VARIANT_LOAD_CLASS_D( &d, retrieveReservations, stream )
  VARIANT_LOAD_CLASS_D( &d, consumers, stream )
  VARIANT_LOAD_CLASS_D( &d, providers, stream )

  VariantList vm_orders = stream.get( "orders" ).toList();
  int index = 0;
  for( auto& var : vm_orders )
  {
    setOrder( (good::Product)index, (Orders::Order)var.toInt() );
    index++;
  }
}

const ConsumerDetails& Store::consumers() const
{
  _dfunc()->consumers.owner = owner();
  return _dfunc()->consumers;
}

const ProviderDetails& Store::providers() const
{
  _dfunc()->providers.owner = owner();
  return _dfunc()->providers;
}

bool Store::isDevastation() const{  return _dfunc()->devastation;}
void Store::setDevastation( bool value ){  _dfunc()->devastation = value;}

Store::~Store() {}

ProductMap Store::details() const
{
  ProductMap ret;
  for( const auto& goodType : good::all() )
  {
    int q = qty( goodType );
    if( q > 0 )
      ret[ goodType ] = q;
  }

  return ret;
}

ProductMap Store::amounts() const
{
  ProductMap ret;
  for( const auto& goodType : good::all() )
  {
    int cap = capacity( goodType );
    if( cap > 0 )
      ret[ goodType ] = cap;
  }

  return ret;
}

void Store::setOrder( const good::Product type, const Orders::Order order ){  _dfunc()->goodOrders.set( type, order );}
Orders::Order Store::getOrder(const good::Product type) const{  return _dfunc()->goodOrders.get( type );}

void Store::removeExpired(DateTime date)
{
  _dfunc()->retrieveReservations.removeExpired( date, 2 );
  _dfunc()->storeReservations.removeExpired( date, 2 );
}

Reservations& Store::_getStoreReservations() {  return _dfunc()->storeReservations; }
Reservations& Store::_getRetrieveReservations(){   return _dfunc()->retrieveReservations;}
int Store::freeQty( const good::Product& goodType ) const{ return capacity( goodType ) - qty( goodType );}
int Store::freeQty() const{  return capacity() - qty();}
bool Store::empty() const{  return qty() == 0;}
const ReserveInfo Reservations::invalid = { good::Stock(), DateTime(), 0 };
Reservations::Reservations(){  _idCounter = 1; }

const ReserveInfo& Reservations::get(unsigned int id) const
{
  for( const auto& info : *this )
  {
    if( info.id == id )
      return info;
  }

  return Reservations::invalid;
}

unsigned int Reservations::push(const good::Stock& stock, DateTime time)
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
  foreach( i, *this )
  {
    if( i->id == id )
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
    if( date.monthsTo( currentDate ) > monthNumber ) { erase( i++ ); }
    else { ++i; }
  }
}

VariantMap Reservations::save() const
{
  VariantMap stream;

  stream[ "idCounter" ] = static_cast<int>(_idCounter);
  VariantList vm_reservations;
  for( const auto& item : *this )
  {
    vm_reservations.push_back( (int)item.id );
    vm_reservations.push_back( item.stock.save() );
    vm_reservations.push_back( item.time );
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

}//end namespace good
