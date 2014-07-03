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
#include "IMG_savepng.h"

#include <SDL.h>

namespace gfx
{

static const char* pngType = "PNG";
static const char* bmpType = "BMP";

void PictureConverter::rgbBalance( Picture& dst, const Picture& src, int lROffset, int lGOffset, int lBOffset )
{
    SDL_Surface* source = const_cast< Picture& >( src ).surface();

    SDL_Surface *target = SDL_CreateRGBSurface( SDL_SWSURFACE, source->w, source->h, 32, 
        0x00ff0000, 0x0000ff00, 
        0x000000ff, 0xff000000);

    if (target == NULL) 
    {
        //cerr << "CreateRGBSurface for rgbbalance conversion failed: " << SDL_GetError() << endl;
        return;
    }

    SDL_LockSurface( source );
    SDL_LockSurface(target);
    Uint32* imgpixels = (Uint32*)source->pixels;
    for (int i=0; i<source->h; i++) 
    {
        Uint8 r, g, b, a;
        for (int j=0; j<source->w; j++) 
        {
            Uint32 pixel = imgpixels[ i * source->w + j ];

            a = (pixel >> 24) & 0xff;//gr * ((( clr >> 24 ) & 0xff ) / 255.f );
            r = (pixel >> 16) & 0xff;//* ((( clr >> 16 ) & 0xff ) / 255.f );
            g = (pixel >> 8 ) & 0xff;// * ((( clr >> 8  ) & 0xff ) / 255.f ); 
            b = (pixel & 0xff);

            r = math::clamp<int>( r + lROffset, 0, 255);
            g = math::clamp<int>( g + lGOffset, 0, 255);
            b = math::clamp<int>( b + lBOffset, 0, 255);

            ((Uint32*)target->pixels)[ i * source->w + j ] = ( a << 24 )+ ( r << 16) 
                                                              + ( g << 8 ) + ( b );
        }
    }
    SDL_UnlockSurface(target);
    SDL_UnlockSurface(source);

    if( dst.surface() )
    {
        SDL_FreeSurface( dst.surface() );
    }

    dst.init( target, src.offset() );   
}

void PictureConverter::maskColor( Picture& dst, const Picture& src, int rmask, int gmask, int bmask, int amask )
{
  SDL_Surface* source = const_cast< Picture& >( src ).surface();

  SDL_Surface *target = SDL_CreateRGBSurfaceFrom( src.surface()->pixels, source->w, source->h, 32, src.surface()->pitch,
                                                  rmask, gmask, bmask, amask );

  if (target == NULL) 
  {
    //cerr << "CreateRGBSurface for rgbbalance conversion failed: " << SDL_GetError() << endl;
    return;
  }

  if( dst.surface() )
  {
    SDL_FreeSurface( dst.surface() );
  }

  dst.init( target, src.offset() );
}

void PictureConverter::flipVertical(Picture& pic)
{
  if( !pic.isValid() )
    return;

  unsigned int width = pic.width();
  ScopedPtr<int> tmpLine( new int[ width ] );
  void* pixels = pic.surface()->pixels;

  for( int y=0; y < pic.height()/2; y++ )
  {
    unsigned int* tp = (unsigned int*)pixels + y * width;
    unsigned int* etp = (unsigned int*)pixels + ( pic.height() - y - 1) * width;
    memcpy( tmpLine.data(), tp, width * 4 );
    memcpy( tp, etp, width * 4 );
    memcpy( etp, tmpLine.data(), width * 4 );
  }
}

void PictureConverter::save(Picture& pic, const std::string& filename, const std::string& type)
{
  if( type == pngType )
  {
    IMG_SavePNG( filename.c_str(), pic.surface(), -1 );
  }
  else if( type == bmpType )
  {
    SDL_SaveBMP( pic.surface(), filename.c_str() );
  }
}

ByteArray PictureConverter::save(Picture& pic, const std::string &type)
{
  SDL_RWops *rout;
  ByteArray rdata;

  if( type == bmpType )
  {
    rdata.resize( pic.size().area() * 4 * 2 );

    if(!(rout = SDL_RWFromMem( rdata.data(), rdata.size() ) ) )
    {
      return ByteArray();
    }

    SDL_SaveBMP_RW( pic.surface(), rout, 0 );

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

    IMG_SavePNG_RW( rout, pic.surface(), 0 );

    // Write the bmp out...
    unsigned int size = SDL_RWtell( rout );
    rdata.resize( size );

    // and we're done
    SDL_RWclose(rout);
    return rdata;
  }

  return ByteArray();
}

void PictureConverter::convToGrayscale( Picture& dst, const Picture& src )
{
    SDL_Surface* source = const_cast< Picture& >( src ).surface();

    SDL_Surface *target = SDL_CreateRGBSurface(SDL_SWSURFACE, source->w, source->h, 32,
        0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
   
    if (target == NULL) 
    {
        //cerr << "CreateRGBSurface for grayscale conversion failed: " << SDL_GetError() << endl;
        return;
    }

    SDL_LockSurface( source );
    SDL_LockSurface(target);
    Uint32* imgpixels = (Uint32*)source->pixels;
    for (int i=0; i<source->h; i++) 
    {
        Uint8 r, g, b, a;
        for (int j=0; j<source->w; j++) 
        {
            Uint32 pixel = imgpixels[ i * source->w + j ];
            Uint8 gr = (Uint8)( (((pixel >> 16) & 0xff) * 0.32)
                                 + (((pixel >> 8 ) & 0xff) * 0.55) 
                                 + (((pixel & 0xff) * 0.071)) );
            
            r = gr;//gr * ((( clr >> 24 ) & 0xff ) / 255.f );
            g = gr;//* ((( clr >> 16 ) & 0xff ) / 255.f );
            b = gr;// * ((( clr >> 8  ) & 0xff ) / 255.f ); 
            a = ((pixel>>24)&0xff);// > 0 ? 0xff : 0;
            ((Uint32*)target->pixels)[ i * source->w + j ] = ( a << 24 ) + ( r << 16 )+ ( g << 16) 
                                                              + ( b << 8 );
        }
    }
    SDL_UnlockSurface(target);
    SDL_UnlockSurface(source);

    if( dst.surface() )
    {
        SDL_FreeSurface( dst.surface() );
    }

    dst.init( target, src.offset() );
}

PictureConverter::~PictureConverter()
{

}

PictureConverter::PictureConverter()
{

}

/*
example for transparent text 
void SetSurfaceAlpha (SDL_Surface *surface, Uint8 alpha)
{
SDL_PixelFormat* fmt = surface->format;

// If surface has no alpha channel, just set the surface alpha.
if( fmt->Amask == 0 ) {
SDL_SetAlpha( surface, SDL_SRCALPHA, alpha );
}
// Else change the alpha of each pixel.
else {
unsigned bpp = fmt->BytesPerPixel;
// Scaling factor to clamp alpha to [0, alpha].
float scale = alpha / 255.0f;

SDL_LockSurface(surface);

for (int y = 0; y < surface->h; ++y) 
for (int x = 0; x < surface->w; ++x) {
// Get a pointer to the current pixel.
Uint32* pixel_ptr = (Uint32 *)( 
(Uint8 *)surface->pixels
+ y * surface->pitch
+ x * bpp
);

// Get the old pixel components.
Uint8 r, g, b, a;
SDL_GetRGBA( *pixel_ptr, fmt, &r, &g, &b, &a );

// Set the pixel with the new alpha.
*pixel_ptr = SDL_MapRGBA( fmt, r, g, b, scale * a );
}   

SDL_UnlockSurface(surface);
}       
}           */

}//end namespace gfx
