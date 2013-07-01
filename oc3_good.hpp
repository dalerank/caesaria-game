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
#include "oc3_serializer.hpp"

class VariantList;

class Good
{
  friend class GoodHelper;
public:
  std::string getName();
  int getImportPrice();
  int getExportPrice();
  bool isAllowUsage();
  bool isAllowStorage();
  bool isAllowImport();
  bool isAllowExport();

private:
  void init(const GoodType &goodType);

  GoodType _goodType;
  BuildingType _outFactoryType;  // type of factory, if any (ex: G_IRON => B_WEAPONS)
  std::string _name;
  int _importPrice;
  int _exportPrice;

  bool _allowUsage;
  bool _allowStorage;
  bool _allowImport;
  bool _allowExport;

  int _usageQuota;   // percent of (rich) population to give access to this product
  int _importTreshold;  // number of units in warehouses above which no import is made
  int _exportTreshold;  // number of units in warehouses under which no export is made
};

class GoodStock 
{
public:
  GoodStock();
  GoodStock(const GoodType &goodType, const int maxQty, const int currentQty=0);

  /** amount: if -1, amount=stock._currentQty */
  void addStock(GoodStock &stock, const int amount = -1);

  void save( VariantList& stream ) const;
  void load( const VariantList& options );

  GoodType _goodType;
  int _maxQty;
  int _currentQty;
};
    
#endif
