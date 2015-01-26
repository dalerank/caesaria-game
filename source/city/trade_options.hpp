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
#include "good/good.hpp"
#include "core/variant.hpp"

namespace city
{

namespace trade
{

typedef enum { importing=0, noTrade, exporting, stacking, disabled } Order;

class Options
{
public:
  Options();
  ~Options();

  unsigned int tradeLimit( Order state, good::Product type) const;
  void setTradeLimit( Order state, good::Product type, unsigned int qty);
  
  void setStackMode( good::Product type, bool stacking );
  bool isGoodsStacking( good::Product type );

  bool isExporting( good::Product type ) const;
  bool isImporting( good::Product type ) const;

  unsigned int sellPrice( good::Product type ) const;
  void setSellPrice( good::Product type, unsigned int price );

  bool isVendor( good::Product type ) const;
  void setVendor( good::Product type, bool available );

  unsigned int buyPrice( good::Product type ) const;
  void setBuyPrice( good::Product type, unsigned int price );

  Order getOrder( good::Product type ) const;
  void setOrder( good::Product type, Order order );
  Order switchOrder( good::Product type );

  VariantMap save() const;
  void load( const VariantMap& stream );

  const good::Store& importingGoods();
  const good::Store& exportingGoods();

public:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace trade

}//end namespace city

#endif //_CAESARIA_CITYTRADEOPTIONS_INCLUDE_H_
