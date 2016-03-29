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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2015 dalerank, dalerankn8@gmail.com

#include "metadata.hpp"

#include <map>
#include "core/gettext.hpp"
#include "core/saveadapter.hpp"
#include "core/utils.hpp"
#include "core/enumerator.hpp"
#include "core/foreach.hpp"
#include "city/desirability.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "infodb.hpp"
#include "constants.hpp"
#include "gfx/picture_info_bank.hpp"
#include "core/variant_list.hpp"

using namespace gfx;

namespace object
{

Info Info::invalid = Info( object::unknown, "unknown" );
namespace { enum { idxRcFamily=0, idxRcNumber }; }

struct AdvPictures : std::map< int, StringArray >
{
  void loadMain( const VariantMap& options )
  {
    VariantList basePic = options.get( "image" ).toList();
    if( !basePic.empty() )
    {
      std::string groupName = basePic.get( idxRcFamily ).toString();
      int imageIndex = basePic.get( idxRcNumber ).toInt();
      Variant vOffset = options.get( "image.offset" );
      if( vOffset.isValid() )
      {
        PictureInfoBank::instance().setOffset( groupName, imageIndex, vOffset.toPoint() );
      }

      Picture pic( groupName, imageIndex );
      if( pic.isValid() )
        (*this)[ 0 ].push_back( pic.name() );
    }
  }

  void loadExt( const VariantMap& options )
  {
    VariantMap extPics = options.get( "image.ext" ).toMap();
    for( auto& config : extPics )
    {
      VariantMap info = config.second.toMap();
      VARIANT_INIT_ANY( int, size, info )
      VARIANT_INIT_ANY( int, start, info );
      VARIANT_INIT_ANY( int, count, info );
      VARIANT_INIT_STR( rc, info );

      for( int i=0; i < count; i++ )
      {
        Picture pic( rc, start + i );
        if( pic.isValid() )
          (*this)[ size ].push_back( pic.name() );
      }
    }
  }
};

struct AdvSound
{
  std::string rc;

  void load( const VariantMap& options )
  {
    VariantList soundVl = options.get( "sound" ).toList();
    if( !soundVl.empty() )
    {
      rc = utils::format( 0xff, "%s_%05d",
                          soundVl.get( idxRcFamily ).toString().c_str(),
                          soundVl.get( idxRcNumber ).toInt() );
    }
  }
};

class Info::Impl
{
public:
  Desirability desirability;
  object::Type tileovType;
  object::Group group;
  std::string name;  // debug name  (english, ex:"iron")
  AdvSound sound;
  StringArray desc;
  VariantMap options;
  std::string prettyName;

  AdvPictures pictures;
};

Info::Info(const object::Type buildingType, const std::string& name )
  : _d( new Impl )
{
  _d->prettyName = "##" + name + "##";
  _d->tileovType = buildingType;
  _d->group = object::group::unknown;
  _d->name = name;
}

void Info::initialize(const VariantMap& options )
{
  _d->options = options;
  VariantMap desMap = options.get( "desirability" ).toMap();
  _d->desirability.VARIANT_LOAD_ANY(base, desMap );
  _d->desirability.VARIANT_LOAD_ANY(range, desMap);
  _d->desirability.VARIANT_LOAD_ANY(step, desMap );

  _d->desc = options.get( "desc" ).toStringArray();
  _d->prettyName = options.get( "prettyName", Variant( _d->prettyName ) ).toString();

  _d->group = InfoDB::findGroup( options.get( "class" ).toString() );

  _d->pictures.loadMain( options );
  _d->pictures.loadExt( options );
  _d->sound.load( options );
}

Info::Info(const Info& a) : _d( new Impl )
{
  *this = a;
}

const Info& Info::find(Type type)
{
  return InfoDB::find( type );
}

Info::~Info(){}
std::string Info::name() const{  return _d->name;}
std::string Info::sound() const{  return _d->sound.rc;}
std::string Info::prettyName() const {  return _d->prettyName;}

std::string Info::description() const
{
  if( _d->desc.empty() )
    return "##" + _d->name + "_info##";

  return _d->desc[ rand() % _d->desc.size() ];
}

object::Type Info::type() const {  return _d->tileovType;}

std::string Info::typeName() const { return object::toString( type() ); }
const Desirability& Info::desirability() const{  return _d->desirability;}

Picture Info::randomPicture(const Size& size) const
{
  return randomPicture( size.width() );
}

Picture Info::randomPicture(int size) const
{
  return _d->pictures[ size ].random();
}

bool Info::havePicture(const std::string& name) const
{
  for( auto& pics : _d->pictures )
  {
    bool found = pics.second.contains( name );
    if( found )
      return true;
  }

  return false;
}

Variant Info::getOption(const std::string &name, Variant defaultVal ) const
{
  VariantMap::iterator it = _d->options.find( name );
  return it != _d->options.end() ? it->second : defaultVal;
}

bool Info::getFlag(const std::string& name, bool defValue) const
{
  return getOption( name, defValue ).toBool();
}

Info& Info::operator=(const Info& a)
{
  _d->tileovType = a._d->tileovType;
  _d->name = a._d->name;
  _d->prettyName = a._d->prettyName;
  _d->sound = a._d->sound;
  _d->pictures = a._d->pictures;
  _d->group = a._d->group;
  _d->desirability = a._d->desirability;
  _d->desc = a._d->desc;
  _d->options = a._d->options;

  return *this;
}

void Info::reload() const { InfoDB::instance().reload( type() ); }
object::Group Info::group() const {  return _d->group; }

ProductConsumer::ProductConsumer(good::Product product)
 : _product( product )
{

}

TypeSet ProductConsumer::consumers() const
{
  TypeSet ret;
  ret.insert( InfoDB::instance().getConsumerType( _product ) );

  return ret;
}

Type ProductConsumer::consumer() const
{
  return InfoDB::instance().getConsumerType( _product );
}

}//end namespace object
