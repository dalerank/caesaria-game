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


#include "oc3_picture_info_bank.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"

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
  Point offset( -1, -1 );
  _d->setRange("land1a", 1, 303, offset);
  _d->setRange("oc3_land", 1, 2, offset);
  _d->setRange( ResourceGroup::land2a, 1, 151, offset);
  _d->setRange( ResourceGroup::land2a, 187, 195, offset); //burning ruins start animation
  _d->setRange( ResourceGroup::land2a, 214, 231, offset); //burning ruins middle animation
  _d->setRange( "land3a", 47, 92, offset);
  _d->setRange( "plateau", 1, 44, offset);
  _d->setRange( ResourceGroup::commerce, 1, 167, offset);
  _d->setRange( ResourceGroup::transport, 1, 93, offset);
  _d->setOne( ResourceGroup::transport, 72, 0, 5 ); //lifting low bridge sw
  _d->setOne( ResourceGroup::transport, 74, 0, 68 ); //span high bridge se
  _d->setOne( ResourceGroup::transport, 77, 0, 53 ); //span high bridge sw
  _d->setRange( ResourceGroup::security, 1, 61, offset);
  _d->setRange( ResourceGroup::entertaiment, 1, 116, offset);
  _d->setRange( ResourceGroup::housing, 1, 51, offset);
  _d->setRange( ResourceGroup::warehouse, 19, 83, offset);
  _d->setRange( ResourceGroup::utilitya, 1, 42, offset);
  _d->setRange( ResourceGroup::govt, 1, 10, offset);
  _d->setRange( ResourceGroup::sprites, 1, 8, offset ); //collapse fog
  _d->setRange( ResourceGroup::sprites, 9, 20, offset ); //overlay columns

  _d->setRange( ResourceGroup::waterOverlay, 1, 2, offset ); //wateroverlay building 1x1
  _d->setRange( ResourceGroup::waterOverlay, 11, 12, offset ); //wateroverlay houses 1x1
  _d->setRange( ResourceGroup::waterOverlay, 21, 22, offset ); //wateroverlay reservoir area 1x1
  _d->setRange( ResourceGroup::waterbuildings, 1, 4, offset ); //waterbuidlings (reservoir,fontain) empty/full

  offset = Point( 0, 30 );
  _d->setRange( ResourceGroup::waterOverlay, 3, 4, offset ); //wateroverlay building 2x2
  _d->setRange( ResourceGroup::waterOverlay, 13, 14, offset ); //wateroverlay houses 2x2 

  offset = Point( 0, 60 );

  _d->setRange( ResourceGroup::waterOverlay, 5, 6, offset ); //wateroverlay building 3x3
  _d->setRange( ResourceGroup::waterOverlay, 15, 16, offset ); //wateroverlay houses 3x3 

  offset = Point( 0, 90 );
  _d->setRange( ResourceGroup::waterOverlay, 7, 8, offset ); //wateroverlay building 4x4
  _d->setRange( ResourceGroup::waterOverlay, 17, 18, offset ); //wateroverlay houses 4x4 

  offset = Point( 0, 120 );
  _d->setRange( ResourceGroup::waterOverlay, 9, 10, offset ); //wateroverlay building 5x5

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

  // stock of input good
  _d->setOne(ResourceGroup::commerce, 153, 45, -8);  // grapes
  _d->setOne(ResourceGroup::commerce, 154, 37, -2);  // olive
  _d->setOne(ResourceGroup::commerce, 155, 48, -4);  // timber
  _d->setOne(ResourceGroup::commerce, 156, 47, -11);  // iron
  _d->setOne(ResourceGroup::commerce, 157, 47, -9);  // clay

  // warehouse
  _d->setOne(ResourceGroup::warehouse, 1, 60, 56);
  _d->setOne(ResourceGroup::warehouse, 18, 56, 93);
  _d->setRange(ResourceGroup::warehouse, 2, 17, Point( 55, 75 ));
  _d->setRange(ResourceGroup::warehouse, 84, 91, Point( 79, 108 ) );

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

  // walkers
  offset = Point( -2, -2 );
  _d->setRange("citizen01", 1, 1240, offset);
  _d->setRange("citizen02", 1, 1030, offset);
  _d->setRange("citizen03", 1, 1128, offset);
  _d->setRange("citizen04", 1, 577, offset);
  _d->setRange("citizen05", 1, 184, offset);
  _d->setRange(ResourceGroup::carts, 369, 464, Point( -15, 32) ); //market lady helper
}

void PictureInfoBank::Impl::setRange(const std::string& preffix, const int first, const int last, const Point& data)
{
  for (int i = first; i<=last; ++i)
  {
    setOne(preffix, i, data);
  }
}

void PictureInfoBank::Impl::setOne(const std::string& preffix, const int index, const Point& offset)
{
  unsigned int hashName = StringHelper::hash( 0xff, "%s_%05d", preffix.c_str(), index );
  data[hashName] = offset;
}

void PictureInfoBank::Impl::setOne(const std::string& preffix, const int index, const int xoffset, const int yoffset)
{
  unsigned int hashName = StringHelper::hash( 0xff, "%s_%05d", preffix.c_str(), index );
  data[hashName] = Point( xoffset, yoffset );
}

Point PictureInfoBank::getOffset(const std::string& resource_name)
{
  Impl::PictureInfoMap::iterator it = _d->data.find( StringHelper::hash( resource_name ) );
  if (it == _d->data.end())
  {
    return Point();
    // THROW("Invalid resource name: " << resource_name);
  }

  return (*it).second;
}

PictureInfoBank::~PictureInfoBank()
{

}