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


#ifndef _OPENCAESAR_GOOD_H_INCLUDE_
#define _OPENCAESAR_GOOD_H_INCLUDE_

#include "enums.hpp"
#include <string>
#include <vector>
#include <map>

class VariantList;

class Good
{
  friend class GoodHelper;
public:
  typedef enum
  {
    none,
    wheat, fish, meat, fruit, vegetable,
    olive, oil,
    grape, wine,
    timber, furniture,
    clay, pottery,
    iron, weapon,
    marble,
    denaries,
    goodCount
  } Type;

  virtual Type type() const { return _type; }

protected:
  Type _type;
};

class GoodStock : public Good
{
public:
  GoodStock();
  GoodStock(const Good::Type &goodType, const int maxQty, const int currentQty=0);

  void setType( const Good::Type &goodType );
  void setMax( const int maxQty );

  /** amount: if -1, amount=stock._currentQty */
  void append(GoodStock &stock, const int amount = -1);

  VariantList save() const;
  void load( const VariantList& options );

  bool empty() const;

  int _maxQty;
  int _currentQty;
};
    
#endif //_OPENCAESAR_GOOD_H_INCLUDE_
