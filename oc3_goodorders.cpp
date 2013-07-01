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

#include "oc3_goodorders.hpp"
#include <map>

class GoodOrders::Impl
{
public:
  typedef std::map< GoodType, Order > Orders;
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
  for( Impl::Orders::iterator it=_d->orders.begin(); it != _d->orders.end(); it++ )
  {
    if( it->second != GoodOrders::none )
    {
      it->second = rule;
    }
  }
}

void GoodOrders::set( const GoodType type, Order rule )
{
  _d->orders[ type ] = rule;  
}

GoodOrders::Order GoodOrders::get( const GoodType type )
{
  Impl::Orders::iterator it = _d->orders.find( type );
  return it != _d->orders.end() ? (*it).second : GoodOrders::none;
}