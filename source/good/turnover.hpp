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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_TURNOVER_H_INCLUDED__
#define __CAESARIA_TURNOVER_H_INCLUDED__

#include "good.hpp"
#include "stock.hpp"
#include "core/position.hpp"
#include "core/scopedptr.hpp"
#include "core/variant_map.hpp"

namespace good
{

struct TurnoverInfo
{
  unsigned int qty = 0;
  Product type = Product::none;
  int level = 0;
  TilePos sender;
  TilePos receiver;
};

struct SmInfo
{
  unsigned int qty = 0;
  DateTime birth;
};

typedef std::vector<TurnoverInfo> Turnovers;
typedef std::vector<SmInfo> SmHistory;

class StockInfoMap : public std::map<Product,SmHistory>
{
public:
  VariantMap save() const;
  void load( const VariantMap& stream );
};

class TurnoverDetails : public std::map<unsigned int,StockInfoMap>
{
public:
  TilePos owner;
  void append(const Stock& stock );
  void append(Product gtype, int qty, int tag,  const DateTime& time);

  VariantMap save() const;
  void load( const VariantMap& stream );

  Turnovers items() const;
};

typedef TurnoverDetails ConsumerDetails;
typedef TurnoverDetails ProviderDetails;

}//end namespace good

#endif //__CAESARIA_TURNOVER_H_INCLUDED__
