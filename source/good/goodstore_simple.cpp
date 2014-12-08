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

#include "goodstore_simple.hpp"
#include "core/math.hpp"
#include "core/foreach.hpp"
#include "core/variant.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"

class SmStock : public good::Stock, public ReferenceCounted
{
public:
  typedef SmartPtr<SmStock> Ptr;

  static Ptr create( good::Type type )
  {
    Ptr ret( new SmStock() );
    ret->setType( type );
    ret->drop();

    return ret;
  }
};


class SimpleGoodStore::Impl
{
public:
  typedef std::vector<SmStock::Ptr> StockList;
  StockList stocks;
  int capacity;

  void reset()
  {
    stocks.clear();
    stocks.reserve(good::goodCount);
    for( int n = 0; n < (int)good::goodCount; ++n)
    {
      stocks.push_back( SmStock::create( (good::Type)n ) );
    }
  }
};

SimpleGoodStore::SimpleGoodStore() : _gsd( new Impl )
{
  _gsd->capacity = 0;
  _gsd->reset();
}

void SimpleGoodStore::setCapacity(const int maxQty) {  _gsd->capacity = maxQty;}
int SimpleGoodStore::capacity() const {  return _gsd->capacity; }

int SimpleGoodStore::qty() const
{
  int qty = 0;
  foreach( goodIt, _gsd->stocks )
  {
    qty += (*goodIt)->qty();
  }

  return qty;
}

good::Stock& SimpleGoodStore::getStock(const good::Type &goodType){  return *(_gsd->stocks[goodType].object());}
int SimpleGoodStore::qty(const good::Type& goodType) const{  return _gsd->stocks[goodType]->qty();}
int SimpleGoodStore::capacity(const good::Type& goodType) const{  return _gsd->stocks[goodType]->capacity();}

void SimpleGoodStore::setCapacity(const good::Type& goodType, const int maxQty)
{
  if( goodType == good::goodCount )
  {
    for( int i=good::none; i < good::goodCount; i++ )
    {
      good::Type gtype = (good::Type)i;
      _gsd->stocks[gtype]->setCapacity( maxQty );
    }
  }
  else
  {
    _gsd->stocks[goodType]->setCapacity( maxQty );
  }
}

void SimpleGoodStore::setQty(const good::Type& goodType, const int currentQty){  _gsd->stocks[goodType]->setQty( currentQty );}

int SimpleGoodStore::getMaxStore(const good::Type goodType)
{
  int freeRoom = 0;
  if( !isDevastation() )
  {
    int globalFreeRoom = capacity() - qty();

    // current free capacity
    good::Stock& st = *_gsd->stocks[goodType].object();
    freeRoom = math::clamp( st.freeQty(), 0, globalFreeRoom );

    // remove all storage reservations
    foreach( i, _getStoreReservations() )
    {
      freeRoom -= i->stock.qty();
    }
  }

  return freeRoom;
}

void SimpleGoodStore::applyStorageReservation(good::Stock &stock, const int reservationID)
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
  _gsd->stocks[ reservedStock.type() ]->push( amount );
  stock.pop( amount );
}

void SimpleGoodStore::applyRetrieveReservation(good::Stock& stock, const int reservationID)
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

VariantMap SimpleGoodStore::save() const
{
  VariantMap stream = GoodStore::save();

  stream[ "max" ] = _gsd->capacity;

  VariantList stockSave;
  for( Impl::StockList::const_iterator itStock = _gsd->stocks.begin();
       itStock != _gsd->stocks.end(); ++itStock )
  {
    stockSave.push_back( (*itStock)->save() );
  }

  stream[ "stock" ] = stockSave;

  return stream;
}

void SimpleGoodStore::load( const VariantMap& stream )
{
  GoodStore::load( stream );
  _gsd->capacity = (int)stream.get( "max" );

  _gsd->reset();
  VariantList stockSave = stream.get( "stock" ).toList();
  foreach( it, stockSave )
  {
    SmStock::Ptr stock = SmStock::create( good::none );
    stock->load( it->toList() );
    _gsd->stocks[ stock->type() ] = stock;
  }
}

SimpleGoodStore::~SimpleGoodStore(){}

void SimpleGoodStore::resize( const GoodStore& other )
{
  setCapacity( other.capacity() );

  for( int i=good::wheat; i < good::goodCount; i++ )
  {
    good::Type gtype = good::Type( i );
    setCapacity( gtype, other.capacity( gtype ) );
  }
}
