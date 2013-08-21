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

#include "oc3_goodhelper.hpp"
#include "oc3_good.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_animation_bank.hpp"
#include <vector>

static const int empPicId[ G_MAX+1 ] = { PicID::bad, 
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

 static const int localPicId[ G_MAX+1 ] = { PicID::bad, 
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


TypeEquale<GoodType> goodTypeEquales[] = { 
  { G_NONE, "none" },
  { G_WHEAT, "wheat" },
  { G_FISH, "fish" },
  { G_MEAT, "meat" },
  { G_FRUIT, "fruit" }, 
  { G_VEGETABLE, "vegetable" }, { G_VEGETABLE, "vegetables" },
  { G_OLIVE, "olive" }, { G_OLIVE, "olives" },
  { G_OIL, "oil" },
  { G_GRAPE, "grape" }, { G_GRAPE, "vines" },
  { G_WINE, "wine" }, 
  { G_TIMBER, "timber" },
  { G_FURNITURE, "furniture" },
  { G_CLAY, "clay" }, 
  { G_POTTERY, "pottery" }, 
  { G_IRON, "iron" }, 
  { G_WEAPON, "weapon" }, { G_WEAPON, "weapons" },
  { G_MARBLE, "marble" }, 
  { G_DENARIES, "denaries" },
  { G_MAX, "" }
};

class GoodHelper::Impl
{
public:
  std::vector<Good> mapGood;  // index=GoodType, value=Good
};

GoodHelper& GoodHelper::getInstance()
{
  static GoodHelper inst;
  return inst;
}

GoodHelper::GoodHelper() : _d( new Impl )
{
  _d->mapGood.resize(G_MAX);

  for (int n = 0; n < G_MAX; ++n)
  {
    GoodType goodType = GoodType(n);
    _d->mapGood[n].init( goodType );
  }
}

Picture GoodHelper::getPicture( GoodType type, bool emp )
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

GoodHelper::~GoodHelper()
{

}

std::string GoodHelper::getName( GoodType type )
{
  return getInstance()._d->mapGood[ type ].getName();
}

GoodType GoodHelper::getType( const std::string& name )
{
  int index=0;
  std::string typeName = goodTypeEquales[ index ].name;

  while( !typeName.empty() )
  {
    if( name == typeName )
    {
      return goodTypeEquales[ index ].type;
    }

    index++;
    typeName = goodTypeEquales[ index ].name;
  }

  StringHelper::debug( 0xff, "Can't find type for goodName %s", name.c_str() );
  _OC3_DEBUG_BREAK_IF( "Can't find type for goodName" );
  return G_NONE;
}

std::string GoodHelper::getTypeName( GoodType type )
{
 
  for( int index=0; goodTypeEquales[ index ].type != G_MAX; index++ )
  {
    if( goodTypeEquales[ index ].type == type )
    {
      return goodTypeEquales[ index ].name;
    }
  } 

  return "unknown";
}

const Picture& GoodHelper::getCartPicture(const GoodStock &stock, const DirectionType &direction)
{
  return AnimationBank::getCart( stock.empty() ? G_NONE :  stock._goodType, direction );
}