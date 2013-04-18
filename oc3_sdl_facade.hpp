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


#ifndef SDL_FACADE_HPP
#define SDL_FACADE_HPP


#include <list>
#include <picture.hpp>
#include <string>


/* Provides usefull functions for SDL */
class SdlFacade
{
public:
   static SdlFacade& instance();

   // deletes a picture (deallocate memory)
   void deletePicture(Picture &pic);
   // creates a picture with the given size, it will need to be loaded by the graphic engine
   virtual Picture& createPicture(int width, int height);
   // creates a copy of the given picture, it will need to be loaded by the graphic engine
   virtual Picture& copyPicture(const Picture &pic);
   // creates a surface with the same format than "display", for fast blitting
   SDL_Surface* createSurface(Uint32 flags, const int width, const int height, const SDL_Surface* display);

   // draw an image on a SDL surface
   void drawPicture(const Picture &srcpic, Picture &dstpic, int dx, int dy);
   void drawPicture(const Picture &pic, SDL_Surface *dstimg, const int dx, const int dy);
   void drawImage(SDL_Surface *srcimg, const int sx, const int sy, const int sw, const int sh, SDL_Surface *dstimg, const int dx, const int dy);
   void drawImage(SDL_Surface *srcimg, SDL_Surface *dstimg, const int dx, const int dy);

   // draws a line of text on a picture
   void drawText(Picture &dstpic, const std::string &text, const int dx, const int dy, Font &font);

   // returns the size of a text line
   void getTextSize(Font &font, const std::string &text, int &width, int &height);

   // lock/unlock the given surface for pixel access
   void lockSurface(SDL_Surface *surface);
   void unlockSurface(SDL_Surface *surface);

   // Uint32 is the pixel color in the surface format. The surface must be locked!!!
   Uint32 get_pixel(SDL_Surface *img, const int x, const int y);
   void set_pixel(SDL_Surface *img, const int x, const int y, const Uint32 color);

   // changes the pixels: perform a logical OR (used to transform black to some other color, without touching the alpha value)
   void color_or(SDL_Surface *img, const SDL_Color &color);
protected:
   std::list<Picture*> _createdPics;  // list of all pictures created by the sdl_facade

   static SdlFacade *_instance;
};


#endif
