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

#include "picture_info_bank.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"
#include "core/logger.hpp"
#include <map>

class PictureInfoBank::Impl
{
public:
  void setRange(const std::string &preffix, const int first, const int last, const Point &data);
  void setOne(const std::string &preffix, const int index, const Point& data);
  void setOne(const std::string &preffix, const int index, const int xoffset, const int yoffset);

  typedef std::map< unsigned int, Point> PictureInfoMap;
  PictureInfoMap data;   // key=image name (Govt_00005)
};

PictureInfoBank& PictureInfoBank::instance()
{
  static PictureInfoBank inst;
  return inst;
}

PictureInfoBank::PictureInfoBank() : _d( new Impl )
{
  // tiles
  Point offset = getDefaultOffset( tileOffset );
  _d->setRange( ResourceGroup::land1a, 1, 303, offset);
  _d->setRange( "oc3_land", 1, 2, offset);
  _d->setRange( ResourceGroup::land2a, 1, 151, offset);
  _d->setRange( ResourceGroup::land2a, 187, 195, offset); //burning ruins start animation
  _d->setRange( ResourceGroup::land2a, 214, 231, offset); //burning ruins middle animation
  _d->setRange( ResourceGroup::land3a, 47, 92, offset);
  _d->setRange( ResourceGroup::plateau, 1, 44, offset);
  _d->setRange( ResourceGroup::commerce, 1, 167, offset);
  _d->setRange( ResourceGroup::transport, 1, 93, offset);
  _d->setOne( ResourceGroup::transport, 72, 0, 44 ); //lifting low bridge sw
  _d->setOne( ResourceGroup::transport, 74, 0, 68 ); //span high bridge se
  _d->setOne( ResourceGroup::transport, 77, 0, 53 ); //span high bridge sw
  _d->setRange( ResourceGroup::security, 1, 61, offset);
  _d->setRange( ResourceGroup::entertaiment, 1, 116, offset);
  _d->setRange( ResourceGroup::housing, 1, 51, offset);
  _d->setRange( ResourceGroup::warehouse, 19, 83, offset);
  _d->setRange( ResourceGroup::utilitya, 1, 42, offset);
  _d->setRange( ResourceGroup::govt, 1, 10, offset);
  _d->setRange( ResourceGroup::sprites, 1, 8, offset ); //collapse fog
  _d->setRange( ResourceGroup::sprites, 224, 225, offset ); //overlay columns
  _d->setRange( ResourceGroup::wall, 152, 185, offset );
  _d->setRange( ResourceGroup::sprites, 114, 161, offset );

  _d->setRange( ResourceGroup::waterOverlay, 1, 2, offset ); //wateroverlay building 1x1
  _d->setRange( ResourceGroup::waterOverlay, 11, 12, offset ); //wateroverlay houses 1x1
  _d->setRange( ResourceGroup::waterOverlay, 21, 22, offset ); //wateroverlay reservoir area 1x1

  _d->setOne( ResourceGroup::entertaiment, 12, 37, 62); // amphitheater
  _d->setOne( ResourceGroup::entertaiment, 35, 34, 37); // theater
  _d->setOne( ResourceGroup::entertaiment, 50, 70, 105);  // collosseum

  // animations
  _d->setRange(ResourceGroup::commerce, 2, 11, Point( 42, 34 ));  // market poor
  _d->setRange(ResourceGroup::commerce, 44, 53, Point( 66, 44 ));  // marble
  _d->setRange(ResourceGroup::commerce, 55, 60, Point( 45, 18 ));  // iron
  _d->setRange(ResourceGroup::commerce, 62, 71, Point( 15, 32 ));  // clay
  _d->setRange(ResourceGroup::commerce, 73, 82, Point( 35, 6 ) );  // timber
  _d->setRange(ResourceGroup::commerce, 87, 98, Point( 14, 36 ) );  // wine
  _d->setRange(ResourceGroup::commerce, 100, 107, Point( 0, 45 ) );  // oil
  _d->setRange(ResourceGroup::commerce, 109, 116, Point( 42, 36 ) );  // weapons
  _d->setRange(ResourceGroup::commerce, 118, 131, Point( 38, 39) );  // furniture
  _d->setRange(ResourceGroup::commerce, 133, 139, Point( 65, 42 ) );  // pottery
  _d->setRange(ResourceGroup::commerce, 159, 167, Point( 62, 42 ) );  // market rich

  _d->setOne( ResourceGroup::land3a, 43, Point( 0, 116 ) );
  _d->setOne( "circus", 5, 0, 106 );

  // stock of input good
  _d->setOne(ResourceGroup::commerce, 153, 45, -8);  // grapes
  _d->setOne(ResourceGroup::commerce, 154, 37, -2);  // olive
  _d->setOne(ResourceGroup::commerce, 155, 48, -4);  // timber
  _d->setOne(ResourceGroup::commerce, 156, 47, -11);  // iron
  _d->setOne(ResourceGroup::commerce, 157, 47, -9);  // clay

  // granary
  _d->setOne(ResourceGroup::commerce, 141, 28, 109);
  _d->setOne(ResourceGroup::commerce, 142, 33, 75);
  _d->setOne(ResourceGroup::commerce, 143, 56, 65);
  _d->setOne(ResourceGroup::commerce, 144, 92, 65);
  _d->setOne(ResourceGroup::commerce, 145, 118, 76);
  _d->setOne(ResourceGroup::commerce, 146, 78, 69);
  _d->setOne(ResourceGroup::commerce, 147, 78, 69);
  _d->setOne(ResourceGroup::commerce, 148, 78, 69);
  _d->setOne(ResourceGroup::commerce, 149, 78, 69);
  _d->setOne(ResourceGroup::commerce, 150, 78, 69);
  _d->setOne(ResourceGroup::commerce, 151, 78, 69);
  _d->setOne(ResourceGroup::commerce, 152, 78, 69);

  //legion standart + flag
  _d->setRange( ResourceGroup::sprites, 21, 47, Point( -32, 29 ) );
  _d->setRange( ResourceGroup::sprites, 48, 68, Point( -32, 52 ) );

  // walkers
  offset = getDefaultOffset( walkerOffset );
  _d->setRange(ResourceGroup::citizen1, 1, 1240, offset);
  _d->setRange(ResourceGroup::citizen2, 1, 1030, offset);
  _d->setRange(ResourceGroup::citizen3, 1, 1128, offset);
  _d->setRange(ResourceGroup::citizen4, 1, 577, offset);
  _d->setRange(ResourceGroup::citizen5, 1, 184, offset);
  _d->setRange("celts", 1, 552, offset );

  _d->setRange(ResourceGroup::carts, 369, 464, Point( -15, 32) ); //market lady helper
  _d->setRange(ResourceGroup::carts, 145, 240, Point( -15, 32) ); //merchant horse caravan
  _d->setRange(ResourceGroup::carts, 249, 264, offset );  //fishing boat
  _d->setRange(ResourceGroup::carts, 241, 248, offset );  //sea merchant boat
  _d->setRange(ResourceGroup::carts, 265, 272, offset );  //ship die
}

