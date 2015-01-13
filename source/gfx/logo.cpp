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

#include "logo.hpp"
#include "picture_bank.hpp"

namespace splash
{

//static unsigned int width = 640;
//static unsigned int height = 480;

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */
/*  GIMP header image file format (RGB): /home/dalerank/logo.h  */
#define PIXEL(r,pixel) {\
pixel[2] = (((r[0] - 33) << 2) | ((r[1] - 33) >> 4)); \
pixel[1] = ((((r[1] - 33) & 0xF) << 4) | ((r[2] - 33) >> 2)); \
pixel[0] = ((((r[2] - 33) & 0x3) << 6) | ((r[3] - 33))); \
pixel[3] = 0xff;\
r += 4; \
}
static const char* const data = "non image";


void initialize( const std::string& name)
{
  /*char const* ptr = data;
  std::vector<unsigned long> pixels;
  pixels.resize( width * height );

  for( unsigned int y = 0; y < height; y++ )
  {
    for( unsigned int x=0; x < width; x++ )
    {
      pixels[ y * width + x ] = NColor( *(long*)ptr ).abgr();
      ptr+=4;
    }
  }
  gfx::Picture* pic = gfx::Picture::create( Size( width, height ), (unsigned char*)&pixels[0] );
  gfx::PictureBank::instance().setPicture( name, *pic );
  */
}

}//end namespace splash
