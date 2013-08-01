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
#include <vector>

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

Picture GoodHelper::getPicture( GoodType type )
{
  int picId = -1;
  switch( type )
  {
  case G_WHEAT: picId = 317; break;
  case G_VEGETABLE: picId = 318; break;
  case G_FRUIT: picId = 319; break;
  case G_OLIVE: picId = 320; break;
  case G_GRAPE: picId = 321; break;
  case G_MEAT: picId = 322; break;
  case G_WINE: picId = 323; break;
  case G_OIL: picId = 324; break;
  case G_IRON: picId = 325; break;
  case G_TIMBER: picId = 326; break; 
  case G_CLAY: picId = 327; break;
  case G_MARBLE: picId = 328; break;
  case G_WEAPON: picId = 329; break;
  case G_FURNITURE: picId = 330; break;
  case G_POTTERY: picId = 331; break;
  case G_DENARIES: picId = 332; break;
  case G_FISH: picId = 333; break;
  default:
  break;
  }

  if( picId > 0 )
  {
    return Picture::load( ResourceGroup::panelBackground, picId);
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
  for( int i=G_NONE; i < G_MAX; i++ )
  {
    if( goodTypeEquales[ i ].type == type )
    {
      return goodTypeEquales[ i ].name;
    }
  } 

  return "unknown";
}