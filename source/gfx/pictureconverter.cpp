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

#include "pictureconverter.hpp"

#include "picture.hpp"
#include "core/math.hpp"
#include "core/position.hpp"
#include "core/logger.hpp"
#include "IMG_savepng.h"

#include <SDL.h>

namespace gfx
{

static const char* pngType = "PNG";
static const char* bmpType = "BMP";

void PictureConverter::rgbBalance( Picture& dst, Picture& src, int lROffset, int lGOffset, int lBOffset )
{
    const unsigned int* source = src.lock();
    unsigned int* target = dst.lock();

    if( !dst.isValid() )
    {
      Logger::warning( "Rgbbalance conversion failed: %s", SDL_GetError() );
      return;
    }

    for (int i=0; i< src.height(); i++)
    {
        Uint8 r, g, b, a;
        for (int j=0; j< src.width(); j++)
        {
            Uint32 pixel = source[ i * src.width() + j ];

            a = (pixel >> 24) & 0xff;//gr * ((( clr >> 24 ) & 0xff ) / 255.f );
            r = (pixel >> 16) & 0xff;//* ((( clr >> 16 ) & 0xff ) / 255.f );
            g = (pixel >> 8 ) & 0xff;// * ((( clr >> 8  ) & 0xff ) / 255.f ); 
            b = (pixel & 0xff);

            r = math::clamp<int>( r + lROffset, 0, 255);
            g = math::clamp<int>( g + lGOffset, 0, 255);
            b = math::clamp<int>( b + lBOffset, 0, 255);

            target[ i * src.width() + j ] = ( a << 24 )+ ( r << 16)
                                                              + ( g << 8 ) + ( b );
        }
    }
    dst.unlock();
    src.unlock();
}

void PictureConverter::save(Picture& pic, const std::string& filename, const std::string& type)
{
  SDL_Surface* srf = SDL_CreateRGBSurfaceFrom( pic.lock(), pic.width(), pic.height(), 32, pic.width() * 4, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );

  if( type == pngType )
  {
    IMG_SavePNG( filename.c_str(), srf, -1 );
  }
  else if( type == bmpType )
  {
    SDL_SaveBMP( srf, filename.c_str() );
  }
  SDL_FreeSurface( srf );
}

ByteArray PictureConverter::save(Picture& pic, const std::string &type)
{
  SDL_Surface* srf = SDL_CreateRGBSurfaceFrom( pic.lock(), pic.width(), pic.height(), 32, pic.width() * 4, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );

  SDL_RWops *rout;
  ByteArray rdata;

  if( type == bmpType )
  {
    rdata.resize( pic.size().area() * 4 * 2 );

    if(!(rout = SDL_RWFromMem( rdata.data(), rdata.size() ) ) )
    {
      return ByteArray();
    }

    SDL_SaveBMP_RW( srf, rout, 0 );

    // Write the bmp out...
    unsigned int size = *(unsigned int*)(&rdata[2]);
    rdata.resize( size );

    // and we're done
    SDL_RWclose(rout);
    return rdata;
  }
  else if( type == pngType )
  {
    rdata.resize( pic.size().area() * 4 * 2 );

    if(!(rout = SDL_RWFromMem( rdata.data(), rdata.size() ) ) )
    {
      return ByteArray();
    }

    IMG_SavePNG_RW( rout, srf, 0 );

    // Write the bmp out...
    unsigned int size = SDL_RWtell( rout );
    rdata.resize( size );

    // and we're done
    SDL_RWclose(rout);
    return rdata;
  }
  SDL_FreeSurface( srf );
  return ByteArray();
}

void PictureConverter::convToGrayscale( Picture& dst, Picture& src )
{
  if( !dst.isValid() )
  {
      //cerr << "CreateRGBSurface for grayscale conversion failed: " << SDL_GetError() << endl;
      return;
  }

  unsigned int* source = src.lock();
  unsigned int* target = dst.lock();

  for (int i=0; i<src.height(); i++)
  {
      Uint8 r, g, b, a;
      for (int j=0; j<src.height(); j++)
      {
          Uint32 pixel = source[ i * src.width() + j ];
          Uint8 gr = (Uint8)( (((pixel >> 16) & 0xff) * 0.32)
                               + (((pixel >> 8 ) & 0xff) * 0.55)
                               + (((pixel & 0xff) * 0.071)) );

          r = gr;//gr * ((( clr >> 24 ) & 0xff ) / 255.f );
          g = gr;//* ((( clr >> 16 ) & 0xff ) / 255.f );
          b = gr;// * ((( clr >> 8  ) & 0xff ) / 255.f );
          a = ((pixel>>24)&0xff);// > 0 ? 0xff : 0;
          target[ i * src.width() + j ] = ( a << 24 ) + ( r << 16 )+ ( g << 16)
                                                            + ( b << 8 );
      }
  }

  dst.unlock();
  src.unlock();
}

void PictureConverter::flipVertical(Picture& pic)
{
  if( !pic.isValid() )
    return;

  unsigned int width = pic.width();
  std::vector<int> tmpLine;
  tmpLine.resize( width );

  void* pixels = pic.lock();
  if( pixels )
  {
    for( int y=0; y < pic.height()/2; y++ )
    {
      unsigned int* tp = (unsigned int*)pixels + y * width;
      unsigned int* etp = (unsigned int*)pixels + ( pic.height() - y - 1) * width;
      memcpy( &tmpLine[0], tp, width * 4 );
      memcpy( tp, etp, width * 4 );
      memcpy( etp, &tmpLine[0], width * 4 );
    }
  }

  pic.unlock();
}


PictureConverter::~PictureConverter() {}

PictureConverter::PictureConverter() {}

}//end namespace gfx
