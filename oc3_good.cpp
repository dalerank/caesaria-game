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

std::string Good::getName()
{
   return _name;
}

int Good::getImportPrice()
{
   return _importPrice;
}

int Good::getExportPrice()
{
   return _exportPrice;
}

bool Good::isAllowUsage()
{
   return _allowUsage;
}

bool Good::isAllowStorage()
{
   return _allowStorage;
}

bool Good::isAllowImport()
{
   return _allowImport;
}

bool Good::isAllowExport()
{
   return _allowExport;
}

void Good::init(const GoodType &goodType)
{
  _goodType = goodType;
  _name = "";
  _importPrice = 0;
  _exportPrice = 0;

  _allowUsage = true;
  _allowStorage = true;
  _allowImport = false;
  _allowExport = false;

  _usageQuota = 100;     // full usage
  _importTreshold = 16;  // lots of import
  _exportTreshold = 0;   // lots of export

  // init static values
  switch (goodType)
  {
  case G_NONE:
     _name = _("None");
     break;
  case G_WHEAT:
     _name = _("Wheat");
     _importPrice = 28;
     _exportPrice = 22;
     break;
  case G_FISH:
     _name = _("Fish");  // no import/export!
     break;
  case G_MEAT:
     _name = _("Meat");
     _importPrice = 44;
     _exportPrice = 36;
     break;
  case G_FRUIT:
     _name = _("Fruits");
     _importPrice = 38;
     _exportPrice = 30;
     break;
  case G_VEGETABLE:
     _name = _("Vegetables");
     _importPrice = 38;
     _exportPrice = 30;
     break;
  case G_OLIVE:
     _name = _("Olives");
     _importPrice = 42;
     _exportPrice = 34;
     break;
  case G_OIL:
     _name = _("Oil");
     _importPrice = 180;
     _exportPrice = 140;
     break;
  case G_GRAPE:
     _name = _("Grape");
     _importPrice = 44;
     _exportPrice = 36;
     break;
  case G_WINE:
     _name = _("Vine");
     _importPrice = 215;
     _exportPrice = 160;
     break;
  case G_TIMBER:
     _name = _("Timber");
     _importPrice = 50;
     _exportPrice = 35;
     break;
  case G_FURNITURE:
     _name = _("Furniture");
     _importPrice = 200;
     _exportPrice = 150;
     break;
  case G_CLAY:
     _name = _("Clay");
     _importPrice = 40;
     _exportPrice = 30;
     break;
  case G_POTTERY:
     _name = _("Pottery");
     _importPrice = 180;
     _exportPrice = 140;
     break;
  case G_IRON:
     _name = _("Iron");
     _importPrice = 60;
     _exportPrice = 40;
     break;
  case G_WEAPON:
     _name = _("Weapon");
     _importPrice = 250;
     _exportPrice = 180;
     break;
  case G_MARBLE:
     _name = _("Marble");
     _importPrice = 200;
     _exportPrice = 140;
     break;
  case G_DENARIES:
    _name = _("##Denaries##");
    break;
  default:
     _OC3_DEBUG_BREAK_IF( true && goodType && "Unexpected goodType" );
  }
}


GoodStock::GoodStock()
{
   _goodType = G_NONE;
   _maxQty = 0;
   _currentQty = 0;
}

GoodStock::GoodStock(const GoodType &goodType, const int maxQty, const int currentQty)
{
   _goodType = goodType;
   _maxQty = maxQty;
   _currentQty = currentQty;
}

void GoodStock::addStock(GoodStock &stock, const int iAmount)
{
  if (stock._goodType == G_NONE)
  {
     // nothing to add => nothing to do!
     return;
  }
  if (_goodType != G_NONE && _goodType != stock._goodType) 
  {
    std::string errorStr = StringHelper::format( 0xff, "GoodTypes do not match: %d vs %d", _goodType, stock._goodType );
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

  _goodType = stock._goodType;  // in case goodType was G_NONE
  _currentQty += amount;
  stock._currentQty -= amount;
  
  if (stock._currentQty == 0)
  {
     stock._goodType = G_NONE;
  }
}

VariantList GoodStock::save() const
{
  VariantList stream;
  stream.push_back( (int)_goodType );
  stream.push_back( _maxQty );
  stream.push_back( _currentQty );

  return stream;
}

void GoodStock::load( const VariantList& stream )
{
  if( stream.size() < 3 )
    return;

  VariantList::const_iterator it=stream.begin();
  _goodType = (GoodType)(*it).toInt(); it++;
  _maxQty = (*it).toInt(); it++;
  _currentQty = (*it).toInt();
}

bool GoodStock::empty() const
{
  return _currentQty == 0;
}
