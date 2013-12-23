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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "good.hpp"

#include "core/gettext.hpp"
#include "core/exception.hpp"
#include "core/variant.hpp"
#include "core/stringhelper.hpp"

#include <iostream>
#include <algorithm>

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

GoodStock::~GoodStock()
{

}

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
    _CAESARIA_DEBUG_BREAK_IF( errorStr.c_str() );
    return;
  }

  int amount = iAmount;  // not const
  if (amount == -1)
  {
     amount = stock._qty;
  }
  if (amount > stock._qty)
  {
    _CAESARIA_DEBUG_BREAK_IF( "GoodStock:Not enough quantity in stock." );
    return;
  }

  amount = math::clamp( amount, 0, _capacity - _qty );
  if (amount+_qty > _capacity)
  {
    _CAESARIA_DEBUG_BREAK_IF( "GoodStock:Not enough free room for storage");
    return;
  }

  _type = stock._type;  // in case goodType was G_NONE
  _qty += amount;
  stock._qty -= amount;
  
  if (stock._qty == 0)
  {
     stock._type = none;
  }
}

VariantList GoodStock::save() const
{
  VariantList stream;
  stream.push_back( (int)_type );
  stream.push_back( _capacity );
  stream.push_back( _qty );

  return stream;
}

void GoodStock::load( const VariantList& stream )
{
  if( stream.empty())
    return;

  _type = (Good::Type)stream.get( 0 ).toInt();
  _capacity = (int)stream.get( 1 );
  _qty = math::clamp( (int)stream.get( 2 ), 0, _capacity );
}

bool GoodStock::empty() const
{
  return _qty == 0;
}

void GoodStock::setType(Type goodType )
{
  _type = goodType;
}

void GoodStock::setCapacity( const int maxQty )
{
  _capacity = maxQty;
}

int GoodStock::freeQty() const
{
  return std::max( _capacity - _qty, 0 );
}
