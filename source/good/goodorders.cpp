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

namespace good
{

class Orders::Impl
{
public:
  typedef std::map< good::Product, Order > Orders;
  Orders orders;
};

Orders::~Orders()
{

}

Orders::Orders() : _d( new Impl )
{

}

void Orders::set( Order rule )
{
  foreach( item, _d->orders )
  {
    if( item->second != Orders::none )
    {
      item->second = rule;
    }
  }
}

void Orders::set(const Product type, Order rule )
{
  _d->orders[ type ] = rule;  
}

Orders::Order Orders::get( const good::Product type )
{
  Impl::Orders::iterator it = _d->orders.find( type );
  return it != _d->orders.end() ? (*it).second : Orders::none;
}

}//end namespace good
