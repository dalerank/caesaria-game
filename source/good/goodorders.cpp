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

#include "goodorders.hpp"
#include "core/foreach.hpp"
#include <map>

class GoodOrders::Impl
{
public:
  typedef std::map< good::Type, Order > Orders;
  Orders orders;
};

GoodOrders::~GoodOrders()
{

}

GoodOrders::GoodOrders() : _d( new Impl )
{

}

void GoodOrders::set( Order rule )
{
  foreach( item, _d->orders )
  {
    if( item->second != GoodOrders::none )
    {
      item->second = rule;
    }
  }
}

void GoodOrders::set(const good::Type type, Order rule )
{
  _d->orders[ type ] = rule;  
}

GoodOrders::Order GoodOrders::get( const good::Type type )
{
  Impl::Orders::iterator it = _d->orders.find( type );
  return it != _d->orders.end() ? (*it).second : GoodOrders::none;
}