void PictureInfoBank::Impl::setRange(const std::string& preffix, const int first, const int last, const Point& data)
{
  for( int i = first; i<=last; ++i)
  {
    setOne(preffix, i, data);
  }
}

void PictureInfoBank::Impl::setOne(const std::string& preffix, const int index, const Point& offset)
{
  unsigned int hashName = utils::hash( 0xff, "%s_%05d", preffix.c_str(), index );
  data[hashName] = offset;
}

void PictureInfoBank::Impl::setOne(const std::string& preffix, const int index, const int xoffset, const int yoffset)
{
  unsigned int hashName = utils::hash( 0xff, "%s_%05d", preffix.c_str(), index );
  data[hashName] = Point( xoffset, yoffset );
}

Point PictureInfoBank::getOffset(const std::string& resource_name)
{
  Impl::PictureInfoMap::iterator it = _d->data.find( utils::hash( resource_name ) );
  if (it == _d->data.end())
  {
    return Point();
    // THROW("Invalid resource name: " << resource_name);
  }

  return (*it).second;
}

void PictureInfoBank::setOffset(const std::string& preffix, const int index, const Point& data)
{
  _d->setOne( preffix, index, data );
}

void PictureInfoBank::setOffset(const std::string& preffix, const int index, const int count, const Point& data)
{
  _d->setRange( preffix, index, index + count, data );
}

PictureInfoBank::~PictureInfoBank() {}

void PictureInfoBank::initialize(vfs::Path filename)
{
  Logger::warning( "PictureInfoBank: start load offsets from " + filename.toString() );
  VariantMap m = config::load( filename );

  foreach( it, m )
  {
    Variant v = it->second;
    Logger::warning( "Set offset for " + it->first );
    if( v.type() == Variant::Map )
    {
      VariantMap vm = v.toMap();
      int startIndex = vm[ "start" ];
      int stopIndex = vm[ "stop" ];
      std::string rc = vm[ "rc" ].toString();
      Point offset = vm[ "offset" ].toPoint();
      _d->setRange( rc.empty() ? it->first : rc, startIndex, stopIndex, offset );
    }
    else if( v.type() == Variant::List )
    {
      VariantList vl = v.toList();
      _d->setOne( it->first, vl.get( 0 ).toInt(), vl.get( 1 ).toInt(), vl.get( 2 ).toInt() );
    }
  }
}

Point PictureInfoBank::getDefaultOffset(PictureInfoBank::OffsetType type) const
{
  switch( type )
  {
  case walkerOffset: return Point( -2, -2 );
  case tileOffset: return Point( -1, -1 );
  }

  return Point();
}
