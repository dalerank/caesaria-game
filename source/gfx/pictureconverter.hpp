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

#ifndef __CAESARIA_PICTURECONVERTER_H_INCLUDE_
#define __CAESARIA_PICTURECONVERTER_H_INCLUDE_

#include "core/rectangle.hpp"
#include "core/bytearray.hpp"
#include "picture.hpp"

namespace gfx
{

class PictureConverter
{
public:
  static void convToGrayscale( Picture& dst, Picture& src );
  static void rgbBalance(Picture& dst, Picture& src, int lROffset, int lGOffset, int lBOffset );
  static void maskColor( const Picture& dst, const Picture& src, unsigned char r=0xff, unsigned char g=0xff, unsigned char b=0xff, unsigned char a=0xff);
  static void flipVertical( Picture& pic );
  static ByteArray save( Picture& pic, const std::string& type );
  static void save(Picture& pic, const std::string& filename , const std::string& type);

private:
  PictureConverter();
  ~PictureConverter();
};

}//end namespace gfx
#endif //__CAESARIA_PICTURECONVERTER_H_INCLUDE_
