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

#include "oc3_goodstore_simple.hpp"
#include "oc3_math.hpp"
#include "oc3_foreach.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"

class SimpleGoodStore::Impl
{
public:
};

SimpleGoodStore::SimpleGoodStore()
{
  _maxQty = 0;

  _goodStockList.resize(Good::goodCount);
  for (int n = 0; n < (int)Good::goodCount; ++n)
  {
    _goodStockList[n].setType( (Good::Type)n );
  }
}


void SimpleGoodStore::setMaxQty(const int maxQty)
{
  _maxQty = maxQty;
}


int SimpleGoodStore::getMaxQty() const
{
  return _maxQty;
}


int SimpleGoodStore::getCurrentQty() const
{
  int qty = 0;
  for( std::vector<GoodStock>::const_iterator goodIt = _goodStockList.begin(); goodIt != _goodStockList.end(); ++goodIt)
  {
    qty += (*goodIt)._currentQty;
  }

  return qty;
}


GoodStock& SimpleGoodStore::getStock(const Good::Type &goodType)
{
  return _goodStockList[goodType];
}


int SimpleGoodStore::getCurrentQty(const Good::Type &goodType) const
{
  return _goodStockList[goodType]._currentQty;
}


int SimpleGoodStore::getMaxQty(const Good::Type &goodType) const
{
  return _goodStockList[goodType]._maxQty;
}


void SimpleGoodStore::setMaxQty(const Good::Type &goodType, const int maxQty)
{
  _goodStockList[goodType]._maxQty = maxQty;
}


void SimpleGoodStore::setCurrentQty(const Good::Type &goodType, const int currentQty)
{
  _goodStockList[goodType]._currentQty = currentQty;
}

int SimpleGoodStore::getMaxStore(const Good::Type goodType)
{
  int freeRoom = 0;
  if( !isDevastation() )
  {
    int globalFreeRoom = getMaxQty() - getCurrentQty();

    // current free capacity
    freeRoom = math::clamp( _goodStockList[goodType]._maxQty - _goodStockList[goodType]._currentQty, 0, globalFreeRoom );

    // remove all storage reservations
    foreach( _Reservations::value_type& item, _getStoreReservations() )
    {
      freeRoom -= item.second._currentQty;
    }
  }

  return freeRoom;
}

void SimpleGoodStore::applyStorageReservation(GoodStock &stock, const long reservationID)
{
  GoodStock reservedStock = getStorageReservation(reservationID, true);

  if (stock.type() != reservedStock.type())
  {
    StringHelper::debug( 0xff, "SimpleGoodStore:GoodType does not match reservation");
    return;
  }

  if (stock._currentQty < reservedStock._currentQty)
  {
    StringHelper::debug( 0xff, "SimpleGoodStore:Quantity does not match reservation");
    return;
  }


  int amount = reservedStock._currentQty;
  GoodStock &currentStock = _goodStockList[reservedStock.type()];
  currentStock._currentQty += amount;
  stock._currentQty -= amount;
}


void SimpleGoodStore::applyRetrieveReservation(GoodStock &stock, const long reservationID)
{
  GoodStock reservedStock = getRetrieveReservation(reservationID, true);

  if (stock.type() != reservedStock.type())
  {
    StringHelper::debug( 0xff, "SimpleGoodStore:GoodType does not match reservation");
    return;
  }

  if( stock._maxQty < stock._currentQty + reservedStock._currentQty)
  {
    StringHelper::debug( 0xff, "SimpleGoodStore:Quantity does not match reservation");
    return;
  }

  int amount = reservedStock._currentQty;
  GoodStock &currentStock = getStock(reservedStock.type());
  currentStock._currentQty -= amount;
  stock._currentQty += amount;
  // std::cout << "SimpleGoodStore, retrieve qty=" << amount << " resID=" << reservationID << std::endl;
}


VariantMap SimpleGoodStore::save() const
{
  VariantMap stream = GoodStore::save();

  stream[ "max" ] = _maxQty;

  VariantList stockSave;
  for( StockList::const_iterator itStock = _goodStockList.begin();
       itStock != _goodStockList.end(); itStock++)
  {
    stockSave.push_back( (*itStock).save() );
  }

  stream[ "stock" ] = stockSave;

  return stream;
}

void SimpleGoodStore::load( const VariantMap& stream )
{
  _goodStockList.clear();

  GoodStore::load( stream );
  _maxQty = (int)stream.get( "max" );

  VariantList stockSave = stream.get( "stock" ).toList();
  for( VariantList::iterator it=stockSave.begin(); it!=stockSave.end(); it++ )
  {
    GoodStock restored;
    restored.load( (*it).toList() );
    _goodStockList.push_back( restored );
  }
}

SimpleGoodStore::~SimpleGoodStore()
{

}

void SimpleGoodStore::resize( const GoodStore& other )
{
  setMaxQty( other.getMaxQty() );

  for( int i=Good::wheat; i < Good::goodCount; i++ )
  {
    Good::Type gtype = Good::Type( i );
    setMaxQty( gtype, other.getMaxQty( gtype ) );
  }
}
