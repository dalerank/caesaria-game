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
#include "oc3_enums_helper.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_animation_bank.hpp"
#include "oc3_gettext.hpp"
#include <vector>

static const int empPicId[ Good::G_MAX+1 ] = { PicID::bad,
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

 static const int localPicId[ Good::G_MAX+1 ] = { PicID::bad,
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

  void append( Good::Type type, const std::string& name, const std::string prName )
  {
    EnumsHelper<Good::Type>::append( type, name );
    goodName[ type ] = prName;
  }

  Impl() : EnumsHelper<Good::Type>(Good::G_NONE)
  {
    append( Good::G_NONE, "none", _("None") );
    append( Good::G_WHEAT, "wheat", _("Wheat") );
    append( Good::G_FISH, "fish", _("Fish") );
    append( Good::G_MEAT, "meat", _("Meat") );
    append( Good::G_FRUIT, "fruit", _("Fruits"));
    append( Good::G_VEGETABLE, "vegetable", _("Vegetables") );
    append( Good::G_OLIVE, "olive", _("Olives") );
    append( Good::G_OIL, "oil", _("Oil") );
    append( Good::G_GRAPE, "grape", _("Grape") );
    append( Good::G_WINE, "wine", _("Wine") );
    append( Good::G_TIMBER, "timber", _("Timber") );
    append( Good::G_FURNITURE, "furniture", _("Furniture") );
    append( Good::G_CLAY, "clay", _("Clay") );
    append( Good::G_POTTERY, "pottery", _("Pottery") );
    append( Good::G_IRON, "iron", _("Iron") );
    append( Good::G_WEAPON, "weapon", _("Weapon") );
    append( Good::G_MARBLE, "marble", _("Marble") );
    append( Good::G_DENARIES, "denaries", _("##Denaries##") );
    append( Good::G_MAX, "", "unknown" );
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

Picture GoodHelper::getPicture( Good::Type type, bool emp )
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

std::string GoodHelper::getName( Good::Type type )
{
  Impl::GoodNames::iterator it = getInstance()._d->goodName.find( type );
  return it != getInstance()._d->goodName.end() ? it->second : "";
}

Good::Type GoodHelper::getType( const std::string& name )
{
  Good::Type type = getInstance()._d->findType( name );

  if( type == getInstance()._d->getInvalid() )
  {
    StringHelper::debug( 0xff, "Can't find type for goodName %s", name.c_str() );
    _OC3_DEBUG_BREAK_IF( "Can't find type for goodName" );
  }

  return type;
}

std::string GoodHelper::getTypeName( Good::Type type )
{
  return getInstance()._d->findName( type );
}

const Picture& GoodHelper::getCartPicture(const GoodStock &stock, const DirectionType &direction)
{
  return AnimationBank::getCart( stock.empty() ? Good::G_NONE :  stock._type, direction );
}
