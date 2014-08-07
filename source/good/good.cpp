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

#include "good.hpp"

#include "core/gettext.hpp"
#include "core/exception.hpp"
#include "core/variant.hpp"
#include "core/stringhelper.hpp"

#include <iostream>
#include <algorithm>
#include "core/logger.hpp"

GoodStock::GoodStock()
{
   _type = none;
   _capacity = 0;
   _qty = 0;
}

GoodStock::GoodStock(const Good::Type &goodType, const int maxQty, const int currentQty)
{
   _type = goodType;
   _capacity = maxQty;
   _qty = currentQty;
}

GoodStock::~GoodStock() {}

void GoodStock::append(GoodStock &stock, const int iAmount)
{
  if (stock._type == none)
  {
     // nothing to add => nothing to do!
     return;
  }
  if (_type != none && _type != stock._type)
  {
    std::string errorStr = StringHelper::format( 0xff, "GoodTypes do not match: %d vs %d", _type, stock._type );
    Logger::warning( errorStr );
    return;
  }

  int amount = iAmount;  // not const
  if (amount == -1)
  {
     amount = stock._qty;
  }
  if (amount > stock._qty)
  {
    Logger::warning( "GoodStock:Not enough quantity in stock." );
    return;
  }

  amount = math::clamp( amount, 0, _capacity - _qty );
  if (amount+_qty > _capacity)
  {
    Logger::warning( "GoodStock: not enough free room for storage");
    return;
  }

  _type = stock._type;  // in case goodType was Good::none
  _qty += amount;
  stock._qty -= amount;
  
  if( stock._qty == 0 )
  {
     stock._type = none;
  }
}

VariantList GoodStock::save() const
{
  VariantList stream;
  stream << (int)_type
         << _capacity
         << _qty;

  return stream;
}

void GoodStock::load( const VariantList& stream )
{
  if( stream.empty())
    return;

  _type = (Good::Type)stream.get( 0 ).toInt();
  if( _type >= Good::goodCount )
  {
    Logger::warning( "GoodStock: wrong type of good %d", _type );
    _type = Good::none;
  }

  _capacity = (int)stream.get( 1 );
  _qty = math::clamp( (int)stream.get( 2 ), 0, _capacity );
}

bool GoodStock::empty() const {  return _qty == 0; }
void GoodStock::setType(Type goodType ) {  _type = goodType;}
void GoodStock::setCapacity( const int maxQty ){  _capacity = maxQty;}
int GoodStock::freeQty() const{  return std::max( _capacity - _qty, 0 );}

void GoodStock::pop(const int qty)
{
  _qty -= math::clamp( qty, 0, _qty );
  if( _qty == 0 )
  {
    _type = Good::none;
  }
}
