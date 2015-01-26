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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "goodhelper.hpp"
#include "good.hpp"
#include "core/enumerator.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/animation_bank.hpp"
#include "city/trade_options.hpp"
#include "city/city.hpp"
#include "core/logger.hpp"
#include "core/metric.hpp"
#include <vector>

using namespace gfx;
using namespace metric;

namespace good
{

static const int empPicId[ 20 ] = { PicID::bad,
                                       /*G_WHEAT*/11, 
                                       /*G_FISH*/27,
                                       /*G_MEAT*/16, 
                                       /*G_FRUIT*/13, 
                                       /*G_VEGETABLE*/12,                           
                                       /*G_OLIVE*/14, 
                                       /*G_OIL*/18,
                                       /*G_GRAPE*/15,                           
                                       /*G_WINE*/17, 
                                       /*G_TIMBER*/20, 
                                       /*G_FURNITURE*/24,
                                       /*G_CLAY*/21,
                                       /*G_POTTERY*/25, 
                                       /*G_IRON*/19, 
                                       /*G_WEAPON*/23,                                                   
                                       /*G_MARBLE*/22,                                                    
                                       /*G_DENARIES*/26,                            
                                       PicID::bad };

 static const int localPicId[ 20 ] = { PicID::bad,
                                          /*G_WHEAT*/317, 
                                          /*G_FISH*/333,
                                          /*G_MEAT*/322,
                                          /*G_FRUIT*/319,
                                          /*G_VEGETABLE*/318,
                                          /*G_OLIVE*/320,
                                          /*G_OIL*/324,
                                          /*G_GRAPE*/321,   
                                          /*G_WINE*/323, 
                                          /*G_TIMBER*/326,
                                          /*G_FURNITURE*/330,
                                          /*G_CLAY*/327,
                                          /*G_POTTERY*/331,
                                          /*G_IRON*/325,  
                                          /*G_WEAPON*/329, 
                                          /*G_MARBLE*/328, 
                                          /*G_DENARIES*/332,
                                          PicID::bad };

class Helper::Impl : public EnumsHelper<good::Product>
{
public:
  typedef std::map<good::Product, std::string > GoodNames;
  GoodNames goodName;  // index=GoodType, value=Good

  void append( good::Product type, const std::string& name, const std::string& prName )
  {
    EnumsHelper<good::Product>::append( type, name );
    goodName[ type ] = prName;
  }

  Impl() : EnumsHelper<good::Product>(good::none)
  {
#define __REG_GTYPE(a) append( good::a, CAESARIA_STR_EXT(a), "##"CAESARIA_STR_EXT(a)"##" );
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
    append( good::goodCount, "unknown", "##unknown##" );
#undef __REG_GTYPE
  }
};

Helper& Helper::getInstance()
{
  static Helper inst;
  return inst;
}

Helper::Helper() : _d( new Impl )
{  
}

Picture Helper::picture(Product type, bool emp )
{
  int picId = -1;

  if( emp )
  {
    picId = empPicId[ type.toInt() ];
  }
  else
  {
    picId = localPicId[ type.toInt() ];
  }
  
  if( picId > 0 )
  {
    return Picture::load( emp ? ResourceGroup::empirepnls : ResourceGroup::panelBackground, picId );
  }

  return Picture();
}

Helper::~Helper() {}

std::string Helper::name(Product type )
{
  Impl::GoodNames::iterator it = getInstance()._d->goodName.find( type );
  return it != getInstance()._d->goodName.end() ? it->second : "";
}

Product Helper::getType( const std::string& name )
{
  good::Product type = getInstance()._d->findType( name );

  if( type == getInstance()._d->getInvalid() )
  {
    Logger::warning( "Can't find type for goodName " + name );
    return good::none;
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find type for goodName" );
  }

  return type;
}

std::string Helper::getTypeName(Product type )
{
  return getInstance()._d->findName( type );
}

float Helper::exportPrice(PlayerCityPtr city, good::Product gtype, int qty)
{
  int price = city->tradeOptions().buyPrice( gtype );
  Unit units = Unit::fromQty( qty );
  return price * units.ivalue();
}

float Helper::importPrice(PlayerCityPtr city, Product gtype, int qty)
{
  int price = city->tradeOptions().sellPrice( gtype );
  Unit units = Unit::fromQty( qty );
  return price * units.ivalue();
}

const Animation& Helper::getCartPicture(const good::Stock& stock, constants::Direction direction)
{
  int index = (stock.empty() ? good::none : stock.type()).toInt();
  return AnimationBank::getCart( index, stock.capacity(), direction );
}

Product Helper::random()
{
  return Product( math::random( goodCount.toInt() ));
}

}//end namespace good
