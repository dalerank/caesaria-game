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


#ifndef GOOD_HPP
#define GOOD_HPP

#include "oc3_enums.hpp"
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
    G_NONE,
    G_WHEAT,
    G_FISH,
    G_MEAT,
    G_FRUIT, G_VEGETABLE, G_OLIVE, G_OIL, G_GRAPE, G_WINE,
    G_TIMBER, G_FURNITURE, G_CLAY, G_POTTERY, G_IRON, G_WEAPON, G_MARBLE,
    G_DENARIES,
    G_MAX
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
    
#endif
