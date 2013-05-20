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


#ifndef GFX_SDL_ENGINE_HPP
#define GFX_SDL_ENGINE_HPP

#include "oc3_gfx_engine.hpp"

#include <list>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>

#include "oc3_picture.hpp"


// This is the SDL engine
// It does a dumb drawing from back to front
class GfxSdlEngine : public GfxEngine
{
public:
   GfxSdlEngine();
   ~GfxSdlEngine();
   void init();
   void exit();
   void load_picture(Picture &ioPicture);
   void unload_picture(Picture& ioPicture);

   void init_frame();
   void drawPicture(const Picture &picture, const int dx, const int dy);
   void drawPicture(const Picture &picture, const Point& pos );
   void exit_frame();

   void setTileDrawMask( int rmask, int gmask, int bmask, int amask );
   void resetTileDrawMask();

private:
   SDL_Surface *_screen;
   int _rmask, _gmask, _bmask, _amask;
   Picture _maskedPic;
};

#endif
