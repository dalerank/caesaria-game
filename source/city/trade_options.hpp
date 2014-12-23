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

class TradeOptions
{
public:
  typedef enum { importing=0, noTrade, exporting, stacking, disabled } Order;
  TradeOptions();
  ~TradeOptions();

  unsigned int exportLimit(good::Type type) const;
  void setExportLimit(good::Type type, unsigned int qty);
  
  void setStackMode( good::Type type, bool stacking );
  bool isGoodsStacking( good::Type type );

  bool isExporting( good::Type type ) const;
  bool isImporting( good::Type type ) const;

  unsigned int sellPrice( good::Type type ) const;
  void setSellPrice( good::Type type, unsigned int price );

  bool isVendor( good::Type type ) const;
  void setVendor( good::Type type, bool available );

  unsigned int buyPrice( good::Type type ) const;
  void setBuyPrice( good::Type type, unsigned int price );

  Order getOrder( good::Type type ) const;
  void setOrder( good::Type type, Order order );
  Order switchOrder( good::Type type );

  VariantMap save() const;
  void load( const VariantMap& stream );

  const good::Store& importingGoods();
  const good::Store& exportingGoods();

public:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace city

#endif //_CAESARIA_CITYTRADEOPTIONS_INCLUDE_H_
