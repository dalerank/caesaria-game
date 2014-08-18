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
#include "core/stringhelper.hpp"
#include "gfx/animation_bank.hpp"
#include "core/logger.hpp"
#include <vector>

using namespace gfx;

static const int empPicId[ Good::goodCount+1 ] = { PicID::bad,
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

 static const int localPicId[ Good::goodCount+1 ] = { PicID::bad,
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

class GoodHelper::Impl : public EnumsHelper<Good::Type>
{
public:
  typedef std::map<Good::Type, std::string > GoodNames;
  GoodNames goodName;  // index=GoodType, value=Good

  void append( Good::Type type, const std::string& name, const std::string& prName )
  {
    EnumsHelper<Good::Type>::append( type, name );
    goodName[ type ] = prName;
  }

  Impl() : EnumsHelper<Good::Type>(Good::none)
  {
#define __REG_GTYPE(a) append( Good::a, CAESARIA_STR_EXT(a), "##"CAESARIA_STR_EXT(a)"##" );
    __REG_GTYPE(none ) // append( Good::none, "none", "##none##");
    __REG_GTYPE(wheat) // append( Good::wheat, "wheat", "##wheat##" );
    __REG_GTYPE(fish ) // append( Good::fish, "fish", "##fish##" );
    __REG_GTYPE(meat ) // append( Good::meat, "meat", "##meat##" );
    __REG_GTYPE(fruit) // append( Good::fruit, "fruit", "##fruit##");
    __REG_GTYPE(vegetable) //append( Good::vegetable, "vegetable", "##vegetable##" );
    __REG_GTYPE(olive) // append( Good::olive, "olive", "##olive##" );
    __REG_GTYPE(oil  ) // append( Good::oil, "oil", "##oil##" );
    __REG_GTYPE(grape) // append( Good::grape, "grape", "##grape##" );
    __REG_GTYPE(wine ) // append( Good::wine, "wine", "##wine##" );
    __REG_GTYPE(timber)// append( Good::timber, "timber", "##timber##" );
    __REG_GTYPE(furniture) // append( Good::furniture, "furniture", "##furniture##" );
    __REG_GTYPE(clay ) // append( Good::clay, "clay", "##clay##" );
    __REG_GTYPE(pottery ) // append( Good::pottery, "pottery", "##pottery##" );
    __REG_GTYPE(iron ) // append( Good::iron, "iron", "##iron##" );
    __REG_GTYPE(weapon ) // append( Good::weapon, "weapon", "##weapon##" );
    __REG_GTYPE(marble ) // append( Good::marble, "marble", "##marble##" );
    __REG_GTYPE(denaries ) //append( Good::denaries, "denaries", "##denaries##");
    __REG_GTYPE(prettyWine ) // append( Good::prettyWine, "prettyWine", "##prettyWine##" );
    append( Good::goodCount, "unknown", "##unknown##" );
#undef __REG_GTYPE
  }
};

GoodHelper& GoodHelper::getInstance()
{
  static GoodHelper inst;
  return inst;
}

GoodHelper::GoodHelper() : _d( new Impl )
{  
}

Picture GoodHelper::picture( Good::Type type, bool emp )
{
  int picId = -1;

  if( emp )
  {
    picId = empPicId[ type ];
  }
  else
  {
    picId = localPicId[ type ];
  }
  
  if( picId > 0 )
  {
    return Picture::load( emp ? ResourceGroup::empirepnls : ResourceGroup::panelBackground, picId );
  }

  return Picture();
}

GoodHelper::~GoodHelper() {}

std::string GoodHelper::name( Good::Type type )
{
  Impl::GoodNames::iterator it = getInstance()._d->goodName.find( type );
  return it != getInstance()._d->goodName.end() ? it->second : "";
}

Good::Type GoodHelper::getType( const std::string& name )
{
  Good::Type type = getInstance()._d->findType( name );

  if( type == getInstance()._d->getInvalid() )
  {
    Logger::warning( "Can't find type for goodName %s", name.c_str() );
    return Good::none;
    //_CAESARIA_DEBUG_BREAK_IF( "Can't find type for goodName" );
  }

  return type;
}

std::string GoodHelper::getTypeName( Good::Type type )
{
  return getInstance()._d->findName( type );
}

Picture GoodHelper::getCartPicture(const GoodStock& stock, constants::Direction direction)
{
  return AnimationBank::getCart( stock.empty() ? Good::none : stock.type(), stock.capacity(), direction );
}
