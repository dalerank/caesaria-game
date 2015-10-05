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

#include "helper.hpp"
#include "stock.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "city/trade_options.hpp"
#include "city/city.hpp"
#include "core/logger.hpp"

using namespace gfx;
using namespace metric;

namespace good
{

struct PicId
{
  Product type ;
  int emp ;
  int local;
};

static std::map<good::Product, PicId> defaultsPicId = {
  { none     , { none,       0,   0 }},
  { wheat    , { wheat,     11, 317 }},
  { vegetable, { vegetable, 12, 318 }},
  { fruit    , { fruit,     13, 319 }},
  { olive    , { olive,     14, 320 }},
  { grape    , { grape,     15, 321 }},
  { meat     , { meat,      16, 322 }},
  { wine     , { wine,      17, 323 }},
  { oil      , { oil,       18, 324 }},
  { iron     , { iron,      19, 325 }},
  { timber   , { timber,    20, 326 }},
  { clay     , { clay,      21, 327 }},
  { marble   , { marble,    22, 328 }},
  { weapon   , { weapon,    23, 329 }},
  { furniture, { furniture, 24, 330 }},
  { pottery  , { pottery,   25, 331 }},
  { denaries , { denaries,  26, 332 }},
  { prettyWine,{ prettyWine,17, 323 }},
  { fish     , { fish,      27, 333 }}
};

class Helper::Impl : public EnumsHelper<good::Product>
{
public:
  typedef std::map<good::Product, std::string > GoodNames;
  GoodNames goodName;  // index=GoodType, value=Good
  const std::string invalidText;

  void append( good::Product type, const std::string& name, const std::string& prName )
  {
    EnumsHelper<good::Product>::append( type, name );
    goodName[ type ] = prName;
  }

  Impl() : EnumsHelper<good::Product>(good::none)
  {
#define __REG_GTYPE(a) append( good::a, CAESARIA_STR_EXT(a), "##" CAESARIA_STR_EXT(a)"##" );
    __REG_GTYPE(none )
    __REG_GTYPE(wheat)
    __REG_GTYPE(fish )
    __REG_GTYPE(meat )
    __REG_GTYPE(fruit)
    __REG_GTYPE(vegetable)
    __REG_GTYPE(olive)
    __REG_GTYPE(oil  )
    __REG_GTYPE(grape)
    __REG_GTYPE(wine )
    __REG_GTYPE(timber)
    __REG_GTYPE(furniture)
    __REG_GTYPE(clay )
    __REG_GTYPE(pottery )
    __REG_GTYPE(iron )
    __REG_GTYPE(weapon )
    __REG_GTYPE(marble )
    __REG_GTYPE(denaries )
    __REG_GTYPE(prettyWine )
#undef __REG_GTYPE
  }
};

Helper& Helper::instance()
{
  static Helper inst;
  return inst;
}

Helper::Helper() : _d( new Impl )
{  
}

Picture Helper::picture(Product type, bool emp )
{
  const PicId& info = defaultsPicId[ type ];

  int picId = emp ? info.emp : info.local;
  
  if( picId > 0 )
  {
    return Picture( emp ? ResourceGroup::empirepnls : ResourceGroup::panelBackground, picId );
  }

  return Picture();
}

Helper::~Helper() {}

const std::string& Helper::name(Product type )
{
  Impl::GoodNames::iterator it = instance()._d->goodName.find( type );
  return it != instance()._d->goodName.end() ? it->second : instance()._d->invalidText;
}

Product Helper::getType( const std::string& name )
{
  good::Product type = instance()._d->findType( name );

  if( type == instance()._d->getInvalid() )
  {
    Logger::warning( "Can't find type for goodName " + name );
    return good::none;
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find type for goodName" );
  }

  return type;
}

std::string Helper::getTypeName(Product type )
{
  return instance()._d->findName( type );
}

float Helper::exportPrice(PlayerCityPtr city, good::Product gtype, unsigned int qty)
{
  int price = city->tradeOptions().buyPrice( gtype );
  Unit units = Unit::fromQty( qty );
  return price * units.ivalue();
}

float Helper::importPrice(PlayerCityPtr city, Product gtype, unsigned int qty)
{
  int price = city->tradeOptions().sellPrice( gtype );
  Unit units = Unit::fromQty( qty );
  return price * units.ivalue();
}

Product Helper::random()
{
  return Product( math::random( good::all().size()-1 ));
}

}//end namespace good
