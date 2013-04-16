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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com



#include <sdl_facade.hpp>

#include <gfx_engine.hpp>
#include <exception.hpp>


SdlFacade* SdlFacade::_instance = NULL;

SdlFacade& SdlFacade::instance()
{
   if (_instance == NULL)
   {
      _instance = new SdlFacade();
      if (_instance == NULL) THROW("Memory error, cannot instantiate object");
   }
   return *_instance;
}


////////////////////////////
// IMAGE CREATION METHODS
////////////////////////////

void SdlFacade::deletePicture(Picture &pic)
{
   GfxEngine::instance().unload_picture(pic);
   delete &pic;
   _createdPics.remove(&pic);
}

Picture& SdlFacade::createPicture(const int width, const int height)
{
   SDL_Surface* img;
   const Uint32 flags = 0;
   img = SDL_CreateRGBSurface(flags, width, height, 32, 0, 0, 0, 0);  // opaque picture with default mask
   if (img == NULL) THROW("Cannot make surface, size=" << width << "x" << height);

   Picture *pic = new Picture();
   pic->init(img, 0, 0);  // no offset

   _createdPics.push_back(pic);
   return *_createdPics.back();
}


Picture& SdlFacade::copyPicture(const Picture &pic)
{
   int width = pic.get_width();
   int height = pic.get_height();

   SDL_Surface* img;
   const SDL_PixelFormat& fmt = *(pic.get_surface()->format);
   const Uint32 flags = 0;
   img = SDL_CreateRGBSurface(flags, width, height, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask );
   if (img == NULL) THROW("Cannot make surface, size=" << width << "x" << height);
   drawImage(pic.get_surface(), img, 0, 0);

   Picture *newpic = new Picture();
   newpic->init(pic.get_surface(), pic.get_xoffset(), pic.get_yoffset());

   _createdPics.push_back(newpic);
   return *_createdPics.back();
}


SDL_Surface* SdlFacade::createSurface(Uint32 flags, const int width, const int height, const SDL_Surface* display)
{
   SDL_Surface* res;
   const SDL_PixelFormat& fmt = *(display->format);
   res = SDL_CreateRGBSurface(flags, width, height, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask );
   if (res == NULL) THROW("Cannot make surface, size=" << width << "x" << height);
   return res;
}


////////////////////////////
// IMAGE DRAWING METHODS
////////////////////////////

void SdlFacade::drawPicture(const Picture &srcpic, Picture &dstpic, const int dx, const int dy)
{
   SDL_Surface *srcimg = srcpic.get_surface();
   SDL_Surface *dstimg = dstpic.get_surface();
   drawImage(srcimg, dstimg, dx + srcpic.get_xoffset(), dy - srcpic.get_yoffset());
}


void SdlFacade::drawPicture(const Picture &srcpic, SDL_Surface *dstimg, const int dx, const int dy)
{
   SDL_Surface *srcimg = srcpic.get_surface();
   drawImage(srcimg, dstimg, dx + srcpic.get_xoffset(), dy - srcpic.get_yoffset());
}


void SdlFacade::drawImage(SDL_Surface *srcimg, SDL_Surface *dstimg, const int dx, const int dy)
{
    if( srcimg )
	    drawImage(srcimg, 0, 0, srcimg->w, srcimg->h, dstimg, dx, dy);
}


void SdlFacade::drawImage(SDL_Surface *srcimg, const int sx, const int sy, const int sw, const int sh, SDL_Surface *dstimg, const int dx, const int dy)
{
   SDL_Rect src, dst;

   src.x = sx;
   src.y = sy;
   src.w = sw;
   src.h = sh;
   dst.x = dx;
   dst.y = dy;
   dst.w = src.w;
   dst.h = src.h;
   SDL_BlitSurface(srcimg, &src, dstimg, &dst);
}


////////////////////////////
// TEXT METHODS
////////////////////////////

void SdlFacade::drawText(Picture &dstpic, const std::string &text, const int dx, const int dy, Font &font)
{
   TTF_Font* ttf = &font.getTTF();
   SDL_Color color = font.getColor();
   SDL_Surface* sText = TTF_RenderUTF8_Blended( ttf, text.c_str(), color );
   drawImage(sText, dstpic.get_surface(), dx, dy);
   SDL_FreeSurface( sText );
}

void SdlFacade::getTextSize(Font &font, const std::string &text, int &width, int &height)
{
   TTF_Font *ttf = &font.getTTF();
   TTF_SizeUTF8(ttf, text.c_str(), &width, &height);
}


////////////////////////////
// PIXEL ACCESS METHODS
////////////////////////////


void SdlFacade::lockSurface(SDL_Surface *surface)
{
   if (SDL_MUSTLOCK(surface))
   {
      int rc = SDL_LockSurface(surface);
      if (rc < 0) THROW("Cannot lock surface: " << SDL_GetError());
   }
}

void SdlFacade::unlockSurface(SDL_Surface *surface)
{
   if (SDL_MUSTLOCK(surface))
   {
      SDL_UnlockSurface(surface);
   }
}


Uint32 SdlFacade::get_pixel(SDL_Surface *img, const int x, const int y)
{
   Uint32 res = 0;
   switch (img->format->BytesPerPixel)
   {
   case 1:
      // 8bpp
      Uint8 *bufp8;
      bufp8 = (Uint8 *)img->pixels + y*img->pitch + x;
      res = *bufp8;
      break;

   case 2:
      // 15bpp or 16bpp
      Uint16 *bufp16;
      bufp16 = (Uint16 *)img->pixels + y*img->pitch/2 + x;
      res = *bufp16;
      break;

   case 3:
      // 24bpp, very slow!
      THROW("Unsupported graphic mode 24bpp");
      break;

   case 4:
      // 32bpp
      Uint32 *bufp32;
      bufp32 = (Uint32 *)img->pixels + y*img->pitch/4 + x;
      res = *bufp32;
      break;
   }

   return res;
}


void SdlFacade::set_pixel(SDL_Surface *img, const int x, const int y, const Uint32 color)
{
   switch (img->format->BytesPerPixel)
   {
   case 1:
      // 8bpp
      Uint8 *bufp8;
      bufp8 = (Uint8 *)img->pixels + y*img->pitch + x;
      *bufp8 = color;
      break;

   case 2:
      // 15bpp or 16bpp
      Uint16 *bufp16;
      bufp16 = (Uint16 *)img->pixels + y*img->pitch/2 + x;
      *bufp16 = color;
      break;

   case 3:
      // 24bpp, very slow!
      THROW("Unsupported graphic mode 24bpp");
      break;

   case 4:
      // 32bpp
      Uint32 *bufp32;
      bufp32 = (Uint32 *)img->pixels + y*img->pitch/4 + x;
      *bufp32 = color;
      break;
   }

}


void SdlFacade::color_or(SDL_Surface *img, const SDL_Color &color_sub)
{
   lockSurface(img);

   Uint32 amount = SDL_MapRGBA(img->format, color_sub.r, color_sub.g, color_sub.b, 0);
   for (int y = 0; y < img->h; ++y)
   {
      for (int x = 0; x < img->w; ++x)
      {
         Uint32 color = get_pixel(img, x, y);
         set_pixel(img, x, y, color | amount);
      }
   }

   unlockSurface(img);
}


