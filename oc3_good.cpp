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



#include "oc3_good.hpp"

#include "oc3_gettext.hpp"
#include "oc3_exception.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"

#include <iostream>
#include <algorithm>

GoodStock::GoodStock()
{
   _type = G_NONE;
   _maxQty = 0;
   _currentQty = 0;
}

GoodStock::GoodStock(const Good::Type &goodType, const int maxQty, const int currentQty)
{
   _type = goodType;
   _maxQty = maxQty;
   _currentQty = currentQty;
}

void GoodStock::append(GoodStock &stock, const int iAmount)
{
  if (stock._type == G_NONE)
  {
     // nothing to add => nothing to do!
     return;
  }
  if (_type != G_NONE && _type != stock._type)
  {
    std::string errorStr = StringHelper::format( 0xff, "GoodTypes do not match: %d vs %d", _type, stock._type );
    _OC3_DEBUG_BREAK_IF( errorStr.c_str() );
    return;
  }

  int amount = iAmount;  // not const
  if (amount == -1)
  {
     amount = stock._currentQty;
  }
  if (amount > stock._currentQty)
  {
    _OC3_DEBUG_BREAK_IF( "GoodStock:Not enough quantity in stock." );
    return;
  }

  amount = math::clamp( amount, 0, _maxQty - _currentQty );
  if (amount+_currentQty > _maxQty)
  {
    _OC3_DEBUG_BREAK_IF( "GoodStock:Not enough free room for storage");
    return;
  }

  _type = stock._type;  // in case goodType was G_NONE
  _currentQty += amount;
  stock._currentQty -= amount;
  
  if (stock._currentQty == 0)
  {
     stock._type = G_NONE;
  }
}

VariantList GoodStock::save() const
{
  VariantList stream;
  stream.push_back( (int)_type );
  stream.push_back( _maxQty );
  stream.push_back( _currentQty );

  return stream;
}

void GoodStock::load( const VariantList& stream )
{
  if( stream.size() < 3 )
    return;

  VariantList::const_iterator it=stream.begin();
  _type = (Good::Type)(*it).toInt(); it++;
  _maxQty = (*it).toInt(); it++;
  _currentQty = (*it).toInt();
}

bool GoodStock::empty() const
{
  return _currentQty == 0;
}

void GoodStock::setType(const Type &goodType )
{
  _type = goodType;
}

void GoodStock::setMax( const int maxQty )
{
  _maxQty = maxQty;
}
