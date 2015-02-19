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

DEFINE_NAMEDTYPE(Product)

const Product none( 0 );
const Product wheat( 1 );
const Product fish( 2 );
const Product meat( 3 );
const Product fruit( 4 );
const Product vegetable( 5 );
const Product olive( 6 );
const Product oil( 7 );
const Product grape( 8 );
const Product wine( 9 );
const Product timber( 10 );
const Product furniture( 11 );
const Product clay( 12 );
const Product pottery( 13 );
const Product iron( 14 );
const Product weapon( 15 );
const Product marble( 16 );
const Product denaries( 17 );
const Product prettyWine( 18 );
const Product goodCount( 19 );

class Stock;
class Store;

typedef std::list<Product> Products;

}//end namespace good
    
#endif //_CAESARIA_GOOD_H_INCLUDE_
