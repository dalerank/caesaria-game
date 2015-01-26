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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_GOOD_H_INCLUDE_
#define _CAESARIA_GOOD_H_INCLUDE_

#include <string>
#include <vector>
#include <map>

class VariantList;

namespace good
{

class Product
{
public:  
  explicit Product( int which=0) { _type = which; }
  int toInt() const { return _type; }
  bool operator==( const Product& a ) const { return _type == a._type; }
  bool operator!=( const Product& a ) const { return _type != a._type; }
  bool operator<( const Product& a ) const { return _type < a._type; }
  bool operator>( const Product& a ) const { return _type > a._type; }
  bool operator<=( const Product& a ) const { return _type <= a._type; }
  Product& operator++() { ++_type; return *this; }
  //int operator() const { return toInt(); }

protected:
  int _type;
};

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

class Stock : Product
{
public:
  Stock();
  Stock(const Product& which, const int maxQty, const int currentQty=0);
  ~Stock();

  void setType( Product goodType );
  const Product& type() const;

  void setCapacity( const int maxQty );
  int capacity() const { return _capacity; }

  void setQty( const int qty ) { _qty = qty; }
  int qty() const { return _qty; }

  int freeQty() const;

  void push( const int qty ) { _qty += qty; }
  void pop( const int qty );

  /** amount: if -1, amount=stock._currentQty */
  void append( Stock& stock, const int amount = -1);

  VariantList save() const;
  void load( const VariantList& options );

  bool empty() const;

protected:
  int _capacity;
  int _qty;
};

class Store;

}//end namespace good
    
#endif //_CAESARIA_GOOD_H_INCLUDE_
