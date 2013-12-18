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

#ifndef _CAESARIA_CITYTRADEOPTIONS_INCLUDE_H_
#define _CAESARIA_CITYTRADEOPTIONS_INCLUDE_H_

#include "core/scopedptr.hpp"
#include "game/good.hpp"
#include "core/variant.hpp"

class GoodStore;

class CityTradeOptions
{
public:
  typedef enum { importing=0, noTrade, exporting, stacking, disabled } Order;
  CityTradeOptions();
  ~CityTradeOptions();

  int getExportLimit( Good::Type type ) const;
  void setExportLimit( Good::Type type, int qty );
  
  void setStackMode( Good::Type type, bool stackGoods );
  bool isGoodsStacking( Good::Type type );

  unsigned int getSellPrice( Good::Type type ) const;
  void setSellPrice( Good::Type type, unsigned int price );

  bool isVendor( Good::Type type ) const;
  void setVendor( Good::Type type, bool available );

  unsigned int getBuyPrice( Good::Type type ) const;
  void setBuyPrice( Good::Type type, unsigned int price );

  Order getOrder( Good::Type type ) const;
  void setOrder( Good::Type type, Order order );
  Order switchOrder( Good::Type type );

  VariantMap save() const;
  void load( const VariantMap& stream );

  const GoodStore& getBuys();
  const GoodStore& getSells();

public:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_CITYTRADEOPTIONS_INCLUDE_H_
