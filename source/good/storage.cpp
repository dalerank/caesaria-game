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

#include "storage.hpp"
#include "core/math.hpp"
#include "core/foreach.hpp"
#include "core/variant_map.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"

namespace good
{

class SmStock : public good::Stock, public ReferenceCounted
{
public:
  typedef SmartPtr<SmStock> Ptr;

  static Ptr create( good::Product type )
  {
    Ptr ret( new SmStock() );
    ret->setType( type );
    ret->drop();

    return ret;
  }
};


class Storage::Impl
{
public:
  typedef std::vector<SmStock::Ptr> StockList;
  StockList stocks;
  int capacity;

  void reset()
  {
    stocks.clear();
    stocks.reserve(good::goodCount.toInt());
    for( good::Product n = good::none; n < good::goodCount; ++n)
    {
      stocks.push_back( SmStock::create( n ) );
    }
  }
};

Storage::Storage() : _gsd( new Impl )
{
  _gsd->capacity = 0;
  _gsd->reset();
}

void Storage::setCapacity(const int maxQty) {  _gsd->capacity = maxQty;}
int Storage::capacity() const {  return _gsd->capacity; }

int Storage::qty() const
{
  int qty = 0;
  foreach( goodIt, _gsd->stocks )
  {
    qty += (*goodIt)->qty();
  }

  return qty;
}

good::Stock& Storage::getStock(const Product& goodType){  return *(_gsd->stocks[goodType.toInt()].object());}
int Storage::qty(const good::Product& goodType) const{  return _gsd->stocks[goodType.toInt()]->qty();}
int Storage::capacity(const good::Product& goodType) const{  return _gsd->stocks[goodType.toInt()]->capacity();}

void Storage::setCapacity(const good::Product& goodType, const int maxQty)
{
  if( goodType == good::goodCount )
  {
    for( good::Product gtype=good::none; gtype < good::goodCount; ++gtype )
    {
      _gsd->stocks[gtype.toInt()]->setCapacity( maxQty );
    }
  }
  else
  {
    _gsd->stocks[goodType.toInt()]->setCapacity( maxQty );
  }
}

void Storage::setQty(const good::Product& goodType, const int currentQty){  _gsd->stocks[goodType.toInt()]->setQty( currentQty );}

int Storage::getMaxStore(const good::Product goodType)
{
  int freeRoom = 0;
  if( !isDevastation() )
  {
    int globalFreeRoom = capacity() - qty();

    // current free capacity
    good::Stock& st = *_gsd->stocks[goodType.toInt()].object();
    freeRoom = math::clamp( st.freeQty(), 0, globalFreeRoom );

    // remove all storage reservations
    foreach( i, _getStoreReservations() )
    {
      freeRoom -= i->stock.qty();
    }
  }

  return freeRoom;
}

void Storage::applyStorageReservation(good::Stock &stock, const int reservationID)
{
  good::Stock reservedStock = getStorageReservation(reservationID, true);

  if (stock.type() != reservedStock.type())
  {
    Logger::warning( "SimpleGoodStore:GoodType does not match reservation");
    return;
  }

  if (stock.qty() < reservedStock.qty())
  {
    Logger::warning( "SimpleGoodStore:Quantity does not match reservation");
    return;
  }

  int amount = reservedStock.qty();
  _gsd->stocks[ reservedStock.type().toInt() ]->push( amount );
  stock.pop( amount );
}

void Storage::applyRetrieveReservation(good::Stock& stock, const int reservationID)
{
  good::Stock reservedStock = getRetrieveReservation(reservationID, true);

  if (stock.type() != reservedStock.type())
  {
    Logger::warning( "SimpleGoodStore:GoodType does not match reservation");
    return;
  }

  if( stock.capacity() < stock.qty() + reservedStock.qty())
  {
    Logger::warning( "SimpleGoodStore:Quantity does not match reservation");
    return;
  }

  int amount = reservedStock.qty();
  good::Stock &currentStock = getStock(reservedStock.type());
  currentStock.pop( amount );
  stock.push( amount );
}

VariantMap Storage::save() const
{
  VariantMap stream = good::Store::save();

  stream[ "max" ] = _gsd->capacity;

  VariantList stockSave;
  foreach( it, _gsd->stocks )
  {
    stockSave.push_back( (*it)->save() );
  }

  stream[ "stock" ] = stockSave;

  return stream;
}

void Storage::load( const VariantMap& stream )
{
  good::Store::load( stream );
  _gsd->capacity = (int)stream.get( "max" );

  _gsd->reset();
  VariantList stockSave = stream.get( "stock" ).toList();
  foreach( it, stockSave )
  {
    SmStock::Ptr stock = SmStock::create( good::none );
    stock->load( it->toList() );
    _gsd->stocks[ stock->type().toInt() ] = stock;
  }
}

Storage::~Storage(){}

void Storage::resize(const Store &other )
{
  setCapacity( other.capacity() );

  for( good::Product gtype=good::wheat; gtype < good::goodCount; ++gtype )
  {
    setCapacity( gtype, other.capacity( gtype ) );
  }
}

}//end namespace good
