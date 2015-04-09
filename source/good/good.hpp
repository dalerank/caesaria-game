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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_GOOD_H_INCLUDE_
#define _CAESARIA_GOOD_H_INCLUDE_

#include "core/namedtype.hpp"
#include <list>
#include <map>

namespace good
{

BEGIN_NAMEDTYPE(Product,none)
APPEND_NAMEDTYPE_ID(wheat,1)
APPEND_NAMEDTYPE_ID(fish,2)
APPEND_NAMEDTYPE_ID(meat,3)
APPEND_NAMEDTYPE_ID(fruit,4 )
APPEND_NAMEDTYPE_ID(vegetable,5 )
APPEND_NAMEDTYPE_ID(olive,6 )
APPEND_NAMEDTYPE_ID(oil,7 )
APPEND_NAMEDTYPE_ID(grape,8 )
APPEND_NAMEDTYPE_ID(wine,9 )
APPEND_NAMEDTYPE_ID(timber,10 )
APPEND_NAMEDTYPE_ID(furniture,11 )
APPEND_NAMEDTYPE_ID(clay,12 )
APPEND_NAMEDTYPE_ID(pottery,13 )
APPEND_NAMEDTYPE_ID(iron,14 )
APPEND_NAMEDTYPE_ID(weapon,15 )
APPEND_NAMEDTYPE_ID(marble,16 )
APPEND_NAMEDTYPE_ID(denaries,17 )
APPEND_NAMEDTYPE_ID(prettyWine,18 )
END_NAMEDTYPE(Product)

typedef std::list<Product> Products;

const Product& any();
const Products& foods();
const Products& materials();
const Products& all();

class Stock;
class ProductMap;
class Store;
}//end namespace good
    
#endif //_CAESARIA_GOOD_H_INCLUDE_
