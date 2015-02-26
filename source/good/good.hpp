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

namespace good
{

#define REGISTER_PRODUCT(name,index) static const Product name = Product(index);

DEFINE_NAMEDTYPE(Product,none)

REGISTER_PRODUCT(wheat,1)
REGISTER_PRODUCT(fish,2)
REGISTER_PRODUCT(meat,3)
REGISTER_PRODUCT(fruit,4 )
REGISTER_PRODUCT(vegetable,5 )
REGISTER_PRODUCT(olive,6 )
REGISTER_PRODUCT(oil,7 )
REGISTER_PRODUCT(grape,8 )
REGISTER_PRODUCT(wine,9 )
REGISTER_PRODUCT(timber,10 )
REGISTER_PRODUCT(furniture,11 )
REGISTER_PRODUCT(clay,12 )
REGISTER_PRODUCT(pottery,13 )
REGISTER_PRODUCT(iron,14 )
REGISTER_PRODUCT(weapon,15 )
REGISTER_PRODUCT(marble,16 )
REGISTER_PRODUCT(denaries,17 )
REGISTER_PRODUCT(prettyWine,18 )

class Stock;
class Store;

Product operator++(const Product& a) { return Product(a+1); }

typedef std::list<Product> Products;

static const Products foods;
static const Products materials;
static const Products all;

}//end namespace good
    
#endif //_CAESARIA_GOOD_H_INCLUDE_
